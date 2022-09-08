#pragma once

#include "global.h"

#include <QObject>
#include <QString>

class DICOMBASE_EXPORT SCUInfo : public QObject
{
    Q_OBJECT

public:
    SCUInfo(QObject *parent = nullptr);
    ~SCUInfo();

    const QString &localAE() const;
    void setLocalAE(const QString &localAE);

    const QString &targetAE() const;
    void setTargetAE(const QString &targetAE);

    const QString &host() const;
    void setHost(const QString &host);

    uint16_t port() const;
    void setPort(const uint16_t port);

    bool stopWhenError() const;
    void setStopWhenError(const bool stop);

    int connectionTimeout() const;
    void setConnectionTimeout(const int seconds);

    int ACSETimeout() const;
    void setACSETimeout(const int seconds);

    int DIMSETimeout() const;
    void setDIMSETimeout(const int seconds);

    int maxPDU() const;
    void setMaxPDU(const int max);

    int compressionLevel() const;
    void setCompressionLevel(const int level);

private:
    QString m_localAE;
    QString m_targetAE;
    QString m_host;
    uint16_t m_port;
    bool m_stopWhenError;
    int m_connectionTimeout;
    int m_ACSETimeout;
    int m_DIMSETimeout;
    int m_maxPDU;
    int m_compressionLevel;
};
