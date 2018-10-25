#include "launcher.h"
#include "mainwindow.h"

Launcher::Launcher(QObject *parent) : QThread(parent)
{

}

void Launcher::setmw(MainWindow *w)
{
    this->mw = w;
}

void Launcher::run()
{
    this->mw->interrupt();
}
