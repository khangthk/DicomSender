#include "scanthread.h"
#include "scandcmtk.h"
#include "scangdcm.h"

#include <QDebug>

ScanThread::ScanThread(const Library lib, QObject *parent)
    : QThread(parent)
{
    switch (lib)
    {
    case Library::dcmtk:
        m_scan.reset(new ScanDcmtk());
        break;
    case Library::gdcm:
        m_scan.reset(new ScanGdcm());
        break;
    default:
        m_scan.reset(new ScanBase());
        break;
    }

    connect(m_scan.get(), &ScanBase::result, this, &ScanThread::result);
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
