#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QtDebug>
#include <QtWidgets>

#include <Windows.h>
#include <WtsApi32.h>


#pragma comment(lib, "Wtsapi32.lib")

#include "LaunchProcess.h"
#include "VerifyEmbeddedSignatureThread.h"
#include "Launcher.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->LaunchPushButton, SIGNAL(pressed()), this, SLOT(launch()));
    QObject::connect(ui->NextLaunchPushButton, SIGNAL(pressed()), this, SLOT(nextLaunch()));
    QObject::connect(ui->pathToExeFilesToolButton, SIGNAL(pressed()), this, SLOT(choosePathToExeFiles()));
    QObject::connect(ui->pathToScreenShotsToolButton, SIGNAL(pressed()), this, SLOT(choosePathToScreenShots()));

    QSettings settings;

    QString pathToExeFiles = settings.value("pathToExeFiles").value<QString>();
    ui->pathToExeFilesLineEdit->setText(pathToExeFiles);

    QString commandLineArguments = settings.value("commandLineArguments").value<QString>();
    ui->commandLineArgumentsLineEdit->setText(commandLineArguments);

    QString pathToScreenShots = settings.value("pathToScreenShots").value<QString>();
    ui->pathToScreenShotsLineEdit->setText(pathToScreenShots);

    int timeOut = (settings.contains("timeOut")) ? settings.value("timeOut").value<int>() : 30;
    ui->timeOutSpinBox->setValue(timeOut);
    
    QString terminateProcessByMask = settings.value("terminateProcessByMask").value<QString>();
    if(terminateProcessByMask.isNull() || terminateProcessByMask.isEmpty()) {
        terminateProcessByMask = QString("*.exe");
    }
    ui->terminateProcessByMaskLineEdit->setText(terminateProcessByMask);

    QObject::connect(this, SIGNAL(submitLog(QString)), this, SLOT(log(QString)));

    lpctsArgs = NULL;
    dwcArgs = 0;
    currentDwProcessId = 0;
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("pathToExeFiles", ui->pathToExeFilesLineEdit->text());
    settings.setValue("commandLineArguments", ui->commandLineArgumentsLineEdit->text());
    settings.setValue("pathToScreenShots", ui->pathToScreenShotsLineEdit->text());
    settings.setValue("timeOut", ui->timeOutSpinBox->value());
    settings.setValue("terminateProcessByMask", ui->terminateProcessByMaskLineEdit->text());
    settings.sync();

    delete ui;
}

