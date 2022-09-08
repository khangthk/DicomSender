#pragma once

#include <QString>
#include <QStringList>

class Setting
{
public:
    static void savePaths(const QStringList &paths);
    static QStringList getPaths();

    static void saveLocalAE(const QString &title);
    static QString getLocalAE();

    static void saveTargetAE(const QString &title);
    static QString getTargetAE();

    static void saveHost(const QString &host);
    static QString getHost();

    static void savePort(const int port);
    static int getPort();

    static void saveLibrary(const QString &library);
    static QString getLibrary();

    static void saveStopWhenError(const bool stop);
    static bool getStopWhenError();

    static void saveConnectionTimeout(const int seconds);
    static int getConnectionTimeout();

    static void saveACSETimeout(const int seconds);
    static int getACSETimeout();

    static void saveDIMSETimeout(const int seconds);
    static int getDIMSETimeout();

    static void saveMaxPDU(const int number);
    static int getMaxPDU();

    static void saveCompressionLevel(const int level);
    static int getCompressionLevel();
};
