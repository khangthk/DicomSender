#include "echobase.h"

EchoBase::EchoBase(QObject *parent)
    : QObject(parent), m_localAE("AE"), m_targetAE("AE"), m_host("localhost"), m_port(104)
{}

EchoBase::~EchoBase()
{}

const QString &EchoBase::localAE() const
{
    return m_localAE;
}

void EchoBase::setLocalAE(const QString &newLocalAE)
{
    m_localAE = newLocalAE;
}

const QString &EchoBase::targetAE() const
{
    return m_targetAE;
}

void EchoBase::setTargetAE(const QString &newTargetAE)
{
    m_targetAE = newTargetAE;
}

const QString &EchoBase::host() const
{
    return m_host;
}

void EchoBase::setHost(const QString &newHost)
{
    m_host = newHost;
}

uint16_t EchoBase::port() const
{
    return m_port;
}

void EchoBase::setPort(const uint16_t newPort)
{
    m_port = newPort;
}
