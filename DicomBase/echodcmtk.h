#pragma once

#include "global.h"
#include "echobase.h"

class DICOMBASE_EXPORT EchoDcmtk : public EchoBase
{
    Q_OBJECT

public:
    EchoDcmtk(QObject *parent = nullptr);
    ~EchoDcmtk();

    void echo() override;
};
