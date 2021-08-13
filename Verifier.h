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
    unsigned int m_VerificationOptions;
public:
    explicit Verifier(QObject *parent = nullptr);
    void setParentWidget(QWidget *parent);
    void setOptions(unsigned int options);
    bool VerifyASProtect(QString fileForVerify, QString *logString);
    bool VerifyEmbeddedSignature(QString fileForVerify, QString *logString);

    enum VerificationOptions
    {
        CHECK_CERTIFICATE_EXPIRATION = 1,
        CHECK_SECONDARY_SIGNATURE_PRESENCE = 2
    };

    static QWaitCondition wait;
    static QMutex lock;
    static bool resume;

signals:
    void confirm(QString title, QString text);
    void progress(QStringList);
    void passed(bool);
};

#endif // VERIFIER_H
