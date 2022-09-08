#pragma once

#include "global.h"
#include "scuinfo.h"

#include <QDirIterator>
#include <QCollator>

class DICOMBASE_EXPORT ScanBase : public SCUInfo
{
    Q_OBJECT

public:
    ScanBase(QObject *parent = nullptr) : SCUInfo(parent), m_files(new QStringList) {};
    ~ScanBase() {};

    void scan();

    const QString &dir() const;
    void setDir(const QString &dir);

    const QStringList &files() const;
    void setOutput(QStringList &files);

protected:
    virtual bool isDicom(const QString &file) = 0;

private:
  QString m_dir;
  QStringList *m_files;

signals:
    void done(const uint count);
};

inline void ScanBase::scan()
{
    m_files->clear();

    QDirIterator it(m_dir, QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
      it.next();
      if (isDicom(it.filePath()))
      {
        m_files->push_back(it.filePath());
      }
    }

    QCollator collator;
    collator.setNumericMode(true);

    auto fnCollator = [&collator](const QString &s1, const QString &s2)
    {
      return collator.compare(s1, s2) < 0;
    };
    std::sort(m_files->begin(), m_files->end(), fnCollator);

    emit done(m_files->count());
}

inline const QString &ScanBase::dir() const
{
    return m_dir;
}

inline void ScanBase::setDir(const QString &dir)
{
    m_dir = dir;
}

inline const QStringList &ScanBase::files() const
{
    return *m_files;
}

inline void ScanBase::setOutput(QStringList &files)
{
    m_files = &files;
}
