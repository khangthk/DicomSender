#pragma once

#include "global.h"
#include "scuinfo.h"

class DICOMBASE_EXPORT EchoBase : public SCUInfo
{
    Q_OBJECT

public:
    EchoBase(QObject *parent = nullptr);
    virtual ~EchoBase();

    virtual void echo() = 0;

signals:
    void result(const bool result, const QString &log);
};
