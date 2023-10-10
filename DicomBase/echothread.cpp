#include "echothread.h"
#include "echodcmtk.h"
#include "echogdcm.h"

#include <QDebug>

EchoThread::EchoThread(const Library lib, QObject *parent)
    : QThread(parent)
{
    Library::dcmtk == lib ? m_echo.reset(new EchoDcmtk()) : m_echo.reset(new EchoGdcm());
    connect(m_echo.get(), &EchoBase::result, this, &EchoThread::result);
}

EchoThread::~EchoThread()
{
    qDebug() << "~EchoThread";
}

EchoBase *EchoThread::object()
{
    return m_echo.get();
}

void EchoThread::run()
{
    m_echo->echo();
}
