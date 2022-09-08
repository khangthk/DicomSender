#pragma once

#include "global.h"
#include "scanbase.h"

#include <QThread>

class DICOMBASE_EXPORT ScanThread : public QThread
{
    Q_OBJECT

public:
    ScanThread(const Library &lib = Library::dcmtk, QObject *parent = nullptr);
    ~ScanThread();

    ScanBase *object();

protected:
    void run() override;

private:
    std::unique_ptr<ScanBase> m_scan;

signals:
    void done(const uint count);
};
