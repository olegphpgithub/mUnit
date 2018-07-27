#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>

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

    LaunchProcess *lp = new LaunchProcess();
    QObject::connect(lp, SIGNAL(log(QString)), this, SLOT(log(QString)));
    lp->start();

    ui->resultTextEdit->append("Launch 1");
    ui->resultTextEdit->append(QString::number(processesAtStart.count()));

    QDir dir("c:\\tmp");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        ui->resultTextEdit->append(QString("%1")
                                   .arg(fileInfo.absoluteFilePath()));
    }

    //lp->deleteLater();
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

    ui->resultTextEdit->append(logString);
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
