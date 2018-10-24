#ifndef LAUNCHPROCESS_H
#define LAUNCHPROCESS_H

#include <QThread>

class LaunchProcess : public QThread
{
    Q_OBJECT
    void run();
    int iTimeOut;
public:
    explicit LaunchProcess(QObject *parent = 0);
    ~LaunchProcess();
    void setTimeout(int timeOut);
signals:
    void comingToAnEnd();
    void log(QString);
public slots:
};

#endif // LAUNCHPROCESS_H
