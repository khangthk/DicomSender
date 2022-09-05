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

    static void savePort(const int &port);
    static int getPort();

    static void saveLibrary(const QString &lib);
    static QString getLibrary();

    static void saveConnectionTimeout(const int &sec);
    static int getConnectionTimeout();

    static void saveACSETimeout(const int &sec);
    static int getACSETimeout();

    static void saveDIMSETimeout(const int &sec);
    static int getDIMSETimeout();

    static void saveMaxPDU(const int &num);
    static int getMaxPDU();

    static void saveCompressionLevel(const int &level);
    static int getCompressionLevel();
};
