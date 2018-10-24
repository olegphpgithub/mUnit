#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QtCore>

class MainWindow;

class Launcher : public QThread
{
    Q_OBJECT
public:
    explicit Launcher(QObject *parent = 0);
    void run();
    void setmw(MainWindow *w);
    MainWindow *mw;
signals:

public slots:
};

#endif // LAUNCHER_H
