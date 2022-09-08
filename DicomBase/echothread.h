#pragma once

#include "global.h"
#include "echobase.h"

#include <QThread>

class DICOMBASE_EXPORT EchoThread : public QThread
{
    Q_OBJECT

public:
    EchoThread(const Library &lib = Library::dcmtk, QObject *parent = nullptr);
    ~EchoThread();

    EchoBase *object();

protected:
    void run() override;

private:
    std::unique_ptr<EchoBase> m_echo;

signals:
    void done(const bool result, const QString &log);
};
