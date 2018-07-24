#include "LaunchProcess.h"

#include <QString>

LaunchProcess::LaunchProcess(QObject *parent) : QObject(parent)
{

}

void LaunchProcess::run() override {

           emit log(QString("Finish"));

       }
