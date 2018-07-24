#ifndef LAUNCHPROCESS_H
#define LAUNCHPROCESS_H

#include <QObject>

class LaunchProcess : public QObject
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
