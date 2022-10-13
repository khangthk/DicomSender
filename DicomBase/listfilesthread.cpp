#include "listfilesthread.h"

#include <QDebug>

ListFilesThread::ListFilesThread(QObject *parent)
    : QThread(parent)
{
    m_scan.reset(new ScanBase());
    connect(m_scan.get(), &ScanBase::done, this, &ListFilesThread::done);
}

ListFilesThread::~ListFilesThread()
{
    qDebug() << "~ListFilesThread";
}

ScanBase *ListFilesThread::object()
{
    return m_scan.get();
}

void ListFilesThread::run()
{
    m_scan->scan();
}
