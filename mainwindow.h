#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include <Windows.h>

class LaunchProcess;
class VerifyEmbeddedSignatureThread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void interrupt();

public slots:
    void setGuiEnabled(bool enable);
    void launch();
    void verifyBeforeLaunch(bool ok, QStringList badFiles);
    void nextLaunch();
    void log(QString);
    void doNotHaveMuchTime();
    void timeoutExceeded();
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
    VerifyEmbeddedSignatureThread *verifier;
    wchar_t *lpctsArgs;
    DWORD dwcArgs;
    DWORD currentDwProcessId;
    int lctot, lcsuc, lcerr;
    void updateStatusBar();
    void preLaunchEvent();
signals:
    void submitLog(QString);
};

#endif // MAINWINDOW_H
