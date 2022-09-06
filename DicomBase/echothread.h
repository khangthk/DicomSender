#pragma once

#include "global.h"
#include "echobase.h"

#include <QThread>

enum class Library { dcmtk, gdcm };

class DICOMBASE_EXPORT EchoThread : public QThread
{
    Q_OBJECT

public:
    EchoThread(const QString &localAE = "AE", const QString &targetAE = "AE",
               const QString &host = "localhost", uint16_t port = 104,
               const Library &lib = Library::dcmtk,  QObject *parent = nullptr);
    ~EchoThread();

protected:
    void run() override;

private:
    std::unique_ptr<EchoBase> m_echo;

signals:
    void done(const bool status, const QString &log);
};

