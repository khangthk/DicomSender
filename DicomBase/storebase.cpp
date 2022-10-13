#include "storebase.h"

#include <QDebug>

StoreBase::StoreBase(QObject *parent) : SCUInfo(parent), m_files(new QStringList), m_percent(0), m_percentOfProcessedFiles(0.0f)
{}

StoreBase::~StoreBase()
{
    qDebug() << "~StoreBase";
}

const QStringList &StoreBase::files() const
{
    return *m_files;
}

void StoreBase::setFiles(const QStringList &files)
{
    m_files = &files;
}

int StoreBase::percent() const
{
    return m_percent;
}

void StoreBase::setPercent(int percent)
{
    if (m_percent != percent)
    {
        qDebug() << "--->Percent: " << percent;
        m_percent = percent;
        emit progress(m_percent);
    }
}

float StoreBase::percentOfProcessedFiles() const
{
    return m_percentOfProcessedFiles;
}

void StoreBase::setPercentOfProcessedFiles(float percentOfProcessedFiles)
{
    m_percentOfProcessedFiles = percentOfProcessedFiles;
}
