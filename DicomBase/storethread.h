#pragma once

#include "global.h"
#include "storebase.h"

#include <QThread>

class DICOMBASE_EXPORT StoreThread : public QThread
{
    Q_OBJECT

public:
    StoreThread(const Library &lib = Library::dcmtk, QObject *parent = nullptr);
    ~StoreThread();

    StoreBase *object();

protected:
    void run() override;

private:
    std::unique_ptr<StoreBase> m_store;

signals:
    void progress(const int percent);
    void processed(const QPair<int, int> &index, const bool result, const QString &log);
    void done(const bool result, const QString &log);
};
