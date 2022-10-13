#include "scandcmtk.h"
#include "dcmtkhelper.h"

#include <QDebug>

ScanDcmtk::ScanDcmtk(QObject *parent)
    : ScanBase(parent)
{}

ScanDcmtk::~ScanDcmtk()
{
    qDebug() << "~ScanDcmtk";
}

bool ScanDcmtk::isValid(const QString &file)
{
    return DcmtkHelper::isDicom(file);
}
