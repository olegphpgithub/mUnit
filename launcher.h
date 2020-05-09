#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QtCore>

#include <tchar.h>
#include <string.h>
#include <Windows.h>

class MainWindow;

class Launcher : public QThread
{
    Q_OBJECT
public:
    explicit Launcher(QObject *parent = 0);
    void run();
    static BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam);
    void interrupt();
    void ClearRegistryKeys();
    bool TerminateProcessById(int dwProcessId, int uExitCode);
    QString pathToScreenShots;
    bool findCommunicationWindow;
signals:
    void submitLog(QString);
    void submitResult(bool);
public slots:
};

#endif // LAUNCHER_H
