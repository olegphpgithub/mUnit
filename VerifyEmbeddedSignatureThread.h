#ifndef VERIFYEMBEDDEDSIGNATURETHREAD_H
#define VERIFYEMBEDDEDSIGNATURETHREAD_H

#include <QThread>

class QStringList;

class VerifyEmbeddedSignatureThread : public QThread
{
    Q_OBJECT
    void run();
public:
    explicit VerifyEmbeddedSignatureThread(QObject *parent = 0);
    ~VerifyEmbeddedSignatureThread();
signals:
    void done(bool ok, QStringList badFiles);
};

#endif // VERIFYEMBEDDEDSIGNATURETHREAD_H
