#include "setting.h"

#include <QSettings>

void Setting::savePaths(const QStringList &paths)
{
    QSettings setting;
    setting.setValue("Paths", paths);
}

QStringList Setting::getPaths()
{
    QSettings setting;
    return setting.value("Paths", {}).toStringList();
}

void Setting::saveLocalAE(const QString &title)
{
    if (!title.isEmpty())
    {
        QSettings setting;
        setting.setValue("LocalAE", title);
    }
}

QString Setting::getLocalAE()
{
    QSettings setting;
    return setting.value("LocalAE", "AE").toString();
}

void Setting::saveTargetAE(const QString &title)
{
    if (!title.isEmpty())
    {
        QSettings setting;
        setting.setValue("TargetAE", title);
    }
}

QString Setting::getTargetAE()
{
    QSettings setting;
    return setting.value("TargetAE", "AE").toString();
}

void Setting::saveHost(const QString &host)
{
    if (!host.isEmpty())
    {
        QSettings setting;
        setting.setValue("Host", host);
    }
}

QString Setting::getHost()
{
    QSettings setting;
    return setting.value("Host", "localhost").toString();
}

void Setting::savePort(const int port)
{
    QSettings setting;
    setting.setValue("Port", port);
}

int Setting::getPort()
{
    QSettings setting;
    return setting.value("Port", 104).toInt();
}

void Setting::saveLibrary(const QString &library)
{
    QSettings setting;
    setting.setValue("Library", library);
}

QString Setting::getLibrary()
{
    QSettings setting;
    return setting.value("Library", "dcmtk").toString();
}

void Setting::saveStopWhenError(const bool stop)
{
    QSettings setting;
    setting.setValue("StopWhenFail", stop);
}

bool Setting::getStopWhenError()
{
    QSettings setting;
    return setting.value("StopWhenFail", true).toBool();
}

void Setting::saveConnectionTimeout(const int seconds)
{
    QSettings setting;
    setting.setValue("ConnectionTimeout", QString("%1").arg(seconds));
}

int Setting::getConnectionTimeout()
{
    QSettings setting;
    return setting.value("ConnectionTimeout", "-1").toInt();
}

void Setting::saveACSETimeout(const int seconds)
{
    QSettings setting;
    setting.setValue("ACSETimeout", QString("%1").arg(seconds));
}

int Setting::getACSETimeout()
{
    QSettings setting;
    return setting.value("ACSETimeout", "30").toInt();
}

void Setting::saveDIMSETimeout(const int seconds)
{
    QSettings setting;
    setting.setValue("DIMSETimeout", QString("%1").arg(seconds));
}

int Setting::getDIMSETimeout()
{
    QSettings setting;
    return setting.value("DIMSETimeout", "-1").toInt();
}

void Setting::saveMaxPDU(const int number)
{
    QSettings setting;
    setting.setValue("MaxPDU", number);
}

int Setting::getMaxPDU()
{
    QSettings setting;
    return setting.value("MaxPDU", 16).toInt();
}

void Setting::saveCompressionLevel(const int level)
{
    QSettings setting;
    setting.setValue("CompressionLevel", level);
}

int Setting::getCompressionLevel()
{
    QSettings setting;
    return setting.value("CompressionLevel", 6).toInt();
}
