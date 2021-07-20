#include "Launcher.h"
#include "mainwindow.h"
#include "ProcessUtil.h"

#include <QtCore>
#include <QtDebug>
#include <QtWidgets>

#include <WtsApi32.h>
#include <strsafe.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

Launcher::Launcher(QObject *parent) : QThread(parent)
{
}

void Launcher::run()
{
    interrupt();
}

void Launcher::interrupt()
{
    QString report;
    bool result = true;

    // +++++ ScreenShot +++++

    try {
        
        QFileInfo fileInfo(ProcessUtil::filesList.at(ProcessUtil::currentFile));
        fileInfo.baseName();

        QPixmap originalPixmap;
        QScreen *screen = QGuiApplication::primaryScreen();

        originalPixmap = screen->grabWindow(0);

        if( !(QDir(ProcessUtil::pathToScreenShots).exists()) ) {
            if( !(QDir().mkdir(ProcessUtil::pathToScreenShots)) ) {
                throw QString("Could not create directory to save screenshot.");
            }
        }

        QString ScreenShotPathName = ProcessUtil::pathToScreenShots;
        ScreenShotPathName.append("\\");
        ScreenShotPathName.append(fileInfo.baseName());
        ScreenShotPathName.append(".png");

        bool ok = originalPixmap.save(ScreenShotPathName, "png");
        if(!ok) {
            throw QString("Could not save screenshot.");
        }
        
    } catch(QString message) {
        emit submitLog(message);
    }

    // ----- ScreenShot -----

    // +++++ terminate processes +++++

    QMap<int, QString> processesAtWork;

    int attempt = 3;
    do {
        processesAtWork = ProcessUtil::getProcessesList();
        report = QString("");

        QFileInfo fileInfo(ProcessUtil::filesList.at(ProcessUtil::currentFile));
        QString fileName(fileInfo.fileName());

        QList<int> processIds = processesAtWork.keys(fileName);

        if (!processIds.isEmpty())
        {
            int succeeded = 0;
            foreach(const int &processId, processIds)
            {
                report = QString("%1 - %2");
                report = report.arg(processesAtWork.take(processId));
                bool ok = ProcessUtil::TerminateProcessById(processId, 1);
                if (ok)
                {
                    succeeded++;
                    report = report.arg("Terminated successfully.");
                    emit submitLog(report);
                }
                else
                {
                    report = report.arg("Failure to terminate process. Try in 5 seconds...");
                }
            }
            if (processIds.length() == succeeded)
            {
                break;
            }
        }
        else
        {
            report = QString("%1 was not found. Try in 5 seconds...");
            report = report.arg(fileName);
        }

        emit submitLog(report);

        if (attempt == 1)
        {
            result = false;
            break;
        }
        if (attempt > 0)
        {
            QThread::sleep(5);
        }
        attempt--;

    } while(attempt > 0);

    // +++++ terminate new processes by mask +++++

    if(
        !(ProcessUtil::terminateProcessByMask.isNull()
        || ProcessUtil::terminateProcessByMask.isEmpty())
    )
    {

        QMap<int, QString>::const_iterator i = processesAtWork.constBegin();
        while (i != processesAtWork.constEnd()) {

            if(ProcessUtil::processesAtStart.contains(i.key())) {
                i++;
                continue;
            }

            if(ProcessUtil::processesAtStart.contains(ProcessUtil::dwCurrentProcessId)) {
                i++;
                continue;
            }

            bool match = false;

            QStringList maskStringList = ProcessUtil::terminateProcessByMask.split('|', QString::SkipEmptyParts);

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

            QString report("%1 - %2");
            report = report.arg(i.value());

            bool ok = ProcessUtil::TerminateProcessById(i.key(), 1);
            if(ok) {
                report = report.arg("Terminated successfully.");
            } else {
                report = report.arg("Failure to terminate process.");
            }

            emit submitLog(report);

            i++;

        }

    }

    // ----- terminate new processes by mask -----

    // ----- terminate processes -----

    emit submitResult(result);

    ClearRegistryKeys();
}

