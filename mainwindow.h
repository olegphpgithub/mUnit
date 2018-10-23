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

public slots:
    void launch();
    void nextLaunch();
    void log(QString);
    void choosePathToExeFiles();
    void choosePathToScreenShots();

private:
    Ui::MainWindow *ui;
    QMap<int, QString> processesAtStart;
    QMap<int, QString> getProcessesList();
    bool TerminateProcessById(int dwProcessId, int uExitCode);
    void StartNextPE();
    QStringList filesList;
    QStringList getFilesListToLaunch();
    int currentFile;
    LaunchProcess *mtimer;
    wchar_t *lpctsArgs;
    DWORD dwcArgs;
    //DWORD currentDwProcessId;
};

#endif // MAINWINDOW_H
