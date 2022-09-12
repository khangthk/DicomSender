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

void Setting::saveCurrentPath(const QString &path)
{
    QSettings setting;
    setting.setValue("CurrentPath", path);
}

QString Setting::getCurrentPath()
{
    QSettings setting;
    return setting.value("CurrentPath", "").toString();
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

void Setting::saveSocketTimeout(const int seconds)
{
    QSettings setting;
    setting.setValue("SocketTimeout", QString("%1").arg(seconds));
}

int Setting::getSocketTimeout()
{
    QSettings setting;
    return setting.value("SocketTimeout", "60").toInt();
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
    return setting.value("DIMSETimeout", "0").toInt();
}

void Setting::saveMaxSendPDU(const int max)
{
    QSettings setting;
    setting.setValue("MaxSendPDU", max);
}

int Setting::getMaxSendPDU()
{
    QSettings setting;
    return setting.value("MaxSendPDU", 16).toInt();
}

void Setting::saveMaxReceivePDU(const int max)
{
    QSettings setting;
    setting.setValue("MaxReceivePDU", max);
}

int Setting::getMaxReceivePDU()
{
    QSettings setting;
    return setting.value("MaxReceivePDU", 16).toInt();
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

void Setting::saveDecompressionMode(const int mode)
{
    QSettings setting;
    setting.setValue("DecompressionMode", mode);
}

int Setting::getDecompressionMode()
{
    QSettings setting;
    return setting.value("DecompressionMode", 1).toInt();
}
