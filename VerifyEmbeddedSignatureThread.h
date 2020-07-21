#ifndef VERIFYEMBEDDEDSIGNATURETHREAD_H
#define VERIFYEMBEDDEDSIGNATURETHREAD_H

#include <QThread>

#include <Windows.h>

class QStringList;

class VerifyEmbeddedSignatureThread : public QThread
{
    Q_OBJECT
    void run();
    bool success;
    QStringList *filesForVerify;
public:
    static bool VerifyEmbeddedSignature(QString fileForVerify, QString *logString);
    explicit VerifyEmbeddedSignatureThread(QObject *parent = nullptr);
    ~VerifyEmbeddedSignatureThread();
    void setFilesForVerify(QStringList *pFilesForVerify);
signals:
    void done(bool ok, QStringList badFiles, QObject* parent);
};

#endif // VERIFYEMBEDDEDSIGNATURETHREAD_H