void MainWindow::choosePathToExeFiles()
{

    QString fileName = QFileDialog::getExistingDirectory(
        this,
        tr("Choose directory"),
        ui->pathToExeFilesLineEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    if(!(fileName.isNull() || fileName.isEmpty())) {
        ui->pathToExeFilesLineEdit->setText(fileName);
    }
    ui->pathToExeFilesLineEdit->setFocus();

}

void MainWindow::choosePathToScreenShots()
{

    QString fileName = QFileDialog::getExistingDirectory(
        this,
        tr("Choose directory"),
        ui->pathToScreenShotsLineEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    if(!(fileName.isNull() || fileName.isEmpty())) {
        ui->pathToScreenShotsLineEdit->setText(fileName);
    }
    ui->pathToScreenShotsLineEdit->setFocus();

}

void MainWindow::launch()
{

    ui->pathToExeFilesLineEdit->setEnabled(false);
    ui->pathToExeFilesToolButton->setEnabled(false);
    ui->commandLineArgumentsLineEdit->setEnabled(false);
    ui->pathToScreenShotsLineEdit->setEnabled(false);
    ui->LaunchPushButton->setEnabled(true);
    ui->pathToScreenShotsToolButton->setEnabled(false);

    ui->LaunchPushButton->setEnabled(false);

    if(ui->pathToExeFilesLineEdit->text().isNull() || ui->pathToExeFilesLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, tr(""), tr("Choose the directory."), QMessageBox::Cancel);
        return;
    }
    QDir dir(ui->pathToExeFilesLineEdit->text());
    if(!dir.exists()) {
        QMessageBox::critical(this, tr(""), tr("Directory doesn't exists."), QMessageBox::Cancel);
        return;
    }

    processesAtStart.clear();
    processesAtStart = getProcessesList();

    filesList.clear();
    filesList = getFilesListToLaunch();
    if(filesList.count() < 1) {
        ui->resultTextEdit->append("There are not files to launch.");
        return;
    }

    verifier = new VerifyEmbeddedSignatureThread();
    verifier->setFilesForVerify(&filesList);
    QObject::connect(verifier, SIGNAL(done(bool, QStringList)), this, SLOT(verifyBeforeLaunch(bool, QStringList)));
    verifier->start();

}

void MainWindow::verifyBeforeLaunch(bool ok, QStringList badFiles)
{
    QObject::disconnect(verifier, SIGNAL(done(bool, QStringList)), this, SLOT(verifyBeforeLaunch(bool, QStringList)));
    verifier->deleteLater();

    foreach(const QString &badFile, badFiles) {
        ui->resultTextEdit->append(badFile);
    }

    if(!ok) {

        ui->pathToExeFilesLineEdit->setEnabled(true);
        ui->pathToExeFilesToolButton->setEnabled(true);
        ui->commandLineArgumentsLineEdit->setEnabled(true);
        ui->pathToScreenShotsLineEdit->setEnabled(true);
        ui->pathToScreenShotsToolButton->setEnabled(true);
        ui->LaunchPushButton->setEnabled(true);
        ui->NextLaunchPushButton->setEnabled(false);

    } else {

        currentFile = -1;
        lctot = lcsuc = lcerr = 0;
        mtimer = NULL;

        updateStatusBar();
        StartNextPE();

    }
}


void MainWindow::nextLaunch()
{
    ui->NextLaunchPushButton->setEnabled(false);
    mtimer->terminate();
}

void MainWindow::preLaunchEvent()
{
    HKEY hKey;
    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\NorthInstaller"), 0, KEY_READ, &hKey);
    if (res == ERROR_SUCCESS)
    {
        RegDeleteTree(HKEY_CURRENT_USER, TEXT("Software\\NorthInstaller"));
    }
}

void MainWindow::StartNextPE()
{

    preLaunchEvent();

    updateStatusBar();

    if(mtimer != NULL) {
        QObject::disconnect(mtimer, SIGNAL(comingToAnEnd()), this, SLOT(doNotHaveMuchTime()));
        QObject::disconnect(mtimer, SIGNAL(finished()), this, SLOT(timeoutExceeded()));
        mtimer->deleteLater();
    }

    if (++currentFile >= filesList.count()) {
        ui->pathToExeFilesLineEdit->setEnabled(true);
        ui->pathToExeFilesToolButton->setEnabled(true);
        ui->commandLineArgumentsLineEdit->setEnabled(true);
        ui->pathToScreenShotsLineEdit->setEnabled(true);
        ui->pathToScreenShotsToolButton->setEnabled(true);
        ui->LaunchPushButton->setEnabled(true);
        ui->NextLaunchPushButton->setEnabled(false);
        return;
    }

    QString proc;
    proc = filesList.at(currentFile);

    wchar_t lpcwCommand[_MAX_PATH];

    ZeroMemory(lpcwCommand, sizeof(lpcwCommand));
    proc.toWCharArray(lpcwCommand);

    QString commandLineArgumentsString = QString("%1 %2");
    commandLineArgumentsString = commandLineArgumentsString.arg(proc);
    commandLineArgumentsString = commandLineArgumentsString.arg(ui->commandLineArgumentsLineEdit->text());

    if(lpctsArgs == NULL) {
        dwcArgs = (commandLineArgumentsString.length() + 1) * sizeof(TCHAR);
        lpctsArgs = (wchar_t*)LocalAlloc(LMEM_FIXED, dwcArgs);
    } else {

        DWORD dwcArgsNeeded = (commandLineArgumentsString.length() + 1) * sizeof(TCHAR);

        if(dwcArgsNeeded > dwcArgs) {
            LocalFree(lpctsArgs);
            dwcArgs = (commandLineArgumentsString.length() + 1) * sizeof(TCHAR);
            lpctsArgs = (wchar_t*)LocalAlloc(LMEM_FIXED, dwcArgs);
        }

    }

    ZeroMemory(lpctsArgs, dwcArgs);
    commandLineArgumentsString.toWCharArray(lpctsArgs);

    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;
    BOOL ok = CreateProcess(lpcwCommand, lpctsArgs, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);

    if (ok) {
        QString report(tr("%1 - Started successfully."));
        report = report.arg(proc);
        ui->resultTextEdit->append(report);
        currentDwProcessId = processInfo.dwProcessId;
        lctot++;
    }

    mtimer = new LaunchProcess();
    mtimer->setTimeout(ui->timeOutSpinBox->value());
    QObject::connect(mtimer, SIGNAL(comingToAnEnd()), this, SLOT(doNotHaveMuchTime()));
    QObject::connect(mtimer, SIGNAL(finished()), this, SLOT(timeoutExceeded()));
    mtimer->start();

    ui->NextLaunchPushButton->setEnabled(true);

}

QStringList MainWindow::getFilesListToLaunch()
{
    QStringList filesList;
    QDir dir(ui->pathToExeFilesLineEdit->text());
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        filesList.append(fileInfo.absoluteFilePath());
    }

    return filesList;
}

void MainWindow::log(QString logString)
{
    ui->resultTextEdit->append(logString);
}

