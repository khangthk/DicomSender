#include "scanbase.h"

#include <QDebug>
#include <QDirIterator>
#include <QCollator>

ScanBase::ScanBase(QObject *parent)
    : QObject(parent), m_cancel(false), m_files(nullptr)
{}

ScanBase::~ScanBase()
{
    qDebug() << "~ScanBase";
}

void ScanBase::scan()
{
    if (m_files == nullptr)
    {
        m_files = new QStringList;
    }

    m_files->clear();

    scanDir(m_scanDir);

    if (cancel())
    {
        delete m_files;
        m_files = nullptr;
    }

    emit result(m_files);
}

void ScanBase::setScanDir(const QString &scanDir)
{
    m_scanDir = scanDir;
}

const QString &ScanBase::scanDir() const
{
    return m_scanDir;
}

const bool ScanBase::cancel() const
{
    return m_cancel;
}

void ScanBase::setCancel(const bool value)
{
    m_cancel = value;
}

bool ScanBase::isValid(const QString &file)
{
    Q_UNUSED(file);
    return true;
}

void ScanBase::sortPathList(QStringList &list)
{
    QCollator collator;
    collator.setNumericMode(true);

    auto fnCollator = [&collator](const QString &s1, const QString &s2)
    {
        return collator.compare(s1, s2) < 0;
    };

    std::sort(list.begin(), list.end(), fnCollator);
}

void ScanBase::scanDir(const QString &dirPath)
{
    if (cancel())
    {
        return;
    }

    QDirIterator itDir(dirPath, QDir::Dirs | QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

    QStringList dirs;
    while (itDir.hasNext() && !cancel())
    {
        itDir.next();
        dirs.push_back(itDir.filePath());
    }

    if (cancel())
    {
        return;
    }

    if (!dirs.empty())
    {
        sortPathList(dirs);

        for (auto &childDir : dirs)
        {
            scanDir(childDir);
        }
    }

    QDirIterator itFile(dirPath, QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    QStringList files;
    while (itFile.hasNext() && !cancel())
    {
        itFile.next();
        if (isValid(itFile.filePath()))
        {
            files.push_back(itFile.filePath());
        }
    }

    if (cancel())
    {
        return;
    }

    if (!files.empty())
    {
        sortPathList(files);
        *m_files << files;
    }    
}
