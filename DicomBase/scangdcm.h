#pragma once

#include "global.h"
#include "scanbase.h"

class DICOMBASE_EXPORT ScanGdcm : public ScanBase
{
    Q_OBJECT

public:
    ScanGdcm(QObject *parent = nullptr);
    virtual ~ScanGdcm();

protected:
    bool isValid(const QString &file) override;
};
