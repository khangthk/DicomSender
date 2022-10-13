#include "echobase.h"

#include <QDebug>

EchoBase::EchoBase(QObject *parent)
    : SCUInfo(parent)
{}

EchoBase::~EchoBase()
{
    qDebug() << "~EchoBase";
}
