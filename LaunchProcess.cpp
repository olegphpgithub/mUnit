#include "LaunchProcess.h"

#include <QString>

LaunchProcess::LaunchProcess(QObject *parent) : QObject(parent)
{

}

void LaunchProcess::run() {

   emit log(QString("Finish"));

}
