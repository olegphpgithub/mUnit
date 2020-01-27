#include "VerifyEmbeddedSignatureThread.h"

#include <QString>
#include <QtDebug>
#include <QFileInfo>

#include <Windows.h>
#include <VersionHelpers.h>
#include <strsafe.h>
#include <SoftPub.h>
#include <WinTrust.h>

// Link with the Wintrust.lib file.
#pragma comment (lib, "wintrust")


VerifyEmbeddedSignatureThread::VerifyEmbeddedSignatureThread(QObject *parent) : QThread(parent)
{
    qDebug() << "VerifyEmbeddedSignatureThread constructor";
}

VerifyEmbeddedSignatureThread::~VerifyEmbeddedSignatureThread()
{
    qDebug() << "VerifyEmbeddedSignatureThread destructor";
}

void VerifyEmbeddedSignatureThread::setFilesForVerify(QStringList *pFilesForVerify)
{
    filesForVerify = pFilesForVerify;
}

void VerifyEmbeddedSignatureThread::run()
{

    success = true;

    QStringList logStringList;

    foreach(const QString fileForVerify, *filesForVerify) {

        QString logString;

        bool ok = VerifyEmbeddedSignature(fileForVerify, &logString);
        if(!ok) {
            success = false;
            logStringList.append(logString);
        }

    }

    if(success) {
        logStringList.append(QString("All files were signed and the signatures were verified."));
    }

    emit done(success, logStringList, this);

}


bool VerifyEmbeddedSignatureThread::VerifyEmbeddedSignature(QString fileForVerify, QString *logString)
{

    bool success = false;
    LONG lStatus;
    DWORD dwLastError;

    WCHAR pwszSourceFile[MAX_PATH] = { 0 };
    fileForVerify.toWCharArray(pwszSourceFile);
    
    QFileInfo fileInfo(fileForVerify);
    *logString = fileInfo.baseName();
    QString message = QString("");

    // Initialize the WINTRUST_FILE_INFO structure.

    WINTRUST_FILE_INFO FileData;
    memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = pwszSourceFile;
    FileData.hFile = nullptr;
    FileData.pgKnownSubject = nullptr;

    /*
    WVTPolicyGUID specifies the policy to apply on the file
    WINTRUST_ACTION_GENERIC_VERIFY_V2 policy checks:

    1) The certificate used to sign the file chains up to a root
    certificate located in the trusted root certificate store. This
    implies that the identity of the publisher has been verified by
    a certification authority.

    2) In cases where user interface is displayed (which this example
    does not do), WinVerifyTrust will check for whether the
    end entity certificate is stored in the trusted publisher store,
    implying that the user trusts content from this publisher.

    3) The end entity certificate has sufficient permission to sign
    code, as indicated by the presence of a code signing EKU or no
    EKU.
    */

    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA WinTrustData;

    // Initialize the WinVerifyTrust input data structure.

    // Default all fields to 0.
    memset(&WinTrustData, 0, sizeof(WinTrustData));

    WinTrustData.cbStruct = sizeof(WinTrustData);

    // Use default code signing EKU.
    WinTrustData.pPolicyCallbackData = nullptr;

    // No data to pass to SIP.
    WinTrustData.pSIPClientData = nullptr;

    // Disable WVT UI.
    WinTrustData.dwUIChoice = WTD_UI_NONE;

    // No revocation checking.
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;

    // Verify an embedded signature on a file.
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;

    // Verify action.
    WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;

    // Verification sets this value.
    WinTrustData.hWVTStateData = nullptr;

    // Not used.
    WinTrustData.pwszURLReference = nullptr;

    // This is not applicable if there is no UI because it changes
    // the UI to accommodate running applications instead of
    // installing applications.
    WinTrustData.dwUIContext = 0;

    // Set pFile.
    WinTrustData.pFile = &FileData;

    bool canUseSignatureSettings = IsWindowsVersionOrGreater(6, 2, 0);

    // N.B. Use of this member is only supported on Windows 8 and Windows Server 2012 (and later)
    WINTRUST_SIGNATURE_SETTINGS signatureSettings;
    // Setup WINTRUST_SIGNATURE_SETTINGS to get the number of signatures in the file
    signatureSettings.cbStruct = sizeof(WINTRUST_SIGNATURE_SETTINGS);
    signatureSettings.dwIndex = 0;
    signatureSettings.dwFlags = WSS_GET_SECONDARY_SIG_COUNT;
    signatureSettings.cSecondarySigs = 0;
    signatureSettings.dwVerifiedSigIndex = 0;
    signatureSettings.pCryptoPolicy = nullptr;

    if(canUseSignatureSettings) {
        WinTrustData.pSignatureSettings = &signatureSettings;
    }

    // WinVerifyTrust verifies signatures as specified by the GUID
    // and Wintrust_Data.
    lStatus = WinVerifyTrust(
        nullptr,
        &WVTPolicyGUID,
        &WinTrustData
    );

    switch (lStatus)
    {
    case ERROR_SUCCESS:
        /*
        Signed file:
        - Hash that represents the subject is trusted.

        - Trusted publisher without any verification errors.

        - UI was disabled in dwUIChoice. No publisher or
        time stamp chain errors.

        - UI was enabled in dwUIChoice and the user clicked
        "Yes" when asked to install and run the signed
        subject.
        */

        if(signatureSettings.cSecondarySigs > 0
                || !canUseSignatureSettings
        )
        {

            success = true;

            message = QString(
                "The file was signed "
                "and the signature was verified."
            );

        } else {

            success = false;

            message = QString("The file does not have a secondary signature.");

        }

        break;

    case TRUST_E_NOSIGNATURE:
        // The file was not signed or had a signature
        // that was not valid.

        // Get the reason for no signature.
        dwLastError = GetLastError();
        if (TRUST_E_NOSIGNATURE == dwLastError ||
            TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError ||
            TRUST_E_PROVIDER_UNKNOWN == dwLastError)
        {
            // The file was not signed.

            message = QString(
                        "The file was not signed."
                        );

        }
        else
        {

            // The signature was not valid or there was an error
            // opening the file.

            message = QString(
                "An unknown error occurred while trying to "
                "verify the signature of the file.");

        }

        break;

    case TRUST_E_EXPLICIT_DISTRUST:
        // The hash that represents the subject or the publisher
        // is not allowed by the admin or user.

        message = QString(
                    "The signature is present, but specifically "
                    "disallowed.");
        break;

    case TRUST_E_SUBJECT_NOT_TRUSTED:

        // The user clicked "No" when asked to install and run.

        message = QString(
                    "The signature in file is present, but not "
                    "trusted."
                    );

        break;

    case CRYPT_E_SECURITY_SETTINGS:
        /*
        The hash that represents the subject or the publisher
        was not explicitly trusted by the admin and the
        admin policy has disabled user trust. No signature,
        publisher or time stamp errors.
        */

        message = QString(
                    "The hash "
                    "representing the subject or the publisher wasn't "
                    "explicitly trusted by the admin and admin policy "
                    "has disabled user trust. No signature, publisher "
                    "or timestamp errors."
                    );

        break;

    default:

        // The UI was disabled in dwUIChoice or the admin policy
        // has disabled user trust. lStatus contains the
        // publisher or time stamp chain error.

        WCHAR pwszErrorMessage[256];
        StringCbPrintfW(pwszErrorMessage, 256, L"Error while verifying the signature: 0x%x.", lStatus);

        message = QString::fromWCharArray(pwszErrorMessage);

        break;
    }

    logString->append(" - ");
    logString->append(message);

    return success;
}
