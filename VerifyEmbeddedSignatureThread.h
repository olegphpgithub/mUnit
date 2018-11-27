#ifndef VERIFYEMBEDDEDSIGNATURETHREAD_H
#define VERIFYEMBEDDEDSIGNATURETHREAD_H

#include <QThread>

#include <windows.h>

class QStringList;

class VerifyEmbeddedSignatureThread : public QThread
{
    Q_OBJECT
    void run();
    bool success;
    QStringList *filesForVerify;
    bool VerifyEmbeddedSignature(QString fileForVerify, QString *logString);
public:
    explicit VerifyEmbeddedSignatureThread(QObject *parent = 0);
    ~VerifyEmbeddedSignatureThread();
    void setFilesForVerify(QStringList *pFilesForVerify);
signals:
    void done(bool ok, QStringList badFiles);
};

#endif // VERIFYEMBEDDEDSIGNATURETHREAD_H
