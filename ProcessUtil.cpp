#include "ProcessUtil.h"

#include <QString>
#include <QtDebug>

#include <Windows.h>
#include <WtsApi32.h>

#pragma comment(lib, "Wtsapi32.lib")

ProcessUtil::ProcessUtil(QObject *parent) : QObject(parent)
{
    qDebug() << "ProcessUtil constructor";
}

ProcessUtil::~ProcessUtil()
{
    qDebug() << "ProcessUtil destructor";
}

QMap<int, QString> ProcessUtil::getProcessesList()
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