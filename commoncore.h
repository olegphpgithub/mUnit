#ifndef COMMONCORE_H
#define COMMONCORE_H

#include <QtCore>
#include <Windows.h>

class CommonCore : public QObject
{
    Q_OBJECT
public:
    explicit CommonCore(QObject *parent = 0);
    static DWORD currentDwProcessId;
    static QMap<int, QString> processesAtStart;
    static QMap<int, QString> getProcessesList();
    static bool TerminateProcessById(int dwProcessId, int uExitCode);
signals:

public slots:
};

#endif // COMMONCORE_H
