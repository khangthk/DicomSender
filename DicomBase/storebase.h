#pragma once

#include "global.h"
#include "scuinfo.h"

#include <utility>

#include <QDebug>

class DICOMBASE_EXPORT StoreBase : public SCUInfo
{
    Q_OBJECT

public:
    StoreBase(QObject *parent = nullptr);
    ~StoreBase();

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
    void processed(const std::pair<int, int> &index, const bool result, const QString &log);
    void done(const bool result, const QString &log);
};

