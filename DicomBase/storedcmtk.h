#pragma once

#include "global.h"
#include "storebase.h"

class DICOMBASE_EXPORT StoreDcmtk : public StoreBase
{
    Q_OBJECT

public:
    StoreDcmtk(QObject *parent = nullptr);
    ~StoreDcmtk();

protected:
    void store() override;
};
