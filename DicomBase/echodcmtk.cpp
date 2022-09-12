#include "echodcmtk.h"

#include <dcmtk/config/osconfig.h>    /* make sure OS specific configuration is included first */
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmnet/dcmtrans.h>    /* for dcmSocketSend/ReceiveTimeout */
#include <dcmtk/dcmdata/dcdict.h>

#include <QDebug>
#include <QString>
#include <QTextStream>

const char *transferSyntaxes[] = {
      UID_LittleEndianImplicitTransferSyntax, /* default xfer syntax first */
      UID_LittleEndianExplicitTransferSyntax,
      UID_BigEndianExplicitTransferSyntax,
      UID_JPEGProcess1TransferSyntax,
      UID_JPEGProcess2_4TransferSyntax,
      UID_JPEGProcess3_5TransferSyntax,
      UID_JPEGProcess6_8TransferSyntax,
      UID_JPEGProcess7_9TransferSyntax,
      UID_JPEGProcess10_12TransferSyntax,
      UID_JPEGProcess11_13TransferSyntax,
      UID_JPEGProcess14TransferSyntax,
      UID_JPEGProcess15TransferSyntax,
      UID_JPEGProcess16_18TransferSyntax,
      UID_JPEGProcess17_19TransferSyntax,
      UID_JPEGProcess20_22TransferSyntax,
      UID_JPEGProcess21_23TransferSyntax,
      UID_JPEGProcess24_26TransferSyntax,
      UID_JPEGProcess25_27TransferSyntax,
      UID_JPEGProcess28TransferSyntax,
      UID_JPEGProcess29TransferSyntax,
      UID_JPEGProcess14SV1TransferSyntax,
      UID_RLELosslessTransferSyntax,
      UID_DeflatedExplicitVRLittleEndianTransferSyntax,
      UID_JPEGLSLosslessTransferSyntax,
      UID_JPEGLSLossyTransferSyntax,
      UID_JPEG2000LosslessOnlyTransferSyntax,
      UID_JPEG2000TransferSyntax,
      UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax,
      UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax,
      UID_MPEG2MainProfileAtMainLevelTransferSyntax,
      UID_MPEG2MainProfileAtHighLevelTransferSyntax,
      UID_MPEG4HighProfileLevel4_1TransferSyntax,
      UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,
      UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,
      UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,
      UID_MPEG4StereoHighProfileLevel4_2TransferSyntax,
      UID_HEVCMainProfileLevel5_1TransferSyntax,
      UID_HEVCMain10ProfileLevel5_1TransferSyntax,
      UID_RLELosslessTransferSyntax,
      UID_SMPTEST2110_20_UncompressedProgressiveActiveVideoTransferSyntax,
      UID_SMPTEST2110_20_UncompressedInterlacedActiveVideoTransferSyntax,
      UID_SMPTEST2110_30_PCMDigitalAudioTransferSyntax,
      UID_RFC2557MIMEEncapsulationTransferSyntax,
      UID_XMLEncodingTransferSyntax,
      UID_PrivateGE_LEI_WithBigEndianPixelDataTransferSyntax
};

class ShutdownSentry
{
public:
    ShutdownSentry() {}
    ~ShutdownSentry() { OFStandard::shutdownNetwork(); }
};

EchoDcmtk::EchoDcmtk(QObject *parent)
    : EchoBase(parent)
{}

EchoDcmtk::~EchoDcmtk()
{}

void EchoDcmtk::echo()
{
    OFBool opt_abortAssociation = OFFalse;
    T_ASC_Network *net;
    T_ASC_Parameters *params;
    T_ASC_Association *assoc;
    OFString temp_str;
    ShutdownSentry shutdown;

    OFStandard::initializeNetwork();

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
    ASC_setAPTitles(params, localAE().toLocal8Bit(), targetAE().toLocal8Bit(), nullptr);

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), QString("%1:%2").arg(host()).arg(port()).toLocal8Bit());

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    int presentationContextID = 1; /* odd byte value 1, 3, 5, .. 255 */
    int numPresentationCtx = 1;
    int numXferSyntaxes = 1;

    for (int i = 0; i < numPresentationCtx; i++)
    {
        cond = ASC_addPresentationContext(params, OFstatic_cast(T_ASC_PresentationContextID, presentationContextID),
            UID_VerificationSOPClass, transferSyntaxes, OFstatic_cast(int, numXferSyntaxes));
        presentationContextID += 2;
        if (cond.bad())
        {
            DimseCondition::dump(temp_str, cond);
            emit done(false, QString(temp_str.c_str()));
            return;
        }
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

        if (opt_abortAssociation)
        {
            qDebug() << "Aborting Association";
            cond = ASC_abortAssociation(assoc);
            if (cond.bad())
            {
                DimseCondition::dump(temp_str, cond);
                emit done(false, "Association Abort Failed: " + QString(temp_str.c_str()));
                return;
            }
        }
        else
        {
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
