#include "scuinfo.h"

#include <QDebug>

SCUInfo::SCUInfo(QObject *parent)
    : QObject(parent), m_localAE("AE"), m_targetAE("AE"), m_host("localhost"), m_port(104),
    m_stopWhenError(true), m_connectionTimeout(-1), m_socketTimeout(60), m_ACSETimeout(30), m_DIMSETimeout(0),
    m_maxSendPDU(16 * 1024), m_maxReceivePDU(16 * 1024)
{}

SCUInfo::~SCUInfo()
{
    qDebug() << "~SCUInfo";
}

const QString &SCUInfo::localAE() const
{
    return m_localAE;
}

void SCUInfo::setLocalAE(const QString &localAE)
{
    m_localAE = localAE;
}

const QString &SCUInfo::targetAE() const
{
    return m_targetAE;
}

void SCUInfo::setTargetAE(const QString &targetAE)
{
    m_targetAE = targetAE;
}

const QString &SCUInfo::host() const
{
    return m_host;
}

void SCUInfo::setHost(const QString &host)
{
    m_host = host;
}

uint16_t SCUInfo::port() const
{
    return m_port;
}

void SCUInfo::setPort(const uint16_t port)
{
    m_port = port;
}

bool SCUInfo::stopWhenError() const
{
    return m_stopWhenError;
}

void SCUInfo::setStopWhenError(const bool stop)
{
    m_stopWhenError = stop;
}

int SCUInfo::connectionTimeout() const
{
    return m_connectionTimeout;
}

void SCUInfo::setConnectionTimeout(const int seconds)
{
    m_connectionTimeout = seconds;
}

int SCUInfo::socketTimeout() const
{
    return m_socketTimeout;
}

void SCUInfo::setSocketTimeout(const int seconds)
{
    m_socketTimeout = seconds;
}

int SCUInfo::ACSETimeout() const
{
    return m_ACSETimeout;
}

void SCUInfo::setACSETimeout(const int seconds)
{
    m_ACSETimeout = seconds;
}

int SCUInfo::DIMSETimeout() const
{
    return m_DIMSETimeout;
}

void SCUInfo::setDIMSETimeout(const int seconds)
{
    m_DIMSETimeout = seconds;
}

int SCUInfo::maxSendPDU() const
{
    return m_maxSendPDU;
}

void SCUInfo::setMaxSendPDU(const int max)
{
    m_maxSendPDU = max;
}

int SCUInfo::maxReceivePDU() const
{
    return m_maxReceivePDU;
}

void SCUInfo::setMaxReceivePDU(const int max)
{
    m_maxReceivePDU = max;
}
