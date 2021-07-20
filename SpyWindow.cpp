#include "SpyWindow.h"

#include <QtDebug>

SpyWindow::SpyWindow(DWORD wantedProcessId, QString wantedWindowClass, QString wantedItemCaption, QObject *parent) : QThread(parent)
{
    qDebug() << "SpyWindow constructor";
    this->wantedProcessId = wantedProcessId;
    this->wantedWindowClass = wantedWindowClass;
    this->wantedItemCaption = wantedItemCaption;
}

SpyWindow::~SpyWindow()
{
    qDebug() << "SpyWindow destructor";
}

void SpyWindow::run()
{
    int i = 1;
    while (true)
    {
        sleep(i);
        BOOL bWindowFound = ! EnumWindows(FindWindowProc, reinterpret_cast<LPARAM>(this));
        if (bWindowFound)
        {
            break;
        }
    }
    sleep(i);
}

BOOL CALLBACK SpyWindow::FindWindowProc(HWND hwnd, LPARAM lParam)
{
    SpyWindow *lpSpyWindow = reinterpret_cast<SpyWindow*>(lParam);
    DWORD dwProcessId;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId == lpSpyWindow->wantedProcessId)
    {
        TCHAR lpwszClassName[100] = { 0 };
        if (GetClassNameW(hwnd, lpwszClassName, 100))
        {
            QString strClassName = QString::fromWCharArray(lpwszClassName);
            if (strClassName == lpSpyWindow->wantedWindowClass)
            {
                BOOL bItemFound = ! EnumChildWindows(hwnd, FindItemProc, lParam);
                if (bItemFound)
                {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

BOOL CALLBACK SpyWindow::FindItemProc(HWND hwnd, LPARAM lParam)
{
    SpyWindow *lpSpyWindow = reinterpret_cast<SpyWindow*>(lParam);
    WCHAR lpwszCaption[1024] = { 0 };
    GetWindowTextW(hwnd, lpwszCaption, 1024);
    QString strCaption = QString::fromWCharArray(lpwszCaption);
    if (strCaption == lpSpyWindow->wantedItemCaption)
    {
        return FALSE;
    }
    return TRUE;
}
