#ifndef VERIFIER_H
#define VERIFIER_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class Verifier : public QThread
{
    Q_OBJECT
    void run();
    QWidget *parentWidget;
public:
    explicit Verifier(QObject *parent = nullptr);
    void setParentWidget(QWidget *parent);
    bool VerifyASProtect(QString fileForVerify, QString *logString);
    bool VerifyEmbeddedSignature(QString fileForVerify, QString *logString);

    static QWaitCondition wait;
    static QMutex lock;
    static bool resume;

signals:
    void confirm(QString title, QString text);
    void progress(QStringList);
    void passed(bool);
};

#endif // VERIFIER_H
