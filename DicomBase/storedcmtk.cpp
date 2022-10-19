#include "storedcmtk.h"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmnet/dcmtrans.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdatutl.h>
#include <dcmtk/dcmdata/dcvrui.h>

#include <vector>
#include <string>
#include <utility>

#include <QDebug>
#include <QString>
#include <QFileInfo>

class NetworkSentry
{
public:
    NetworkSentry() { OFStandard::initializeNetwork(); }
    ~NetworkSentry() { OFStandard::shutdownNetwork(); }
};

StoreDcmtk::StoreDcmtk(QObject *parent)
    : StoreBase(parent), m_qDebugDelay(QtDebugMsg)
{}

StoreDcmtk::~StoreDcmtk()
{
    qDebug() << "~StoreDcmtk";
}

QDebug &StoreDcmtk::qDebugDelay()
{
    return m_qDebugDelay;
}

void StoreDcmtk::resetQDebugDelay()
{
    m_qDebugDelay = QDebug(QtDebugMsg);
}

void StoreDcmtk::store()
{
    auto sendFiles = files();
    if (sendFiles == nullptr || sendFiles->empty())
    {
        emit result(false, "No files to send");
        return;
    }

    T_ASC_Network *net;
    T_ASC_Parameters *params;
    T_ASC_Association *assoc;
    OFString temp_str;
    NetworkSentry sentry;

    /* always set the timeout values since the global default might be different */
    dcmConnectionTimeout.set(static_cast<Sint32>(connectionTimeout()));
    dcmSocketSendTimeout.set(static_cast<Sint32>(socketTimeout()));
    dcmSocketReceiveTimeout.set(static_cast<Sint32>(socketTimeout()));

    /* set the global max send PDU */
    dcmMaxOutgoingPDUSize.set(static_cast<Uint32>(maxSendPDU()));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        qDebug() << "No data dictionary loaded";
    }

    /* initialize network, i.e. create an instance of T_ASC_Network*. */
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, ACSETimeout(), &net);
    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit result(false, QString(temp_str.c_str()));
        return;
    }

    /* initialize association parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, maxReceivePDU(), dcmConnectionTimeout.get());
    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit result(false, QString(temp_str.c_str()));
        return;
    }

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, localAE().toStdString().c_str(), targetAE().toStdString().c_str(), nullptr);

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), QString("%1:%2").arg(host()).arg(port()).toStdString().c_str());

    /* Add presentation context to params */
    cond = addPresentationContext(params);

    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit result(false, QString(temp_str.c_str()));
        return;
    }

    /* dump presentation contexts */
    qDebug() << "Request Parameters:" << Qt::endl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ).c_str();

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    qDebug() << "Requesting Association";
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad())
    {
        QString log;
        if (cond == DUL_ASSOCIATIONREJECTED)
        {
            T_ASC_RejectParameters rej;
            ASC_getRejectParameters(params, &rej);
            ASC_printRejectParameters(temp_str, &rej);
            log = "Association Rejected:\n" + QString(temp_str.c_str());
            emit result(false, log);
            return;
        }
        else
        {
            DimseCondition::dump(temp_str, cond);
            log = "Association Request Failed: " + QString(temp_str.c_str());
            emit result(false, log);
            return;
        }
    }

    /* dump the connection parameters if in debug mode*/
    qDebug() << ASC_dumpConnectionParameters(temp_str, assoc).c_str();

    /* dump the presentation contexts which have been accepted/refused */
    qDebug() << "Association Parameters Negotiated:" << Qt::endl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC).c_str();

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0)
    {
        emit result(false, "No Acceptable Presentation Contexts");
        return;
    }

    /* dump general information concerning the establishment of the network connection*/
    qDebug() << "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")";

    /* send files to the SCP */
    auto total = sendFiles->size();
    int index = 0;
    float oneFilePercent = 0.0f;
    if (total > 0)
    {
        oneFilePercent = 100.0f / total;
    }

    for (auto &file : *sendFiles)
    {
        ++index;
        QString fileName = QFileInfo(file).fileName();
        qDebug().noquote() << QString("--->Processing[%1/%2]: %3").arg(index).arg(total).arg(fileName);

        cond = storeSCU(assoc, file);
        if (cond.good())
        {
            emit processed(qMakePair(index, total), true, QString("Send \"%1\" successfully").arg(fileName));
        }
        else
        {
            DimseCondition::dump(temp_str, cond);
            emit processed(qMakePair(index, total), false, QString("Send \"%1\" failed. Error: %2").arg(fileName, temp_str.c_str()));
        }

        setPercentOfProcessedFiles(oneFilePercent * index);
        int newPercent = static_cast<int>(percentOfProcessedFiles() + 0.5f);
        if (percent() != newPercent)
        {
            setPercent(newPercent);
        }

        if (stopWhenError() && cond.bad())
        {
            break;
        }
    }

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        /* release association */
        qDebug() << "Releasing Association";
        cond = ASC_releaseAssociation(assoc);
        if (cond.bad())
        {
            DimseCondition::dump(temp_str, cond);
            emit result(false, "Association Release Failed: " + QString(temp_str.c_str()));
            return;
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        qDebug() << "Protocol Error: Peer requested release (Aborting)";
        qDebug() << "Aborting Association";
        cond = ASC_abortAssociation(assoc);
        if (cond.bad())
        {
            DimseCondition::dump(temp_str, cond);
            emit result(false, "Association Abort Failed: " + QString(temp_str.c_str()));
            return;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        emit result(false, "Peer Aborted Association");
        return;
    }
    else
    {
        DimseCondition::dump(temp_str, cond);
        qDebug() << "Store SCU Failed: " << temp_str.c_str();
        qDebug() << "Aborting Association";
        cond = ASC_abortAssociation(assoc);
        if (cond.bad())
        {
            DimseCondition::dump(temp_str, cond);
            emit result(false, "Association Abort Failed: " + QString(temp_str.c_str()));
            return;
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit result(false, QString(temp_str.c_str()));
        return;
    }

    /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
    /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
    cond = ASC_dropNetwork(&net);
    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit result(false, QString(temp_str.c_str()));
        return;
    }
}

