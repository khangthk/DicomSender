#include "scandcmtk.h"

ScanDcmtk::ScanDcmtk(QObject *parent)
    : ScanBase(parent)
{}

ScanDcmtk::~ScanDcmtk()
{}

bool ScanDcmtk::isDicom(const QString &file)
{
    return false;
}
