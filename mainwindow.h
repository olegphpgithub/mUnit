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
    
    static DWORD currentDwProcessId;
    static QStringList filesList;
    static QMap<int, QString> processesAtStart;
    
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
    void updateStatusBar(bool ok);

private:
    Ui::MainWindow *ui;
    
    QStringList getFilesListToLaunch();
    int currentFile;
    LaunchProcess *mtimer;
    VerifyEmbeddedSignatureThread *verifier;
    wchar_t *lpctsArgs;
    DWORD dwcArgs;
    int lctot, lcsuc, lcerr;
    
    void preLaunchEvent();

signals:
    void submitLog(QString);
};

#endif // MAINWINDOW_H
