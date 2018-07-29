#include "LaunchProcess.h"

#include <QString>
#include <QtDebug>

LaunchProcess::LaunchProcess(QObject *parent) : QThread(parent)
{
    qDebug() << "LaunchProcess";
}

LaunchProcess::~LaunchProcess()
{
    qDebug() << "destroy LaunchProcess";
}

void LaunchProcess::run() {


    sleep(20);
    emit log(QString("Finish"));

}
