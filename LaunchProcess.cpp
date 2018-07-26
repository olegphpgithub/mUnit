#include "LaunchProcess.h"

#include <QString>
#include <Windows.h>
#include <WtsApi32.h>

#pragma comment(lib, "Wtsapi32.lib")

LaunchProcess::LaunchProcess(QObject *parent) : QThread(parent)
{

}

void LaunchProcess::run() {




    WTS_PROCESS_INFO *pWPIs = NULL;
    DWORD dwProcCount = 0;
    if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
    {
        for (DWORD i = 0; i < dwProcCount; i++) {

            QString procn(QString::fromWCharArray(pWPIs[i].pProcessName));
            emit log(procn);

            /*if (strcmp(szProcName, pWPIs[i].pProcessName) == 0) {

                std::cout << "Process is found" << std::endl;
                ok = TerminateMyProcess(pWPIs[i].ProcessId, 1);
                if(ok) {
                    std::cout << "Process was terminated successfully." << std::endl;
                } else {
                    std::cout << "Failure to terminate process." << std::endl;
                }

            }*/

        }
    }



    sleep(3);
    emit log(QString("Finish"));

}
