#pragma once

#include "global.h"
#include "echobase.h"

class OFCondition;
struct T_ASC_Association;

class DICOMBASE_EXPORT EchoDcmtk : public EchoBase
{
    Q_OBJECT

public:
    EchoDcmtk(QObject *parent = nullptr);
    virtual ~EchoDcmtk();

    void echo() override;

private:
    OFCondition echoSCU(T_ASC_Association *assoc);
};
