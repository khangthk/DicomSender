#pragma once

#include "global.h"

#include <QObject>

class DICOMBASE_EXPORT EchoBase : public QObject
{
    Q_OBJECT

public:
    EchoBase(QObject *parent = nullptr);
    ~EchoBase();

    virtual bool echo() { return false; };

    const QString &localAE() const;
    void setLocalAE(const QString &newLocalAE);

    const QString &targetAE() const;
    void setTargetAE(const QString &newTargetAE);

    const QString &host() const;
    void setHost(const QString &newHost);

    uint16_t port() const;
    void setPort(const uint16_t newPort);

private:
    QString m_localAE;
    QString m_targetAE;
    QString m_host;
    uint16_t m_port;

signals:
    void done(const bool status, const QString &log);
};
