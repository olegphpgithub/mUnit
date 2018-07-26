#ifndef LAUNCHPROCESS_H
#define LAUNCHPROCESS_H

#include <QThread>

class LaunchProcess : public QThread
{
    Q_OBJECT
    void run();
public:
    explicit LaunchProcess(QObject *parent = 0);

signals:
    void log(QString);
public slots:
};

#endif // LAUNCHPROCESS_H
