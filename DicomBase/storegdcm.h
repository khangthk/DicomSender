#pragma once

#include "global.h"
#include "storebase.h"

class DICOMBASE_EXPORT StoreGdcm : public StoreBase
{
    Q_OBJECT

public:
    StoreGdcm(QObject *parent = nullptr);
    ~StoreGdcm();

protected:
    void store() override;
};
