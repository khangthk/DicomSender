#include "storegdcm.h"

#include <gdcmReader.h>
#include <gdcmAttribute.h>
#include <gdcmEvent.h>
#include <gdcmULConnectionManager.h>
#include <gdcmSimpleSubjectWatcher.h>
#include <gdcmProgressEvent.h>
#include <gdcmPresentationContextGenerator.h>

#include <QDebug>
#include <QFileInfo>

using namespace gdcm;

class StoreWatcher : public SimpleSubjectWatcher
{
public:
    StoreWatcher(Subject *s, const char *comment = "cstore", StoreBase *parent = nullptr, const size_t n = 1)
        :SimpleSubjectWatcher(s, comment), m_totalFile(n),
        m_percent(0), m_progress(0.0), m_refProgress(0.0), m_parent(parent)
    {}

    void ShowIteration() override
    {
        m_refProgress = m_progress;
    }

    void ShowProgress(Subject *caller, const Event &evt) override
    {
        Q_UNUSED(caller);
        const ProgressEvent &pe = dynamic_cast<const ProgressEvent&>(evt);
        m_progress = m_refProgress + (100.0 / m_totalFile) * pe.GetProgress();
        //qDebug() << "Progress: " << m_progress;
        UpdatePercent();
    }

    void ShowDataSet(Subject *caller, const Event &evt) override
    {
        Q_UNUSED(caller);
        Q_UNUSED(evt);
    }

    void UpdateProgressWhenError()
    {
        m_progress += 100.0 / m_totalFile;
        m_refProgress = m_progress;
        UpdatePercent();
    }

    void UpdatePercent()
    {
        int newPercent = static_cast<int>(m_progress + 0.5);
        if (newPercent != m_percent)
        {
            m_percent = newPercent;
            qDebug() << "Percent: " << m_percent;
            emit m_parent->progress(m_percent);
        }
    }

private:
    size_t m_totalFile;
    int m_percent;
    double m_progress;
    double m_refProgress;
    StoreBase *m_parent;

signals:
    void progress(int);
};


StoreGdcm::StoreGdcm(QObject *parent)
    : StoreBase(parent)
{}

StoreGdcm::~StoreGdcm()
{}

void StoreGdcm::store()
{
    auto sendFiles = files();
    auto total = sendFiles.size();

    Directory::FilenamesType fileList;
    fileList.reserve(total);

    for (auto &file : sendFiles)
    {
        fileList.push_back(file.toStdString());
    }

    SmartPointer<network::ULConnectionManager> ps = new network::ULConnectionManager;
    network::ULConnectionManager &theManager = *ps;

    StoreWatcher watcher(ps, "cstore", this, sendFiles.size());

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
    int i = 0;

    for (i = 0; i < total; ++i)
    {
        try
        {
            QString fileName = QFileInfo(sendFiles.at(i)).fileName();
            fileErr = fileName;
            qDebug() << QString("Processing[%1/%2]: %3").arg(i + 1).arg(total).arg(fileName);

            Reader reader;
            reader.SetFileName(sendFiles.at(i).toUtf8());
            if (!reader.Read())
            {
                emit processed(qMakePair(i + 1, total), false, QString("Can not read \"%1\"").arg(fileName));
                error = true;
                watcher.UpdateProgressWhenError();
                continue;
            }

            const File& file = reader.GetFile();
            std::vector<DataSet> theDataSets;
            theDataSets = theManager.SendStore(file);
            if (theDataSets.empty())
            {
                emit processed(qMakePair(i + 1, total), false, QString("Can not send \"%1\"").arg(fileName));
                error = true;
                watcher.UpdateProgressWhenError();
                continue;
            }

            assert(theDataSets.size() == 1);
            const DataSet &ds = theDataSets[0];
            assert(ds.FindDataElement(Tag(0x0, 0x0900)));
            const DataElement& de = ds.GetDataElement(Tag(0x0, 0x0900));
            Attribute<0x0, 0x0900> at;
            at.SetFromDataElement(de);
            // PS 3.4 - 2011
            // Table W.4-1 C-STORE RESPONSE STATUS VALUES
            const uint16_t theVal = at.GetValue();
            // http://dicom.nema.org/medical/dicom/current/output/chtml/part07/chapter_C.html
            if (theVal == 0x0) // Success
            {
                emit processed(qMakePair(i + 1, total), true,
                    QString("Send \"%1\" successfully").arg(fileName));
                error = false;
            }
            else if (theVal == 0x0001 || (theVal & 0xf000) == 0xb000) // Warning
            {
                emit processed(qMakePair(i + 1, total), true,
                    QString("Send \"%1\" successfully but with warning").arg(fileName));
                error = false;
            }
            else if ((theVal & 0xf000) == 0xa000 || (theVal & 0xf000) == 0xc000) // Failure
            {
                Attribute<0x0, 0x0902> errormsg;
                errormsg.SetFromDataSet(ds);
                const char* themsg = errormsg.GetValue();
                assert(themsg); (void)themsg;
                emit processed(qMakePair(i + 1, total), false,
                    QString("Send \"%1\" failed. Response Status: %2").arg(fileName).arg(QString(themsg)));
                error = true;
                watcher.UpdateProgressWhenError();
            }
            else
            {
                emit processed(qMakePair(i + 1, total), false,
                    QString("Send \"%1\" failed. Unhandled error code: %2").arg(fileName).arg(QString(theVal)));
                error = true;
                watcher.UpdateProgressWhenError();
            }

            theManager.InvokeEvent(IterationEvent());
        }
        catch (Exception ex)
        {
            // If you reach here this is basically because GDCM does not support encoding other
            // than raw transfer syntax (Little Endian Explicit/Implicit...)
            emit processed(qMakePair(i + 1, total), false,
                QString("Send \"%1\" failed. Error: %2").arg(fileErr).arg(QString(ex.GetDescription())));
            error = true;
            watcher.UpdateProgressWhenError();
        }
        catch (...)
        {
            emit processed(qMakePair(i + 1, total), false, QString("Send \"%1\" failed").arg(fileErr));
            error = true;
            watcher.UpdateProgressWhenError();
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
