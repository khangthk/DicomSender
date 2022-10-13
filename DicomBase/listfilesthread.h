#pragma once

#include "global.h"
#include "scanbase.h"

#include <QThread>

class DICOMBASE_EXPORT ListFilesThread : public QThread
{
    Q_OBJECT
public:
    ListFilesThread(QObject *parent = nullptr);
    ~ListFilesThread();

    ScanBase *object();

protected:
    void run() override;

private:
    std::unique_ptr<ScanBase> m_scan;

signals:
    void done(const uint count);
};
