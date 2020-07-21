#include "Verifier.h"
#include "VerifyASProtectThread.h"
#include "ProcessUtil.h"
#include "VerifyASProtectThread.h"
#include "VerifyEmbeddedSignatureThread.h"

#include <QWidget>
#include <QMessageBox>
#include <QDebug>


QWaitCondition Verifier::wait;
QMutex Verifier::lock;
bool Verifier::resume = true;

Verifier::Verifier(QObject *parent) : QThread(parent)
{

}

void Verifier::setParentWidget(QWidget *parent)
{
    parentWidget = parent;
}

void Verifier::run()
{
    resume = true;
    lock.lock();

    bool success = true;
    QStringList logStringList;

    foreach(const QString fileForVerify, ProcessUtil::filesList) {

        QString logString;

        bool ok = VerifyASProtectThread::VerifyASProtect(fileForVerify, &logString);
        if(!ok) {
            success = false;
            logStringList.append(logString);
        }

    }

    if(success) {
        logStringList.append(QString("All files were protected."));
    }

    emit progress(logStringList);
    if(!success)
    {
        emit confirm(QString("Warning"), QString("ASProtect verification failed. Do you want to continue?"));
        wait.wait(&lock);
    }

    if(!resume)
    {
        emit passed(false);
        lock.unlock();
        return;
    }

    success = true;
    logStringList.clear();

    foreach(const QString fileForVerify, ProcessUtil::filesList) {

        QString logString;

        bool ok = VerifyEmbeddedSignatureThread::VerifyEmbeddedSignature(fileForVerify, &logString);
        if(!ok) {
            success = false;
            logStringList.append(logString);
        }

    }

    if(success) {
        logStringList.append(QString("All files were signed and the signatures were verified."));
    }

    emit progress(logStringList);
    if(!success)
    {
        emit confirm(QString("Warning"), QString("Signature verification failed. Do you want to continue?"));
        wait.wait(&lock);
    }

    if(!resume)
    {
        emit passed(false);
        lock.unlock();
        return;
    }
    emit passed(true);
    lock.unlock();
}
