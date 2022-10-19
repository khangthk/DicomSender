#include "storebase.h"

#include <QDebug>

StoreBase::StoreBase(QObject *parent)
    : SCUInfo(parent), m_percent(0), m_percentOfProcessedFiles(0.0f), m_files(nullptr)
{
    qRegisterMetaType<QPair<int, int>>("QPair<int,int>");
}

StoreBase::~StoreBase()
{
    qDebug() << "~StoreBase";
}

const int StoreBase::percent() const
{
    return m_percent;
}

void StoreBase::setPercent(const int percent)
{
    if (m_percent != percent)
    {
        qDebug() << "--->Percent: " << percent;
        m_percent = percent;
        emit progress(m_percent);
    }
}

const float StoreBase::percentOfProcessedFiles() const
{
    return m_percentOfProcessedFiles;
}

void StoreBase::setPercentOfProcessedFiles(const float percentOfProcessedFiles)
{
    m_percentOfProcessedFiles = percentOfProcessedFiles;
}

const QStringList *StoreBase::files() const
{
    return m_files;
}

void StoreBase::setFiles(const QStringList *files)
{
    m_files = files;
}
