#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QtDebug>

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::launch()
{
    processesAtStart.clear();
    processesAtStart = getProcessesList();



    ui->resultTextEdit->append("Launch 1");
    ui->resultTextEdit->append(QString::number(processesAtStart.count()));

    filesList.clear();
    filesList = getFilesListToLaunch();
    if(filesList.count() < 1) {
        ui->resultTextEdit->append("There are not files to launch.");
        return;
    }
    qDebug() << QString("launch 19");

    currentFile = -1;
    mtimer = NULL;

    StartNextPE();

    /** --- */

}

void MainWindow::StartNextPE()
{
    qDebug() << "+++";

    if (++currentFile >= filesList.count()) {
        return;
    }
    qDebug() << "continue";

    QString proc;
    proc = filesList.at(currentFile);

    qDebug() << proc;
    if(proc == NULL) {
        return;
    }

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

    if(mtimer != NULL) {
        QObject::disconnect(mtimer, SIGNAL(log(QString)), this, SLOT(log(QString)));
        mtimer->deleteLater();
    }

    LaunchProcess *mtimer = new LaunchProcess(this);
    QObject::connect(mtimer, SIGNAL(log(QString)), this, SLOT(log(QString)));
    mtimer->start();


}

QStringList MainWindow::getFilesListToLaunch()
{
    QStringList filesList;
    QDir dir("c:\\tmp");
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

    //process->deleteLater();

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