void MainWindow::doNotHaveMuchTime()
{
    ui->NextLaunchPushButton->setEnabled(false);
}

void MainWindow::timeoutExceeded()
{
    Launcher *l = new Launcher();
    l->setmw(this);
    QObject::connect(l, SIGNAL(finished()), this, SLOT(StartNextPE()));
    l->start();
}

void MainWindow::interrupt()
{

    // +++++ ScreenShot +++++
    QFileInfo fileInfo(filesList.at(currentFile));
    fileInfo.baseName();

    QPixmap originalPixmap;
    QScreen *screen = QGuiApplication::primaryScreen();

    originalPixmap = screen->grabWindow(0);

    QString ScreenShotPathName = ui->pathToScreenShotsLineEdit->text();
    ScreenShotPathName.append("\\");
    ScreenShotPathName.append(fileInfo.baseName());
    ScreenShotPathName.append(".png");

    bool ok = originalPixmap.save(ScreenShotPathName, "png");
    if(!ok) {
        qDebug() << "Cannot save";

    }
    // ----- ScreenShot -----



    // +++++ terminate processes +++++

    QMap<int, QString> processesAtWork;
    QString report;

    int attempt = 3;
    do {

        processesAtWork = getProcessesList();
        report = QString("");

        if(processesAtWork.contains(currentDwProcessId)) {
            report = QString("%1 - %2");
            report = report.arg(processesAtWork.take(currentDwProcessId));
            bool ok = TerminateProcessById(currentDwProcessId, 1);
            if(ok) {
                report = report.arg("Terminated successfully.");
                lcsuc++;
                break;
            } else {
                report = report.arg("Failure to terminate process. Pause 5 seconds...");
                if(attempt == 1) {
                    lcerr++;
                }
            }
        } else {
            report = QString("Parent process %1 was not found. Pause 5 seconds...");
            report = report.arg(filesList.at(currentFile));
            if(attempt == 1) {
                lcerr++;
            }
        }

        QThread::sleep(5);
        attempt--;

    } while(attempt > 0);

    emit submitLog(report);

    QMap<int, QString>::const_iterator i = processesAtWork.constBegin();
    while (i != processesAtWork.constEnd()) {
        
        if(processesAtStart.contains(i.key())) {
            i++;
            continue;
        }

        if(processesAtStart.contains(currentDwProcessId)) {
            i++;
            continue;
        }
        
        
        /** +++++ Check process mask +++++ */
        
        if(
            !(ui->terminateProcessByMaskLineEdit->text().isNull()
            || ui->terminateProcessByMaskLineEdit->text().isEmpty())
        )
        {
            
            bool match = false;
            
            QString maskString = ui->terminateProcessByMaskLineEdit->text();
            
            QStringList maskStringList = maskString.split('|', QString::SkipEmptyParts);
            
            QStringList result;
            foreach (const QString &str, maskStringList) {
                qDebug() << str;
                QRegExp rx(str);
                rx.setPatternSyntax(QRegExp::Wildcard);
                match = rx.exactMatch(i.value());
                if(match) {
                    break;
                }
            }
            
            if(!match) {
                i++;
                continue;
            }
            
        }
        
        /** ----- Check process mask ----- */
        

        QString report("%1 - %2");
        report = report.arg(i.value());

        bool ok = TerminateProcessById(i.key(), 1);
        if(ok) {
            report = report.arg("Terminated successfully.");
        } else {
            report = report.arg("Failure to terminate process.");
        }

        emit submitLog(report);

        i++;
    }

    // ----- terminate processes -----

}


bool MainWindow::TerminateProcessById(int dwProcessId, int uExitCode)
{
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    if (hProcess == NULL)
        return FALSE;

    bool result = TerminateProcess(hProcess, uExitCode);

    CloseHandle(hProcess);

    return result;
}

QMap<int, QString> MainWindow::getProcessesList()
{

    QMap<int, QString> processesMap;

    WTS_PROCESS_INFO *pWPIs = NULL;
    DWORD dwProcCount = 0;
    if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
    {
        for (DWORD i = 0; i < dwProcCount; i++) {

            QString processName(QString::fromWCharArray(pWPIs[i].pProcessName));

            if(processName.isEmpty() || processName.isNull()) {
                continue;
            }

            int processId = pWPIs[i].ProcessId;

            if(processId < 1) {
                continue;
            }

            processesMap.insert(pWPIs[i].ProcessId, processName);



        }
    }

    return processesMap;

}


void MainWindow::updateStatusBar()
{
    QString statusBarMsg("Launch: %1, Success: %2, Error: %3");
    statusBarMsg = statusBarMsg.arg(lctot);
    statusBarMsg = statusBarMsg.arg(lcsuc);
    statusBarMsg = statusBarMsg.arg(lcerr);
    ui->statusBar->showMessage(statusBarMsg, 0);
}
