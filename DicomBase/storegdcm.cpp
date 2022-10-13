#include "storegdcm.h"

#include <gdcmReader.h>
#include <gdcmAttribute.h>
#include <gdcmEvent.h>
#include <gdcmULConnectionManager.h>
#include <gdcmSimpleSubjectWatcher.h>
#include <gdcmProgressEvent.h>
#include <gdcmPresentationContextGenerator.h>

#include <utility>

#include <QDebug>
#include <QFileInfo>

using namespace gdcm;

class StoreWatcher : public SimpleSubjectWatcher
{
public:
    StoreWatcher(Subject *s, const char *comment = "cstore", StoreGdcm *parent = nullptr)
        :SimpleSubjectWatcher(s, comment), m_totalFile(1), m_totalPercent(0.0f), m_parent(parent)
    {
        if (parent != nullptr)
        {
            auto totalFile = parent->files().size();
            if (totalFile > 0)
            {
                m_totalFile = totalFile;
            }
        }
    }

    void ShowIteration() override
    {}

    void ShowDataSet(Subject *caller, const Event &evt) override
    {
        Q_UNUSED(caller);
        Q_UNUSED(evt);
    }

    void ShowProgress(Subject *caller, const Event &evt) override
    {
        Q_UNUSED(caller);
        if (m_parent != nullptr && m_totalFile > 0)
        {
            const ProgressEvent &pe = dynamic_cast<const ProgressEvent &>(evt);
            m_totalPercent = m_parent->percentOfProcessedFiles() + (100.0f / m_totalFile) * pe.GetProgress();
            int newPercent = static_cast<int>(m_totalPercent + 0.5f);
            if (m_parent->percent() != newPercent)
            {
                m_parent->setPercent(newPercent);
            }
        }
    }

private:
    size_t m_totalFile;
    float m_totalPercent;
    StoreGdcm *m_parent;

signals:
    void progress(int);
};

StoreGdcm::StoreGdcm(QObject *parent)
    : StoreBase(parent)
{}

StoreGdcm::~StoreGdcm()
{
    qDebug() << "~StoreGdcm";
}

void StoreGdcm::store()
{
    auto &sendFiles = files();
    auto total = sendFiles.size();

    Directory::FilenamesType fileList;
    fileList.reserve(total);

    for (auto &file : sendFiles)
    {
        fileList.push_back(file.toStdString());
    }

    SmartPointer<network::ULConnectionManager> ps = new network::ULConnectionManager;
    network::ULConnectionManager &theManager = *ps;

    StoreWatcher watcher(ps, "cstore", this);

    // Generate the PresentationContext array from the File-Set:
    PresentationContextGenerator generator;
    if (!generator.GenerateFromFilenames(fileList))
    {
        emit done(false, "Failed to generate presentation context");
        return;
    }

    bool connect = true;
    try
    {
        if (!theManager.EstablishConnection(localAE().toStdString(),
                                            targetAE().toStdString(),
                                            host().toStdString(),
                                            0,
                                            port(),
                                            connectionTimeout(),
                                            generator.GetPresentationContexts()))
        {
            connect = false;
        }
    }
    catch (...)
    {
        connect = false;
    }

    if (!connect)
    {
        emit done(false, "Failed to establish connection");
        return;
    }

    QString fileErr;
    bool error = false;
    int index = 0;
    float oneFilePercent = 0.0f;
    if (total > 0)
    {
        oneFilePercent = 100.0f / total;
    }

    for (auto &file : sendFiles)
    {
        ++index;
        try
        {
            QString fileName = QFileInfo(file).fileName();
            fileErr = fileName;
            qDebug().noquote() << QString("--->Processing[%1/%2]: %3").arg(index).arg(total).arg(fileName);

            Reader reader;
            reader.SetFileName(file.toStdString().c_str());
            if (!reader.Read())
            {
                emit processed(std::make_pair(index, total), false, QString("Send \"%1\" failed. Error: Unable to read file.").arg(fileName));
                error = true;
                goto doneOneFile;
            }

            std::vector<DataSet> theDataSets;
            theDataSets = theManager.SendStore(reader.GetFile());
            if (theDataSets.empty())
            {
                emit processed(std::make_pair(index, total), false, QString("Send \"%1\" failed. Error: Unable to send file.").arg(fileName));
                error = true;
                goto doneOneFile;
            }

            assert(theDataSets.size() == 1);
            const DataSet &ds = theDataSets[0];
            assert(ds.FindDataElement(Tag(0x0, 0x0900)));
            const DataElement &de = ds.GetDataElement(Tag(0x0, 0x0900));
            Attribute<0x0, 0x0900> at{};
            at.SetFromDataElement(de);
            // PS 3.4 - 2011
            // Table W.4-1 C-STORE RESPONSE STATUS VALUES
            const uint16_t theVal = at.GetValue();
            // http://dicom.nema.org/medical/dicom/current/output/chtml/part07/chapter_C.html
            if (theVal == 0x0) // Success
            {
                emit processed(std::make_pair(index, total), true, QString("Send \"%1\" successfully").arg(fileName));
                error = false;
            }
            else if (theVal == 0x0001 || (theVal & 0xf000) == 0xb000) // Warning
            {
                emit processed(std::make_pair(index, total), true, QString("Send \"%1\" successfully but with warning").arg(fileName));
                error = false;
            }
            else if ((theVal & 0xf000) == 0xa000 || (theVal & 0xf000) == 0xc000) // Failure
            {
                Attribute<0x0, 0x0902> errormsg;
                errormsg.SetFromDataSet(ds);
                const char *themsg = errormsg.GetValue();
                assert(themsg); (void)themsg;
                emit processed(std::make_pair(index, total), false, QString("Send \"%1\" failed. Response Status: %2").arg(fileName).arg(QString(themsg)));
                error = true;
            }
            else
            {
                emit processed(std::make_pair(index, total), false, QString("Send \"%1\" failed. Unhandled error code: %2").arg(fileName).arg(QString(theVal)));
                error = true;
            }

            theManager.InvokeEvent(IterationEvent());
        }
        catch (Exception ex)
        {
            // If you reach here this is basically because GDCM does not support encoding other
            // than raw transfer syntax (Little Endian Explicit/Implicit...)
            emit processed(std::make_pair(index, total), false, QString("Send \"%1\" failed. Error: %2").arg(fileErr).arg(QString(ex.GetDescription())));
            error = true;
        }
        catch (...)
        {
            emit processed(std::make_pair(index, total), false, QString("Send \"%1\" failed").arg(fileErr));
            error = true;
        }

    doneOneFile:
        setPercentOfProcessedFiles(oneFilePercent * index);
        int newPercent = static_cast<int>(percentOfProcessedFiles() + 0.5f);
        if (percent() != newPercent)
        {
            setPercent(newPercent);
        }

        if (stopWhenError() && error)
        {
            break;
        }
    }

    try
    {
        theManager.BreakConnection(-1); // wait for a while for the connection to break, ie, infinite
    }
    catch (...)
    {}
}
