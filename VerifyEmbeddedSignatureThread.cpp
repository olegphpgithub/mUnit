#include "VerifyEmbeddedSignatureThread.h"

#include <QString>
#include <QtDebug>

VerifyEmbeddedSignatureThread::VerifyEmbeddedSignatureThread(QObject *parent) : QThread(parent)
{
    qDebug() << "VerifyEmbeddedSignatureThread constructor";
}

VerifyEmbeddedSignatureThread::~VerifyEmbeddedSignatureThread()
{
    qDebug() << "VerifyEmbeddedSignatureThread destructor";
}

void VerifyEmbeddedSignatureThread::run() {

    sleep(2);
    QStringList badFilesStringList;
    badFilesStringList.append(QString("success"));
    emit done(true, badFilesStringList);

}