/* refer to DcmStorageSCU::checkSOPInstance */
OFCondition StoreDcmtk::checkSOPInstance(const OFString &sopClassUID, const OFString &sopInstanceUID,
                                         const OFString &transferSyntaxUID, const bool checkValues)
{
    OFCondition status = EC_Normal;
    /* check SOP class UID for validity */
    if (sopClassUID.empty())
    {
        qDebug() << "Empty or missing SOP Class UID";
        status = NET_EC_InvalidSOPClassUID;
    }
    /* check value representation and value multiplicity (if not disabled) */
    if (status.good() && checkValues)
    {
        status = DcmUniqueIdentifier::checkStringValue(sopClassUID, "1" /* vm */);
        if (status.good())
        {
            /* in addition, check whether it is a known storage SOP class */
            if (!dcmIsaStorageSOPClassUID(sopClassUID.c_str(), ESSC_All))
            {
                /* check whether the DICOM standard prefix for storage UIDs is used */
                if (sopClassUID.compare(0, 23, "1.2.840.10008.5.1.4.1.1") == 0)
                {
                    qDebug() << "unknown storage SOP class (with standard UID prefix): " << sopClassUID.c_str();
                }
                /* check whether the DICOM standard prefix for UIDs is used */
                else if (sopClassUID.compare(0, 13, "1.2.840.10008") == 0)
                {
                    /* try to find the human-readable name of this UID */
                    const char *uidName = dcmFindNameOfUID(sopClassUID.c_str(), nullptr /* defaultValue */);
                    if (uidName != nullptr)
                    {
                        qDebug() << "unexpected standard UID (no storage SOP class): " << sopClassUID.c_str() << " (" << uidName << ")";
                        status = NET_EC_InvalidSOPClassUID;
                    }
                    else
                    {
                        qDebug() << "unknown standard UID (probably no storage SOP class): " << sopClassUID.c_str();
                        status = NET_EC_UnknownStorageSOPClass;
                    }
                }
                else
                {
                    /* do not reject supposed private SOP classes */
                    qDebug() << "unknown UID (possibly private storage SOP class): " << sopClassUID.c_str();
                }
            }
        }
        else
        {
            qDebug() << "invalid SOP Class UID " << sopClassUID.c_str() << ": " << status.text();
            status = NET_EC_InvalidSOPClassUID;
        }
    }
    /* check SOP instance UID for validity */
    if (status.good())
    {
        if (sopInstanceUID.empty())
        {
            qDebug() << "empty or missing SOP Instance UID";
            status = NET_EC_InvalidSOPInstanceUID;
        }
        /* check value representation and value multiplicity (if not disabled) */
        if (status.good() && checkValues)
        {
            status = DcmUniqueIdentifier::checkStringValue(sopInstanceUID, "1" /* vm */);
            if (status.bad())
            {
                qDebug() << "invalid SOP Instance UID " << sopInstanceUID.c_str() << ": " << status.text();
                status = NET_EC_InvalidSOPInstanceUID;
            }
        }
    }
    /* check transfer syntax UID for validity */
    if (status.good())
    {
        if (transferSyntaxUID.empty())
        {
            qDebug() << "empty or missing Transfer Syntax UID";
            status = NET_EC_InvalidTransferSyntaxUID;
        }
        /* check value representation and value multiplicity (if not disabled) */
        if (status.good() && checkValues)
        {
            status = DcmUniqueIdentifier::checkStringValue(transferSyntaxUID, "1" /* vm */);
            if (status.good())
            {
                DcmXfer xfer(transferSyntaxUID.c_str());
                /* in addition, check whether it is a retired or unknown transfer syntax */
                if (xfer.isRetired())
                {
                    qDebug() << "retired transfer syntax: " << transferSyntaxUID.c_str();
                }
                else if (xfer.getXfer() == EXS_Unknown)
                {
                    /* check whether the DICOM standard prefix for transfer syntax UIDs is used */
                    if (sopClassUID.compare(0, 17, "1.2.840.10008.1.2") == 0)
                    {
                        qDebug() << "unknown standard transfer syntax (with standard UID prefix): " << transferSyntaxUID.c_str();
                    }
                    /* check whether the DICOM standard prefix for UIDs is used */
                    else if (sopClassUID.compare(0, 13, "1.2.840.10008") == 0)
                    {
                        qDebug() << "unknown standard UID (probably no transfer syntax): " << transferSyntaxUID.c_str();
                    }
                    else
                    {
                        qDebug() << "unknown UID (possibly private transfer syntax): " << transferSyntaxUID.c_str();
                    }
                    /* by default, we are not supporting unknown transfer syntaxes */
                    status = NET_EC_UnknownTransferSyntax;
                }
            }
            else
            {
                qDebug() << "invalid Transfer Syntax UID " << transferSyntaxUID.c_str() << ": " << status.text();
                status = NET_EC_InvalidTransferSyntaxUID;
            }
        }
    }
    return status;
}

