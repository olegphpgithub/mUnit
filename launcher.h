#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QtCore>

class MainWindow;

class Launcher : public QThread
{
    Q_OBJECT
public:
    explicit Launcher(QObject *parent = 0);
    QMap<int, QString> getProcessesList();
    void run();
    void interrupt();
    bool TerminateProcessById(int dwProcessId, int uExitCode);
    MainWindow *mw;
    QString currentFile;
    QString pathToScreenShots;
    QString terminateProcessByMask;
signals:
    void submitLog(QString);
    void submitResult(bool);
public slots:
};

#endif // LAUNCHER_H
