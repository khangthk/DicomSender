#include "storethread.h"
#include "storedcmtk.h"
#include "storegdcm.h"

#include <QDebug>

StoreThread::StoreThread(const Library lib, QObject *parent)
    : QThread(parent)
{
    Q_ASSERT(lib != Library::none);
    Library::dcmtk == lib ? m_store.reset(new StoreDcmtk()) : m_store.reset(new StoreGdcm());
    connect(m_store.get(), &StoreBase::progress, this, &StoreThread::progress);
    connect(m_store.get(), &StoreBase::processed, this, &StoreThread::processed);
    connect(m_store.get(), &StoreBase::result, this, &StoreThread::result);
}

StoreThread::~StoreThread()
{
    qDebug() << "~StoreThread";
}

StoreBase *StoreThread::object()
{
    return m_store.get();
}

void StoreThread::run()
{
    m_store->store();
}
