#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    QString pathToScreenShots = settings.value("pathToScreenShots").value<QString>();
    ui->pathToScreenShotsLineEdit->setText(pathToScreenShots);

    int timeOut = (settings.contains("timeOut")) ? settings.value("timeOut").value<int>() : 30;
    ui->timeOutSpinBox->setValue(timeOut);

}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("pathToExeFiles", ui->pathToExeFilesLineEdit->text());
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

    processesAtStart.clear();
    processesAtStart = getProcessesList();

    filesList.clear();
    filesList = getFilesListToLaunch();
    if(filesList.count() < 1) {
        ui->resultTextEdit->append("There are not files to launch.");
        return;
    }

    currentFile = -1;
    mtimer = NULL;
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
        ui->LaunchPushButton->setEnabled(true);
        ui->NextLaunchPushButton->setEnabled(false);
        return;
    }

    QString proc;
    proc = filesList.at(currentFile);

    wchar_t lpcwCommand[_MAX_PATH];

    ZeroMemory(lpcwCommand, sizeof(lpcwCommand));
    proc.toWCharArray(lpcwCommand);

    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;
    BOOL ok = CreateProcess(lpcwCommand, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);

    if (ok) {
        QString report(tr("%1 - Started successfully."));
        report = report.arg(proc);
        ui->resultTextEdit->append(report);
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


    QMap<int, QString> processesAtWork = getProcessesList();

    QMap<int, QString>::const_iterator i = processesAtWork.constBegin();
    while (i != processesAtWork.constEnd()) {

        if(processesAtStart.contains(i.key())) {
            i++;
            continue;
        }

        QString report("%1 - %2");
        report = report.arg(i.value());

        bool ok = TerminateProcessById(i.key(), 1);
        if(ok) {
            report = report.arg("Terminated successfully.");
        } else {
            report = report.arg("Failure to terminate process.");
        }

        ui->resultTextEdit->append(report);

        i++;
    }

    ui->resultTextEdit->append(logString);

    StartNextPE();

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
