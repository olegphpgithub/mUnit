#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QtCore>
#include <string.h>

class MainWindow;

class Launcher : public QThread
{
    Q_OBJECT
public:
    explicit Launcher(QObject *parent = 0);
    void run();
    void interrupt();
    void ClearRegistryKeys();
    QString pathToScreenShots;
signals:
    void submitLog(QString);
    void submitResult(bool);
public slots:
};

#endif // LAUNCHER_H
