#pragma once

#include "global.h"
#include "scuinfo.h"

#include <QDebug>
#include <QPair>

class DICOMBASE_EXPORT StoreBase : public SCUInfo
{
    Q_OBJECT

public:
    StoreBase(QObject *parent = nullptr) : SCUInfo(parent), m_files(new QStringList), m_percent(0), m_percentOfProcessedFiles(0.0f)
    {
        qRegisterMetaType<QPair<int, int>>("QPair<int,int>");
    };
    ~StoreBase() {};

    virtual void store() = 0;

    const QStringList &files() const;
    void setFiles(const QStringList &files);

    int percent() const;
    void setPercent(int percent);

    float percentOfProcessedFiles() const;
    void setPercentOfProcessedFiles(float percentOfProcessedFiles);

private:
    const QStringList *m_files;
    int m_percent;
    float m_percentOfProcessedFiles;

signals:
    void progress(const int percent);
    void processed(const QPair<int, int> &index, const bool result, const QString &log);
    void done(const bool result, const QString &log);
};

inline const QStringList &StoreBase::files() const
{
    return *m_files;
}

inline void StoreBase::setFiles(const QStringList &files)
{
    m_files = &files;
}

inline int StoreBase::percent() const
{
    return m_percent;
}

inline void StoreBase::setPercent(int percent)
{
    if (m_percent != percent)
    {
        qDebug() << "--->Percent: " << percent;
        m_percent = percent;
        emit progress(m_percent);
    }
}

inline float StoreBase::percentOfProcessedFiles() const
{
    return m_percentOfProcessedFiles;
}

inline void StoreBase::setPercentOfProcessedFiles(float percentOfProcessedFiles)
{
    m_percentOfProcessedFiles = percentOfProcessedFiles;
}
