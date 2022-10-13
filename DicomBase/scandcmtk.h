#pragma once

#include "global.h"
#include "scanbase.h"

class DICOMBASE_EXPORT ScanDcmtk : public ScanBase
{
    Q_OBJECT

public:
    ScanDcmtk(QObject *parent = nullptr);
    ~ScanDcmtk();

protected:
    bool isValid(const QString &file) override;
};
