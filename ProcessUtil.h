#ifndef PROCESSUTIL_H
#define PROCESSUTIL_H

#include <Windows.h>
#include <WtsApi32.h>

#include <QThread>

class ProcessUtil : public QObject
{
    Q_OBJECT
public:
    explicit ProcessUtil(QObject *parent = 0);
    ~ProcessUtil();
    static QMap<int, QString> getProcessesList();
    static bool TerminateProcessById(int dwProcessId, int uExitCode);
    static QString pathToScreenShots;
    static QString terminateProcessByMask;
    static QMap<int, QString> processesAtStart;
    static DWORD dwCurrentProcessId;
    static int currentFile;
    static QStringList filesList;
};

#endif // PROCESSUTIL_H
