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

    virtual void scan();

    void setScanDir(const QString &scanDir);
    const QString &scanDir() const;

    const bool cancel() const;
    void setCancel(const bool value);

protected:
    virtual bool isValid(const QString &file);
    virtual void sortPathList(QStringList &list);

private:
    void scanDir(const QString &dirPath);

    bool m_cancel;
    QString m_scanDir;
    QStringList *m_files;

signals:
    void result(const QStringList *files);
};
