#include "mainwindow.h"
#include <QApplication>

#include <Windows.h>

#pragma comment(lib, "Advapi32.lib")

BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode)
{
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    if (hProcess == NULL)
        return FALSE;

    BOOL result = TerminateProcess(hProcess, uExitCode);

    CloseHandle(hProcess);

    return result;
}

BOOL gIsWinXP()
{
    OSVERSIONINFO osvi = { 0 };
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    return osvi.dwMajorVersion <= 5;
}

bool IsRunAsAdmin()
{
    if (gIsWinXP())
        return TRUE;

    BOOL fIsRunAsAdmin = FALSE;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdministratorsGroup))
    {
        // Determine whether the SID of administrators group is enabled in
        // the primary access token of the process.
        CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin);
    }

    // Cleanup for all allocated resources.
    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    return fIsRunAsAdmin;

}

bool elevateProcess()
{
    bool fIsRunAsAdmin;

    fIsRunAsAdmin = IsRunAsAdmin();

    if (!fIsRunAsAdmin)
    {
        wchar_t szPath[MAX_PATH];
        if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
        {
            // Launch itself as administrator.
            SHELLEXECUTEINFO sei = { sizeof(sei) };
            sei.lpVerb = L"runas";
            sei.lpFile = szPath;
            sei.hwnd = NULL;
            sei.nShow = SW_NORMAL;

            if (!ShellExecuteEx(&sei)) {

                DWORD dwError = GetLastError();
                if (dwError == ERROR_CANCELLED)
                {
                    // The user refused the elevation.
                    // Do nothing.
                } else {
                    // The user accept the elevation.
                    // Do nothing.
                    fIsRunAsAdmin = true;
                }
            }
        }
    }

    return fIsRunAsAdmin;

}

int main(int argc, char *argv[])
{

    if(!elevateProcess())
        return -1;

    QCoreApplication::setOrganizationName("NetNucleusRus");
    QCoreApplication::setOrganizationDomain("nn-rus.com");
    QCoreApplication::setApplicationName("mUnit");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
