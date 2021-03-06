#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commoncore.h"


#include <QtCore>
#include <QtDebug>
#include <QtWidgets>

#include <Windows.h>
#include <WtsApi32.h>


#pragma comment(lib, "Wtsapi32.lib")

#include "LaunchProcess.h"

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

    lpctsArgs = NULL;
    dwcArgs = 0;
    CommonCore::currentDwProcessId = 0;
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("pathToExeFiles", ui->pathToExeFilesLineEdit->text());
    settings.setValue("commandLineArguments", ui->commandLineArgumentsLineEdit->text());
    settings.setValue("pathToScreenShots", ui->pathToScreenShotsLineEdit->text());
    settings.setValue("timeOut", ui->timeOutSpinBox->value());
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

    if(ui->pathToExeFilesLineEdit->text().isNull() || ui->pathToExeFilesLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, tr(""), tr("Choose the directory."), QMessageBox::Cancel);
        return;
    }
    QDir dir(ui->pathToExeFilesLineEdit->text());
    if(!dir.exists()) {
        QMessageBox::critical(this, tr(""), tr("Directory doesn't exists."), QMessageBox::Cancel);
        return;
    }

    CommonCore::processesAtStart.clear();
    CommonCore::processesAtStart = CommonCore::getProcessesList();

    filesList.clear();
    filesList = getFilesListToLaunch();
    if(filesList.count() < 1) {
        ui->resultTextEdit->append("There are not files to launch.");
        return;
    }

    currentFile = -1;
    mtimer = NULL;
    ui->pathToExeFilesLineEdit->setEnabled(false);
    ui->pathToExeFilesToolButton->setEnabled(false);
    ui->commandLineArgumentsLineEdit->setEnabled(false);
    ui->pathToScreenShotsLineEdit->setEnabled(false);
    ui->pathToScreenShotsToolButton->setEnabled(false);
    ui->LaunchPushButton->setEnabled(false);
    ui->NextLaunchPushButton->setEnabled(true);

    StartNextPE();

}

void MainWindow::nextLaunch()
{
    mtimer->terminate();
    emit log(QString("Continue"));
}

void MainWindow::StartNextPE()
{

    if(mtimer != NULL) {
        QObject::disconnect(mtimer, SIGNAL(log(QString)), this, SLOT(log(QString)));
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
        CommonCore::currentDwProcessId = processInfo.dwProcessId;
    }

    mtimer = new LaunchProcess();
    mtimer->setTimeout(ui->timeOutSpinBox->value());
    QObject::connect(mtimer, SIGNAL(log(QString)), this, SLOT(log(QString)));
    mtimer->start();
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

    QMap<int, QString> processesAtWork = CommonCore::getProcessesList();

    QString report = QString("");

    if(processesAtWork.contains(CommonCore::currentDwProcessId)) {
        report = QString("%1 - %2");
        report = report.arg(processesAtWork.take(CommonCore::currentDwProcessId));
        bool ok = CommonCore::TerminateProcessById(CommonCore::currentDwProcessId, 1);
        if(ok) {
            report = report.arg("Terminated successfully.");
        } else {
            report = report.arg("Failure to terminate process.");
        }
    } else {
        report = QString("Parent process %1 - was not found!");
        report = report.arg(processesAtWork.take(CommonCore::currentDwProcessId));
    }

    ui->resultTextEdit->append(report);

    QMap<int, QString>::const_iterator i = processesAtWork.constBegin();
    while (i != processesAtWork.constEnd()) {

        if(CommonCore::processesAtStart.contains(i.key())) {
            i++;
            continue;
        }

        if(CommonCore::processesAtStart.contains(CommonCore::currentDwProcessId)) {
            i++;
            continue;
        }

        QString report("%1 - %2");
        report = report.arg(i.value());

        bool ok = CommonCore::TerminateProcessById(i.key(), 1);
        if(ok) {
            report = report.arg("Terminated successfully.");
        } else {
            report = report.arg("Failure to terminate process.");
        }

        ui->resultTextEdit->append(report);

        i++;
    }

    // ----- terminate processes -----

    ui->resultTextEdit->append(logString);

    StartNextPE();

}