OFCondition StoreDcmtk::addPresentationContext(T_ASC_Parameters *params)
{
    OFCondition cond = EC_Normal;

    auto sendFiles = files();
    if (sendFiles != nullptr && !sendFiles->empty())
    {
        std::vector<std::pair<std::string, std::string>> listPreCtx;
        for (auto &file : *sendFiles)
        {
            /* get relevant information from the DICOM file */
            OFString sopClassUID, sopInstanceUID, transferSyntaxUID;
            cond = DcmDataUtil::getSOPInstanceFromFile(file.toStdWString().c_str(), sopClassUID, sopInstanceUID, transferSyntaxUID, ERM_autoDetect);
            if (cond.good())
            {
                /* check the SOP instance before adding it */
                cond = checkSOPInstance(sopClassUID, sopInstanceUID, transferSyntaxUID, true);
                if (cond.good())
                {
                    auto preCtx = std::make_pair(std::string(sopClassUID.c_str()), std::string(transferSyntaxUID.c_str()));
                    if (std::find(listPreCtx.cbegin(), listPreCtx.cend(), preCtx) == listPreCtx.cend())
                    {
                        listPreCtx.push_back(preCtx);
                    }
                }
                else
                {
                    OFString temp_str;
                    DimseCondition::dump(temp_str, cond);
                    qDebug() << "Error:" << temp_str.c_str();
                }
            }
        }

        unsigned int presContextID = 1;

        for (auto &preCtx : listPreCtx)
        {
            std::vector<const char *> transferSyntaxList;
            transferSyntaxList.push_back(preCtx.second.c_str());
            
            cond = ASC_addPresentationContext(params, presContextID, preCtx.first.c_str(), transferSyntaxList.data(),
                                              static_cast<int>(transferSyntaxList.size()), ASC_SC_ROLE_DEFAULT);
            if (cond.bad())
            {
                return cond;
            }

            presContextID += 2;

            /* limit 128 presentation contexts in total, each with an odd ID */
            if (presContextID >= 256)
            {
                break;
            }
        }

        /* check whether any presentation contexts have been added */
        if ((presContextID == 1) && cond.good())
        {
            cond = NET_EC_NoPresentationContextsDefined;
        }
    }
    else
    {
        cond = NET_EC_NoSOPInstancesToSend;
    }

    return cond;
}

