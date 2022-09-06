#include "echothread.h"
#include "echodcmtk.h"
#include "echogdcm.h"

#include <QDebug>

EchoThread::EchoThread(const QString &localAE, const QString &targetAE,
                       const QString &host, uint16_t port,
                       const Library &lib, QObject *parent)
    : QThread(parent)
{
    if (lib == Library::dcmtk)
    {
        m_echo.reset(new EchoDcmtk());
    }
    else
    {
        m_echo.reset(new EchoGdcm());
    }
    m_echo->setLocalAE(localAE);
    m_echo->setTargetAE(targetAE);
    m_echo->setHost(host);
    m_echo->setPort(port);

    connect(m_echo.get(), &EchoBase::done, this, &EchoThread::done);
}

EchoThread::~EchoThread()
{
    qDebug() << "~EchoThread";
}

void EchoThread::run()
{
    m_echo->echo();
}
