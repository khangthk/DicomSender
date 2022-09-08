#include "scangdcm.h"
#include "gdcmhelper.h"

ScanGdcm::ScanGdcm(QObject *parent)
    : ScanBase(parent)
{}

ScanGdcm::~ScanGdcm()
{}

bool ScanGdcm::isDicom(const QString &file)
{
    return GdcmHelper::isDicom(file);
}