/*
 * This function will read all the information from the given file,
 * figure out a corresponding presentation context which will be used
 * to transmit the information over the network to the SCP, and it
 * will finally initiate the transmission of all data to the SCP.
 *
 * Parameters:
 *   assoc - [in] The association (network connection to another DICOM application).
 *   fname - [in] Name of the file which shall be processed.
 */
OFCondition StoreDcmtk::storeSCU(T_ASC_Association *assoc, const QString &file)
{
    DIC_US msgId = assoc->nextMsgID++;
    T_ASC_PresentationContextID presID;
    T_DIMSE_C_StoreRQ req = {};
    T_DIMSE_C_StoreRSP rsp = {};
    DIC_UI sopClass;
    DIC_UI sopInstance;
    OFString temp_str;
    DcmDataset *statusDetail = nullptr;

    /* read information from file. After the call to DcmFileFormat::loadFile(...) the information */
    /* which is encapsulated in the file will be available through the DcmFileFormat object. */
    /* In detail, it will be available through calls to DcmFileFormat::getMetaInfo() (for */
    /* meta header information) and DcmFileFormat::getDataset() (for data set information). */
    DcmFileFormat dcmff;
    OFCondition cond = dcmff.loadFile(file.toStdWString().c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength, ERM_autoDetect);

    /* figure out if an error occurred while the file was read */
    if (cond.bad())
    {
        qDebug() << "Bad DICOM file: " << file << ": " << cond.text();
        return cond;
    }

    /* figure out which SOP class and SOP instance is encapsulated in the file */
    if (!DU_findSOPClassAndInstanceInDataSet(dcmff.getDataset(),
        sopClass, sizeof(sopClass), sopInstance, sizeof(sopInstance), false))
    {
        qDebug() << "No SOP Class or Instance UID in file: " << file;
        return DIMSE_BADDATA;
    }

    /* figure out which of the accepted presentation contexts should be used */
    DcmXfer filexfer(dcmff.getDataset()->getOriginalXfer());
    if (filexfer.getXfer() != EXS_Unknown)
    {
        presID = ASC_findAcceptedPresentationContextID(assoc, sopClass, filexfer.getXferID());
    }
    else
    {
        presID = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    }
    if (presID == 0)
    {
        const char *modalityName = dcmSOPClassUIDToModality(sopClass);
        if (!modalityName) modalityName = dcmFindNameOfUID(sopClass);
        if (!modalityName) modalityName = "unknown SOP class";
        qDebug() << "No presentation context for: (" << modalityName << ") " << sopClass;
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    T_ASC_PresentationContext pc;
    ASC_findAcceptedPresentationContext(assoc->params, presID, &pc);
    DcmXfer netTransfer(pc.acceptedTransferSyntax);

    /* dump general information concerning transfer syntaxes */
    DcmXfer fileTransfer(dcmff.getDataset()->getOriginalXfer());
    qDebug() << "Converting transfer syntax: " << fileTransfer.getXferName() << " -> " << netTransfer.getXferName();

    /* prepare the transmission of data */
    memset(OFreinterpret_cast(char*, &req), 0, sizeof(req));
    req.MessageID = msgId;
    OFStandard::strlcpy(req.AffectedSOPClassUID, sopClass, sizeof(req.AffectedSOPClassUID));
    OFStandard::strlcpy(req.AffectedSOPInstanceUID, sopInstance, sizeof(req.AffectedSOPInstanceUID));
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_MEDIUM;

    /* dump some more general information */
    qDebug() << "Sending Store Request (MsgID " << msgId << ", " << dcmSOPClassUIDToModality(sopClass, "OT") << ")";

    int timeout = DIMSETimeout();
    if (timeout <= 0)
    {
        timeout = 0;
    }

    /* finally conduct transmission of data */
    cond = DIMSE_storeUser(assoc, presID, &req,
        nullptr, dcmff.getDataset(), progressCallback, this,
        timeout == 0 ? T_DIMSE_BlockingMode::DIMSE_BLOCKING : T_DIMSE_BlockingMode::DIMSE_NONBLOCKING,
        timeout, &rsp, &statusDetail, nullptr, OFstatic_cast(long, OFStandard::getFileSize(file.toStdWString().c_str())));

    /* dump some more general information */
    if (cond == EC_Normal)
    {
        qDebug() << "Received Store Response (" << DU_cstoreStatusString(rsp.DimseStatus) << ")";
        qDebug() << DIMSE_dumpMessage(temp_str, rsp, DIMSE_INCOMING, nullptr, presID).c_str();
    }
    else
    {
        qDebug() << "Store Failed, file: " << file << ":" << Qt::endl << DimseCondition::dump(temp_str, cond).c_str();
    }

    if (statusDetail != nullptr)
    {
        delete statusDetail;
    }

    /* return */
    return cond;
}

void StoreDcmtk::progressCallback(void *parent, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *req)
{
    Q_UNUSED(req);
    auto storeDcmtk = reinterpret_cast<StoreDcmtk*>(parent);
    static float totalPercent = 0.0f;
    switch (progress->state)
    {
    case DIMSE_StoreBegin:
        if (storeDcmtk != nullptr)
        {
            //OFString str;
            //DIMSE_dumpMessage(str, *req, DIMSE_OUTGOING);
            //qDebug() << str.c_str();
#ifndef QT_NO_DEBUG_OUTPUT
            storeDcmtk->resetQDebugDelay();
            storeDcmtk->qDebugDelay() << "XMIT: ";
#endif
        }

        break;
    case DIMSE_StoreEnd:
        if (storeDcmtk != nullptr)
        {
#ifndef QT_NO_DEBUG_OUTPUT
            storeDcmtk->resetQDebugDelay();
#endif
        }

        break;
    case DIMSE_StoreProgressing:
        if (storeDcmtk != nullptr)
        {
#ifndef QT_NO_DEBUG_OUTPUT
            storeDcmtk->qDebugDelay() << ".";
#endif
            auto totalFile = storeDcmtk->files()->size();
            if (totalFile > 0)
            {
                totalPercent = storeDcmtk->percentOfProcessedFiles() + 100.0f / totalFile * progress->progressBytes / progress->totalBytes;
                int newPercent = static_cast<int>(totalPercent + 0.5f);
                if (storeDcmtk->percent() != newPercent)
                {
                    storeDcmtk->setPercent(newPercent);
                }
            }
        }

        break;
    default:
        break;
    }
}
