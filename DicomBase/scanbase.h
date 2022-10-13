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
    ~ScanBase();

    void scan();

    void setDir(const QString &dir);
    void setOutput(QStringList &files);

    const bool cancel() const;
    void setCancel(const bool value);

protected:
    virtual bool isValid(const QString &file);

private:
    void scanDir(const QString &dirPath);
    void sortPathList(QStringList &list);

    QString m_dir;
    QStringList *m_files;
    bool m_cancel;

signals:
    void done(const uint count);
};
