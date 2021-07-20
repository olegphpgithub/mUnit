#ifndef SPYWINDOW_H
#define SPYWINDOW_H

#include <QThread>

#include <Windows.h>

class SpyWindow : public QThread
{
    Q_OBJECT
    void run();
    static BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam);
    static BOOL CALLBACK FindItemProc(HWND hwnd, LPARAM lParam);
    DWORD wantedProcessId;
    QString wantedWindowClass;
    QString wantedItemCaption;
public:
    explicit SpyWindow(DWORD wantedProcessId, QString wantedWindowClass, QString wantedItemCaption, QObject *parent = nullptr);
    ~SpyWindow();
};

#endif // SPYWINDOW_H
