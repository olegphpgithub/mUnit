#include "VerifyASProtectThread.h"

#include <QString>
#include <QtDebug>
#include <QFileInfo>

#include <Windows.h>

VerifyASProtectThread::VerifyASProtectThread(QObject *parent) : QThread(parent)
{

}

VerifyASProtectThread::~VerifyASProtectThread()
{
    qDebug() << "VerifyASProtectThread destructor";
}

void VerifyASProtectThread::setFilesForVerify(QStringList *pFilesForVerify)
{
    filesForVerify = pFilesForVerify;
}

void VerifyASProtectThread::run()
{
    success = true;
    QStringList logStringList;

    foreach(const QString fileForVerify, *filesForVerify) {

        QString logString;

        bool ok = VerifyASProtect(fileForVerify, &logString);
        if(!ok) {
            success = false;
            logStringList.append(logString);
        }

    }

    if(success) {
        logStringList.append(QString("All files were protected."));
    }

    emit done(success, logStringList, this);
}


bool VerifyASProtectThread::VerifyASProtect(QString fileForVerify, QString *logString)
{
    bool success = false;

    HANDLE hFileMapping;
    LPVOID lpFileBase;
    PIMAGE_DOS_HEADER dosHeader;
    PIMAGE_NT_HEADERS peHeader;

    wchar_t lpFileName[_MAX_PATH];
    ZeroMemory(lpFileName, _MAX_PATH * sizeof(wchar_t));

    fileForVerify.toWCharArray(lpFileName);

    HANDLE hFile = CreateFileW(lpFileName,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        logString->append("Could not open file");
        return false;
    }

    hFileMapping = CreateFileMapping(hFile,
                                     nullptr,
                                     PAGE_READONLY,
                                     0,
                                     0,
                                     nullptr);

    if(hFileMapping == nullptr)
    {
        logString->append("Could not map file to memory");
        CloseHandle(hFile);
        return false;
    }

    lpFileBase = MapViewOfFile(hFileMapping,
                               FILE_MAP_READ,
                               0,
                               0,
                               0);

    if(lpFileBase == nullptr)
    {
        logString->append("MapViewOfFile failed");
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        return false;
    }

    dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(lpFileBase);

    if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
    {

        peHeader = reinterpret_cast<PIMAGE_NT_HEADERS>((u_char*)dosHeader+dosHeader->e_lfanew);

        if (peHeader->Signature == IMAGE_NT_SIGNATURE)
        {

            if(peHeader->OptionalHeader.AddressOfEntryPoint == 0x1000) {
                success = true;
            } else {
                logString->append("File was not protected");
            }

        } else {
            logString->append("File does not have a valid IMAGE_NT_SIGNATURE");
        }

    } else {
        logString->append("File does not have a valid IMAGE_DOS_SIGNATURE");
    }

    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);

    return success;
}
