#include "LaunchProcess.h"

#include <QString>
#include <QtDebug>

LaunchProcess::LaunchProcess(QObject *parent) : QThread(parent)
{
    qDebug() << "LaunchProcess constructor";
}

void LaunchProcess::setTimeout(int timeOut)
{
    iTimeOut = timeOut;
}

LaunchProcess::~LaunchProcess()
{
    qDebug() << "LaunchProcess destructor";
}

void LaunchProcess::run() {

    int i = 3;
    sleep(iTimeOut - i);
    emit comingToAnEnd();
    sleep(i);

}
