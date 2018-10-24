#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include <Windows.h>

class LaunchProcess;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void interrupt(QString logString);

public slots:
    void launch();
    void nextLaunch();
    void log(QString);
    void timedout(QString);
    void choosePathToExeFiles();
    void choosePathToScreenShots();
    void StartNextPE();

private:
    Ui::MainWindow *ui;
    QMap<int, QString> processesAtStart;
    QMap<int, QString> getProcessesList();
    bool TerminateProcessById(int dwProcessId, int uExitCode);
    QStringList filesList;
    QStringList getFilesListToLaunch();
    int currentFile;
    LaunchProcess *mtimer;
    wchar_t *lpctsArgs;
    DWORD dwcArgs;
    DWORD currentDwProcessId;
signals:
    void submitLog(QString);
};

#endif // MAINWINDOW_H
