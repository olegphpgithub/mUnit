#ifndef VERIFYASPROTECTTHREAD_H
#define VERIFYASPROTECTTHREAD_H

#include <QThread>

class VerifyASProtectThread : public QThread
{
    Q_OBJECT
    void run();
    bool success;
    QStringList *filesForVerify;
public:
    static bool VerifyASProtect(QString fileForVerify, QString *logString);
    explicit VerifyASProtectThread(QObject *parent = nullptr);
    ~VerifyASProtectThread();
    void setFilesForVerify(QStringList *pFilesForVerify);
signals:
    void done(bool ok, QStringList badFiles, QObject* parent);
};

#endif // VERIFYASPROTECTTHREAD_H
