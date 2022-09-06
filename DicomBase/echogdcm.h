#pragma once

#include "global.h"
#include "echobase.h"

class DICOMBASE_EXPORT EchoGdcm : public EchoBase
{
    Q_OBJECT

public:
    EchoGdcm(QObject *parent = nullptr);
    ~EchoGdcm();

    bool echo();
};
