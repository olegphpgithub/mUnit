#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ProcessUtil.h"

#include <QtCore>
#include <QtDebug>
#include <QtWidgets>

#include <Windows.h>
#include <WtsApi32.h>


#pragma comment(lib, "Wtsapi32.lib")

#include "LaunchProcess.h"
#include "VerifyASProtectThread.h"
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
    
    QObject::connect(
                      ui->pathToScreenShotsLineEdit,
                      SIGNAL(textChanged(QString)),
                      this,
                      SLOT(pathToScreenShotsLineEditTextChanged(QString))
                    );
    
    QObject::connect(
                      ui->terminateProcessByMaskLineEdit,
                      SIGNAL(textChanged(QString)),
                      this,
                      SLOT(terminateProcessByMaskLineEditTextChanged(QString))
                    );

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
    if(terminateProcessByMask.isNull()) {
        terminateProcessByMask = QString("*.exe");
    }
    ui->terminateProcessByMaskLineEdit->setText(terminateProcessByMask);

    QObject::connect(this, SIGNAL(submitLog(QString)), this, SLOT(log(QString)));

    lpctsArgs = NULL;
    dwcArgs = 0;
    ProcessUtil::dwCurrentProcessId = 0;
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

void MainWindow::pathToScreenShotsLineEditTextChanged(const QString text)
{
    ProcessUtil::pathToScreenShots = text;
}

void MainWindow::terminateProcessByMaskLineEditTextChanged(const QString text)
{
    ProcessUtil::terminateProcessByMask = text;
}

void MainWindow::setGuiEnabled(bool enable)
{
    ui->pathToExeFilesLineEdit->setEnabled(enable);
    ui->pathToExeFilesToolButton->setEnabled(enable);
    ui->commandLineArgumentsLineEdit->setEnabled(enable);
    ui->pathToScreenShotsLineEdit->setEnabled(enable);
    ui->pathToScreenShotsToolButton->setEnabled(enable);
}

void MainWindow::launch()
{

    ui->LaunchPushButton->setEnabled(false);
    setGuiEnabled(false);

    try {
        
        if(ui->pathToExeFilesLineEdit->text().isNull() || ui->pathToExeFilesLineEdit->text().isEmpty()) {
            throw new QString(tr("Choose the directory."));
        }
        QDir dir(ui->pathToExeFilesLineEdit->text());
        if(!dir.exists()) {
            throw new QString(tr("Directory doesn't exists."));
        }

        ProcessUtil::processesAtStart.clear();
        ProcessUtil::processesAtStart = ProcessUtil::getProcessesList();

        ProcessUtil::filesList.clear();
        ProcessUtil::filesList = getFilesListToLaunch();
        if(ProcessUtil::filesList.count() < 1) {
            throw new QString(tr("There are not files to launch."));
        }

        verifyBeforeLaunch(false, QStringList(), nullptr);

    } catch(QString *exception) {
        
        QMessageBox::critical(this, tr("Critical Error"), *exception, QMessageBox::Cancel);
        delete exception;
        ui->LaunchPushButton->setEnabled(true);
        setGuiEnabled(true);
        
    }
}

