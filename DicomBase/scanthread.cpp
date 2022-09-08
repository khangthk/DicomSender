#include "scanthread.h"
#include "scandcmtk.h"
#include "scangdcm.h"

#include <QDebug>

ScanThread::ScanThread(const Library &lib, QObject *parent)
    : QThread(parent)
{
    Library::dcmtk == lib ? m_scan.reset(new ScanDcmtk()) : m_scan.reset(new ScanGdcm());
    connect(m_scan.get(), &ScanBase::done, this, &ScanThread::done);
}

ScanThread::~ScanThread()
{
    qDebug() << "~ScanThread";
}

ScanBase *ScanThread::object()
{
    return m_scan.get();
}

void ScanThread::run()
{
    m_scan->scan();
}
