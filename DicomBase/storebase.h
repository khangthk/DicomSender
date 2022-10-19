#pragma once

#include "global.h"
#include "scuinfo.h"

#include <QDebug>
#include <QPair>

class DICOMBASE_EXPORT StoreBase : public SCUInfo
{
    Q_OBJECT

public:
    StoreBase(QObject *parent = nullptr);
    virtual ~StoreBase();

    virtual void store() = 0;

    const int percent() const;
    void setPercent(const int percent);

    const float percentOfProcessedFiles() const;
    void setPercentOfProcessedFiles(const float percentOfProcessedFiles);

    const QStringList *files() const;
    void setFiles(const QStringList *files);

private:
    int m_percent;
    float m_percentOfProcessedFiles;
    const QStringList *m_files;

signals:
    void progress(const int percent);
    void processed(const QPair<int, int> &index, const bool result, const QString &log);
    void result(const bool result, const QString &log);
};