void MainWindow::verifyBeforeLaunch(bool ok, QStringList badFiles, QObject *parent)
{

    if(parent == nullptr) {
        VerifyASProtectThread *verifyASProtectThread =
                new VerifyASProtectThread();
        verifyASProtectThread->setFilesForVerify(&ProcessUtil::filesList);
        QObject::connect(verifyASProtectThread,
                         SIGNAL(done(bool, QStringList, QObject*)),
                         this,
                         SLOT(verifyBeforeLaunch(bool, QStringList, QObject*))
                         );
        verifyASProtectThread->start();
        return;
    }

    try {

        VerifyASProtectThread *verifyASProtectThread =
                dynamic_cast<VerifyASProtectThread*>(parent);

        if(verifyASProtectThread != nullptr) {

            QObject::disconnect(verifyASProtectThread,
                                SIGNAL(done(bool, QStringList, QObject*)),
                                this,
                                SLOT(verifyBeforeLaunch(bool, QStringList, QObject*))
                                );
            verifyASProtectThread->deleteLater();

            foreach(const QString &badFile, badFiles) {
                ui->resultTextEdit->append(badFile);
            }

            if(!ok) {
                setGuiEnabled(true);
                ui->LaunchPushButton->setEnabled(true);
                ui->NextLaunchPushButton->setEnabled(false);
            } else {
                VerifyEmbeddedSignatureThread *verifier =
                        new VerifyEmbeddedSignatureThread();
                verifier->setFilesForVerify(&ProcessUtil::filesList);
                QObject::connect(verifier,
                                 SIGNAL(done(bool, QStringList, QObject*)),
                                 this,
                                 SLOT(verifyBeforeLaunch(bool, QStringList, QObject*))
                                 );
                verifier->start();
            }
            return;
        }

    } catch(const std::bad_cast&) {

    }

    try {

        VerifyEmbeddedSignatureThread *verifyEmbeddedSignatureThread =
                dynamic_cast<VerifyEmbeddedSignatureThread*>(parent);
        if(verifyEmbeddedSignatureThread != nullptr) {

            QObject::disconnect(verifyEmbeddedSignatureThread,
                                SIGNAL(done(bool, QStringList, QObject*)),
                                this,
                                SLOT(verifyBeforeLaunch(bool, QStringList, QObject*))
                                );

            verifyEmbeddedSignatureThread->deleteLater();

            foreach(const QString &badFile, badFiles) {
                ui->resultTextEdit->append(badFile);
            }

            if(!ok) {
                setGuiEnabled(true);
                ui->LaunchPushButton->setEnabled(true);
                ui->NextLaunchPushButton->setEnabled(false);
            } else {
                ProcessUtil::currentFile = -1;
                lctot = lcsuc = lcerr = 0;
                ui->statusBar->showMessage(QString(""), 0);
                mtimer = nullptr;
                StartNextPE();
            }
        }

    } catch(const std::bad_cast&) {

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

    if(mtimer != NULL) {
        QObject::disconnect(mtimer, SIGNAL(comingToAnEnd()), this, SLOT(doNotHaveMuchTime()));
        QObject::disconnect(mtimer, SIGNAL(finished()), this, SLOT(timeoutExceeded()));
        mtimer->deleteLater();
    }

    if (++ProcessUtil::currentFile >= ProcessUtil::filesList.count()) {
        setGuiEnabled(true);
        ui->LaunchPushButton->setEnabled(true);
        ui->NextLaunchPushButton->setEnabled(false);
        return;
    }

    QString proc;
    proc = ProcessUtil::filesList.at(ProcessUtil::currentFile);

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
        QFileInfo fileInfo(proc);
        QString fileName(fileInfo.fileName());
        report = report.arg(fileName);
        ui->resultTextEdit->append(report);
        ProcessUtil::dwCurrentProcessId = processInfo.dwProcessId;
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
    QObject::connect(l, SIGNAL(finished()), this, SLOT(StartNextPE()));
    QObject::connect(l, SIGNAL(submitResult(bool)), this, SLOT(updateStatusBar(bool)));
    QObject::connect(l, SIGNAL(submitLog(QString)), this, SLOT(log(QString)));
    l->start();
}


void MainWindow::updateStatusBar(bool ok)
{
    if(ok) {
        lcsuc++;
    } else {
        lcerr++;
    }
    QString statusBarMsg("Launch: %1, Success: %2, Error: %3");
    statusBarMsg = statusBarMsg.arg(lctot);
    statusBarMsg = statusBarMsg.arg(lcsuc);
    statusBarMsg = statusBarMsg.arg(lcerr);
    ui->statusBar->showMessage(statusBarMsg, 0);
}
