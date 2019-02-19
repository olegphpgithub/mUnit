#include "Launcher.h"
#include "mainwindow.h"
#include "ProcessUtil.h"


#include <QtCore>
#include <QtDebug>
#include <QtWidgets>

#include <Windows.h>
#include <WtsApi32.h>


Launcher::Launcher(QObject *parent) : QThread(parent)
{

}

void Launcher::run()
{
    interrupt();
}

void Launcher::interrupt()
{

    // +++++ ScreenShot +++++
    QFileInfo fileInfo(currentFile);
    fileInfo.baseName();

    QPixmap originalPixmap;
    QScreen *screen = QGuiApplication::primaryScreen();

    originalPixmap = screen->grabWindow(0);

    QString ScreenShotPathName = ProcessUtil::pathToScreenShots;
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

        processesAtWork = ProcessUtil::getProcessesList();
        report = QString("");

        if(processesAtWork.contains(MainWindow::currentDwProcessId)) {
            report = QString("%1 - %2");
            report = report.arg(processesAtWork.take(MainWindow::currentDwProcessId));
            bool ok = ProcessUtil::TerminateProcessById(MainWindow::currentDwProcessId, 1);
            if(ok) {
                report = report.arg("Terminated successfully.");
                emit submitResult(true);
                emit submitLog(report);
                break;
            } else {
                report = report.arg("Failure to terminate process. Try in 5 seconds...");
            }
        } else {
            report = QString("%1 was not found. Try in 5 seconds...");
            MainWindow::filesList;
            QFileInfo fileInfo(currentFile);
            QString fileName(fileInfo.fileName());
            report = report.arg(fileName);
        }

        emit submitLog(report);
        if(attempt > 0) {
            QThread::sleep(5);
        }
        if(attempt == 1) {
            emit submitResult(false);
        }
        attempt--;

    } while(attempt > 0);

    QMap<int, QString>::const_iterator i = processesAtWork.constBegin();
    while (i != processesAtWork.constEnd()) {
        
        if(MainWindow::processesAtStart.contains(i.key())) {
            i++;
            continue;
        }

        if(MainWindow::processesAtStart.contains(MainWindow::currentDwProcessId)) {
            i++;
            continue;
        }
        
        
        /** +++++ Check process mask +++++ */
        
        if(
            !(ProcessUtil::terminateProcessByMask.isNull()
            || ProcessUtil::terminateProcessByMask.isEmpty())
        )
        {
            
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
            
        }
        
        /** ----- Check process mask ----- */
        

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

    // ----- terminate processes -----

}