void Launcher::ClearRegistryKeys()
{

    HKEY hKey;

    TCHAR lptstrKeyPath[MAX_KEY_LENGTH];
    TCHAR lptstrSearchScope[MAX_KEY_LENGTH] = TEXT("Software");

    DWORD lpcdwSubKeys2 = 0;
    DWORD lpcdwSubValues2 = 0;

    TCHAR lptstrValueName[MAX_VALUE_NAME];
    DWORD dwcchValueName = MAX_VALUE_NAME;

    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, lptstrSearchScope, 0, KEY_READ | KEY_WRITE, &hKey);
    if (res == ERROR_SUCCESS)
    {

        TCHAR HKUClass[MAX_PATH] = TEXT("");
        DWORD dwcchClass = MAX_PATH;
        DWORD dwcSubKeys = 0;

        res = RegQueryInfoKey (hKey,
            HKUClass,
            &dwcchClass,
            NULL,
            &dwcSubKeys,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        );

        if(res == ERROR_SUCCESS) {

            QStringList NNRusMutexsStringList;

            for (DWORD i=0; i<dwcSubKeys; i++) {

                DWORD dwcbName = MAX_KEY_LENGTH;
                res = RegEnumKeyEx(hKey, i,
                    lptstrKeyPath,
                    &dwcbName,
                    NULL,
                    NULL,
                    NULL,
                    NULL
                );

                if(res == ERROR_SUCCESS) {

                    TCHAR lptstrNNRusMutexPath[MAX_KEY_LENGTH];
                    StringCchPrintf(lptstrNNRusMutexPath, MAX_KEY_LENGTH, TEXT("%s\\%s"), lptstrSearchScope, lptstrKeyPath);

                    HKEY hKeyNNRusMutex;
                    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER,
                        lptstrNNRusMutexPath,
                        0,
                        KEY_READ | KEY_WRITE,
                        &hKeyNNRusMutex
                    );

                    if (res == ERROR_SUCCESS) {

                        /** +++++ Registry key must have only one value */

                        res = RegQueryInfoKey (hKeyNNRusMutex,
                            NULL,
                            NULL,
                            NULL,
                            &lpcdwSubKeys2,
                            NULL,
                            NULL,
                            &lpcdwSubValues2,
                            NULL,
                            NULL,
                            NULL,
                            NULL
                        );

                        if( (lpcdwSubKeys2 == 0) && (lpcdwSubValues2 == 1) ) {

                            /** Name must have 2 chars */

                            dwcchValueName = MAX_VALUE_NAME;
                            ZeroMemory(lptstrValueName, MAX_VALUE_NAME);

                            res = RegEnumValue(hKeyNNRusMutex,
                                0,
                                lptstrValueName,
                                &dwcchValueName,
                                NULL,
                                NULL,
                                NULL,
                                NULL
                            );

                            if(res == ERROR_SUCCESS) {
                                if(dwcchValueName == 2) {
                                    NNRusMutexsStringList.append(QString::fromWCharArray(lptstrNNRusMutexPath));
                                }
                            }

                        }

                    }

                    RegCloseKey(hKeyNNRusMutex);

                }

            }

            foreach (const QString &str, NNRusMutexsStringList) {
                
                TCHAR *lptstrNNRusMutexKeyPath;
                DWORD dwcchNNRusMutexKeyPath = str.length() + 1;
                
                lptstrNNRusMutexKeyPath = new TCHAR[dwcchNNRusMutexKeyPath];
                ZeroMemory(lptstrNNRusMutexKeyPath, dwcchNNRusMutexKeyPath * sizeof(TCHAR));
                
                str.toWCharArray(lptstrNNRusMutexKeyPath);
                
                res = RegDeleteTree(
                    HKEY_CURRENT_USER,
                    lptstrNNRusMutexKeyPath
                );
                
                if(res == ERROR_SUCCESS) {
                    QString logString("Registry key %2 was removed successfully");
                    logString = logString.arg(str);
                    emit submitLog(logString);
                }
                
                delete lptstrNNRusMutexKeyPath;
                
            }
            
        }

    }
}
