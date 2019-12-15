#include "VerifyASProtectThread.h"

#include <QString>
#include <QtDebug>
#include <QFileInfo>

VerifyASProtectThread::VerifyASProtectThread(QObject *parent) : QThread(parent)
{

}

VerifyASProtectThread::~VerifyASProtectThread()
{
    qDebug() << "VerifyASProtectThread destructor";
}

void VerifyASProtectThread::setFilesForVerify(QStringList *pFilesForVerify)
{
    filesForVerify = pFilesForVerify;
}

void VerifyASProtectThread::run()
{
    success = true;
    QStringList logStringList;
    emit done(success, logStringList);
}


bool VerifyASProtectThread::VerifyASProtect(QString fileForVerify, QString *logString)
{
    bool success = false;
    return success;
}

