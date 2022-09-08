#pragma once

#include "global.h"
#include "scuinfo.h"

class DICOMBASE_EXPORT StoreBase : public SCUInfo
{
    Q_OBJECT

public:
    StoreBase(QObject *parent = nullptr) : SCUInfo(parent), m_files(new QStringList)
    {
        qRegisterMetaType<QPair<int, int>>("QPair<int,int>");
    };
    ~StoreBase() {};

    virtual void store() = 0;

    const QStringList &files() const;
    void setFiles(const QStringList &files);

private:
    const QStringList *m_files;

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
