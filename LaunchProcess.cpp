#include "LaunchProcess.h"

#include <QString>


LaunchProcess::LaunchProcess(QObject *parent) : QThread(parent)
{

}

void LaunchProcess::run() {


    sleep(15);
    emit log(QString("Finish"));

}
