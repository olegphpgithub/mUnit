#include "LaunchProcess.h"

#include <QString>
#include <QtDebug>

LaunchProcess::LaunchProcess(QObject *parent) : QThread(parent)
{
    qDebug() << "LaunchProcess constructor";
}

LaunchProcess::~LaunchProcess()
{
    qDebug() << "LaunchProcess destructor";
}

void LaunchProcess::run() {


    sleep(30);
    emit log(QString("Finish"));

}
