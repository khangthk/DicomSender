#pragma once

#include "global.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDebug>

class DICOMBASE_EXPORT ScanBase : public QObject
{
    Q_OBJECT

public:
    ScanBase(QObject *parent = nullptr);
    virtual ~ScanBase();

    void scan();
    void setScanDir(const QString &scanDir);

    const bool cancel() const;
    void setCancel(const bool value);

protected:
    virtual bool isValid(const QString &file);

private:
    void scanDir(const QString &dirPath);
    void sortPathList(QStringList &list);

    bool m_cancel;
    QString m_scanDir;
    QStringList *m_files;

signals:
    void result(const QStringList *files);
};
