#include "echodcmtk.h"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmnet/dcmtrans.h>
#include <dcmtk/dcmdata/dcdict.h>

#include <QDebug>
#include <QString>
#include <QTextStream>

class NetworkSentry
{
public:
    NetworkSentry() { OFStandard::initializeNetwork(); }
    ~NetworkSentry() { OFStandard::shutdownNetwork(); }
};

EchoDcmtk::EchoDcmtk(QObject *parent)
    : EchoBase(parent)
{}

EchoDcmtk::~EchoDcmtk()
{
    qDebug() << "~EchoDcmtk";
}

void EchoDcmtk::echo()
{
    T_ASC_Network *net;
    T_ASC_Parameters *params;
    T_ASC_Association *assoc;
    OFString temp_str;
    NetworkSentry sentry;

    // always set the timeout values since the global default might be different
    dcmSocketSendTimeout.set(static_cast<Sint32>(socketTimeout()));
    dcmSocketReceiveTimeout.set(static_cast<Sint32>(socketTimeout()));

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
        emit done(false, QString(temp_str.c_str()));
        return;
    }

    /* initialize association parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, maxReceivePDU(), dcmConnectionTimeout.get());
    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit done(false, QString(temp_str.c_str()));
        return;
    }

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, localAE().toStdString().c_str(), targetAE().toStdString().c_str(), nullptr);

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), QString("%1:%2").arg(host()).arg(port()).toStdString().c_str());

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    const char *transferSyntaxes[] = { UID_LittleEndianImplicitTransferSyntax }; // default transfer syntax
    cond = ASC_addPresentationContext(params, 1, UID_VerificationSOPClass, transferSyntaxes, DIM_OF(transferSyntaxes));
    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit done(false, QString(temp_str.c_str()));
        return;
    }

    /* dump presentation contexts if required */
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
            emit done(false, log);
            return;
        }
        else
        {
            DimseCondition::dump(temp_str, cond);
            log = "Association Request Failed: " + QString(temp_str.c_str());
            emit done(false, log);
            return;
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    qDebug() << "Association Parameters Negotiated:" << Qt::endl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC).c_str();

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0)
    {
        emit done(false, "No Acceptable Presentation Contexts");
        return;
    }

    /* dump general information concerning the establishment of the network connection if required */
    qDebug() << "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")";

    /* do the real work, i.e. send a number of C-ECHO-RQ messages to the DICOM application */
    /* this application is connected with and handle corresponding C-ECHO-RSP messages. */
    cond = echoSCU(assoc);

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        emit done(true, "Echo successfully");

        /* release association */
        qDebug() << "Releasing Association";
        cond = ASC_releaseAssociation(assoc);
        if (cond.bad())
        {
            DimseCondition::dump(temp_str, cond);
            emit done(false, "Association Release Failed: " + QString(temp_str.c_str()));
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
            emit done(false, "Association Abort Failed: " + QString(temp_str.c_str()));
            return;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        emit done(false, "Peer Aborted Association");
        return;
    }
    else
    {
        DimseCondition::dump(temp_str, cond);
        qDebug() << "Echo SCU Failed: " << QString(temp_str.c_str());
        qDebug() << "Aborting Association";
        cond = ASC_abortAssociation(assoc);

        if (cond.bad())
        {
            DimseCondition::dump(temp_str, cond);
            emit done(false, "Association Abort Failed: " + QString(temp_str.c_str()));
            return;
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit done(false, QString(temp_str.c_str()));
        return;
    }

    /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
    /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
    cond = ASC_dropNetwork(&net);
    if (cond.bad())
    {
        DimseCondition::dump(temp_str, cond);
        emit done(false, QString(temp_str.c_str()));
        return;
    }
}

/*
 * This function will send a C-ECHO-RQ over the network to another DICOM application
 * and handle the response.
 *
 * Parameters:
 *   assoc - [in] The association (network connection to another DICOM application).
 */
OFCondition EchoDcmtk::echoSCU(T_ASC_Association *assoc)
{
    DIC_US msgId = assoc->nextMsgID++;
    DIC_US status;
    DcmDataset *statusDetail = nullptr;

    /* dump information if required */
    qDebug() << "Sending Echo Request (MsgID " << msgId << ")";

    int timeout = DIMSETimeout();
    if (timeout <= 0)
    {
        timeout = 0;
    }
    /* send C-ECHO-RQ and handle response */
    OFCondition cond = DIMSE_echoUser(assoc, msgId,
        timeout == 0 ? T_DIMSE_BlockingMode::DIMSE_BLOCKING : T_DIMSE_BlockingMode::DIMSE_NONBLOCKING,
        timeout, &status, &statusDetail);

    /* depending on if a response was received, dump some information */
    if (cond.good())
    {
        qDebug() << "Received Echo Response (" << DU_cechoStatusString(status) << ")";
    }
    else
    {
        OFString temp_str;
        DimseCondition::dump(temp_str, cond);
        qDebug() << "Echo Failed: " << temp_str.c_str();
    }

    if (statusDetail != nullptr)
    {
        delete statusDetail;
    }

    /* return result value */
    return cond;
}
