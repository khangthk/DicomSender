#include "echogdcm.h"

#include <gdcmAttribute.h>
#include <gdcmULConnectionManager.h>
#include <gdcmPresentationContextGenerator.h>

#include <QDebug>

using namespace gdcm;

EchoGdcm::EchoGdcm(QObject *parent)
    : EchoBase(parent)
{}

EchoGdcm::~EchoGdcm()
{
    qDebug() << "~EchoDcmtk";
}

void EchoGdcm::echo()
{
    PresentationContextGenerator generator;
    if (!generator.GenerateFromUID(UIDs::VerificationSOPClass))
    {
        emit result(false, "Failed to generate presentation context");
        return;
    }

    network::ULConnectionManager theManager;
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
        emit result(false, "Failed to establish connection");
        return;
    }

    std::vector<network::PresentationDataValue> theValues = theManager.SendEcho();

    try
    {
        theManager.BreakConnection(-1); // wait for a while for the connection to break, ie, infinite
    }
    catch (...)
    {
    }

    // Check the Success Status
    DataSet ds = network::PresentationDataValue::ConcatenatePDVBlobs(theValues);
    Attribute<0x0, 0x0900> at{};
    if (ds.FindDataElement(at.GetTag()))
    {
        at.SetFromDataSet(ds);
        if (at.GetValue() != 0)
        {
            emit result(false, "Wrong value for status (C-ECHO)");
            return;
        }

        emit result(true, "Echo successfully");
        return;
    }

    emit result(false, "Empty return on C-ECHO (no status)");
    return;
}
