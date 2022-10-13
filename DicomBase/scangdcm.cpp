#include "scangdcm.h"
#include "gdcmhelper.h"

#include <QDebug>

ScanGdcm::ScanGdcm(QObject *parent)
    : ScanBase(parent)
{}

ScanGdcm::~ScanGdcm()
{
    qDebug() << "~ScanGdcm";
}

bool ScanGdcm::isValid(const QString &file)
{
    return GdcmHelper::isDicom(file);
}
