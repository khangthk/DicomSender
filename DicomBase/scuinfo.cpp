#include "scuinfo.h"

SCUInfo::SCUInfo(QObject *parent)
    : QObject(parent), m_localAE("AE"), m_targetAE("AE"), m_host("localhost"), m_port(104), m_stopWhenError(true),
    m_connectionTimeout(10), m_ACSETimeout(10), m_DIMSETimeout(10), m_maxPDU(16), m_compressionLevel(6)
{}

SCUInfo::~SCUInfo()
{}

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

int SCUInfo::maxPDU() const
{
    return m_maxPDU;
}

void SCUInfo::setMaxPDU(const int max)
{
    m_maxPDU = max;
}

int SCUInfo::compressionLevel() const
{
    return m_compressionLevel;
}

void SCUInfo::setCompressionLevel(const int level)
{
    m_compressionLevel = level;
}
