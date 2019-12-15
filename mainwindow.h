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
    void verifyBeforeLaunch(bool ok, QStringList badFiles, QObject *verifier);
    void nextLaunch();
    void log(QString);
    void doNotHaveMuchTime();
    void timeoutExceeded();
    void choosePathToExeFiles();
    void choosePathToScreenShots();
    void StartNextPE();
    void updateStatusBar(bool ok);
    void pathToScreenShotsLineEditTextChanged(const QString text);
    void terminateProcessByMaskLineEditTextChanged(const QString text);

private:
    Ui::MainWindow *ui;
    
    QStringList getFilesListToLaunch();
    LaunchProcess *mtimer;
    wchar_t *lpctsArgs;
    DWORD dwcArgs;
    int lctot, lcsuc, lcerr;
    enum {
        ASProtectVerification,
        EmbeddedSignatureVerification
    };
    void preLaunchEvent();

signals:
    void submitLog(QString);
};

#endif // MAINWINDOW_H
