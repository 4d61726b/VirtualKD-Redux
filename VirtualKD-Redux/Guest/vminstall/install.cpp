#include "stdafx.h"

#include <Windows.h>

#include <BazisLib/bzscore/file.h>
#include <BazisLib/bzshlp/Win32/BCD.h>
#include <BazisLib/bzscore/strfast.h>
#include <BazisLib/bzshlp/Win32/wow64.h>
#include <BazisLib/bzscore/Win32/registry.h>

#include "resource.h"
#include "resource2.h"
#include "bootedit.h"
#include "install.h"
#include "TakeOwnership.h"

using namespace BazisLib;
using namespace BazisLib::Win32::BCD;
using namespace BootEditor;

ActionStatus FindBestOSEntry(ManagedPointer<AIBootConfigurationEntry> *ppEntry, bool *pbAlreadyInstalled, ManagedPointer<AIBootConfigurationEditor> pEditor)
{
    ASSERT(ppEntry);

    *ppEntry = NULL;

    if (!pEditor)
        pEditor = CreateConfigurationEditor();
    if (!pEditor)
        return MAKE_STATUS(UnknownError);

    ActionStatus st = pEditor->StartEnumeration();
    if (!st.Successful())
        return st;

    if (pbAlreadyInstalled)
        *pbAlreadyInstalled = false;

    ManagedPointer<AIBootConfigurationEntry> pEntry, pCurrentOSEntry;
    while (pEntry = pEditor->GetNextEntry())
    {
        if (pEntry->IsCurrentOS())
        {
            if (!pCurrentOSEntry)
                pCurrentOSEntry = pEntry;

            if (pEntry->GetDebuggerMode() == kdCustom)
            {
                WCHAR wcsBuf[MAX_PATH];
                String strFullPath;
                DWORD dwVerSize;
                DWORD dwVerSizeHandle = 0;
                String strData;
                UINT uiValLen = 0;
                WCHAR* wcsProductName;

                if (GetSystemDirectoryW(wcsBuf, _countof(wcsBuf)) == FALSE)
                    continue;
                
                strFullPath = Path::Combine(wcsBuf, pEntry->GetCustomKDName());
                dwVerSize = GetFileVersionInfoSizeW(strFullPath.c_str(), &dwVerSizeHandle);

                if (!dwVerSize)
                    continue;

                strData.resize(dwVerSize, L'\0');

                if (GetFileVersionInfoW(strFullPath.c_str(), 0, dwVerSize, (LPVOID)strData.data()) == FALSE)
                    continue;

                if (VerQueryValueW(strData.data(), L"\\StringFileInfo\\000904B0\\ProductName", (LPVOID*)&wcsProductName, &uiValLen) == FALSE)
                    continue;

                if (wcscmp(wcsProductName, L"VirtualKD-Redux") != 0)
                    continue;

                pCurrentOSEntry = pEntry;
                if (pbAlreadyInstalled)
                    *pbAlreadyInstalled = true;
                break;
            }
        }
    }

    *ppEntry = pCurrentOSEntry;

    if (!pCurrentOSEntry)
        return MAKE_STATUS(NotSupported);
    return MAKE_STATUS(Success);
}

ActionStatus CreateVirtualKDBootEntry(bool CreateNewEntry, bool SetNewEntryDefault, LPCWSTR lpEntryName, unsigned Timeout, bool replacingKdcom)
{
    ManagedPointer<AIBootConfigurationEditor> pEditor = CreateConfigurationEditor();
    if (!pEditor)
        return MAKE_STATUS(UnknownError);

    bool alreadyInstalled = false;
    ManagedPointer<AIBootConfigurationEntry> pEntry;

    ActionStatus st = FindBestOSEntry(&pEntry, &alreadyInstalled, pEditor);
    if (!st.Successful())
        return st;

    if (!pEntry)
        return MAKE_STATUS(NotSupported);

    if (CreateNewEntry && !alreadyInstalled)
    {
        pEntry->ExplicitlyDisableDebugging();
        pEntry = pEditor->CopyEntry(pEntry, true, &st);
    }

    if (!pEntry)
        return MAKE_STATUS(NotSupported);

    st = pEntry->EnableCustomKD(replacingKdcom ? L"kdcom.dll" : L"kdbazis.dll");
    if (!st.Successful())
        return COPY_STATUS(st);

    if (!st.Successful())
        return COPY_STATUS(st);
    if (!pEntry)
        return MAKE_STATUS(UnknownError);

    if (lpEntryName)
    {
        st = pEntry->SetDescription(lpEntryName);
        if (!st.Successful())
            return COPY_STATUS(st);
    }

    if (SetNewEntryDefault)
    {
        st = pEditor->SetDefaultEntry(pEntry);
        if (!st.Successful())
            return COPY_STATUS(st);
    }

    if (Timeout != -1)
    {
        st = pEditor->SetTimeout(Timeout);
        if (!st.Successful())
            return COPY_STATUS(st);
    }

    return pEditor->FinishEditing();
}

static ActionStatus SaveResourceToFile(const String& filePath, LPCTSTR lpResType, DWORD dwID)
{
    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(dwID), lpResType);
    if (!hRes)
        return MAKE_STATUS(ActionStatus::FailFromLastError());

    DWORD len = SizeofResource(hModule, hRes);
    if (!len)
        return MAKE_STATUS(ActionStatus::FailFromLastError());

    HGLOBAL hResObj = LoadResource(hModule, hRes);
    if (!hResObj)
        return MAKE_STATUS(ActionStatus::FailFromLastError());

    PVOID p = LockResource(hResObj);
    if (!p)
        return MAKE_STATUS(ActionStatus::FailFromLastError());

    ActionStatus st;
    File file(filePath, FileModes::CreateOrTruncateRW, &st);
    if (!st.Successful())
        return st;
    file.Write(p, len, &st);
    if (!st.Successful())
        return st;
    return MAKE_STATUS(Success);
}

LRESULT DeployKdCom(bool replaceKdcom, String monitorLocation)
{
    String fp = Path::Combine(Path::GetSpecialDirectoryLocation(dirSystem), replaceKdcom ? ConstString(L"kdcom.dll") : ConstString(L"kdbazis.dll"));

#ifdef _WIN64
    bool is64Bit = true;
#else
    bool is64Bit = BazisLib::WOW64APIProvider::sIsWow64Process();
#endif

    ActionStatus st;

    {
        if (replaceKdcom)
        {
            String kdcomBackup = Path::Combine(Path::GetSpecialDirectoryLocation(dirSystem), L"kdcom_old.dll");

            for (DWORD i = 2; File::Exists(kdcomBackup); ++i)
            {
                String kdcomBackupCurrentName;
                kdcomBackupCurrentName.AppendFormat(L"kdcom_old_%u.dll", i);
                kdcomBackup = Path::Combine(Path::GetSpecialDirectoryLocation(dirSystem), kdcomBackupCurrentName);
            }

            st = TakeOwnership(const_cast<LPTSTR>(String(fp).c_str()));
            if (!st.Successful())
            {
                ::MessageBox(HWND_DESKTOP,
                    String::sFormat(_T("Cannot replace owner on kdcom.dll: %s"), st.GetMostInformativeText().c_str()).c_str(),
                    NULL,
                    MB_ICONERROR);
                return 0;
            }

            Win32::Security::TranslatedAcl dacl = File::GetDACLForPath(fp, &st);
            if (!st.Successful())
            {
                ::MessageBox(HWND_DESKTOP,
                    String::sFormat(_T("Cannot query permissions on kdcom.dll: %s"), st.GetMostInformativeText().c_str()).c_str(),
                    NULL,
                    MB_ICONERROR);
                return 0;
            }
            dacl.AddAllowingAce(STANDARD_RIGHTS_ALL | SPECIFIC_RIGHTS_ALL, BazisLib::Win32::Security::Sid::CurrentUserSid());
            st = File::SetDACLForPath(fp, dacl);
            if (!st.Successful())
            {
                ::MessageBox(HWND_DESKTOP,
                    String::sFormat(_T("Cannot set permissions on kdcom.dll: %s"), st.GetMostInformativeText().c_str()).c_str(),
                    NULL,
                    MB_ICONERROR);
                return 0;
            }

            if (!MoveFile(fp.c_str(), kdcomBackup.c_str()))
            {
                ::MessageBox(HWND_DESKTOP,
                    String::sFormat(_T("Cannot rename old kdcom.dll: %s"), MAKE_STATUS(ActionStatus::FromLastError()).GetMostInformativeText().c_str()).c_str(),
                    NULL,
                    MB_ICONERROR);
                return 0;
            }
        }
        st = SaveResourceToFile(fp, _T("KDVMDLL"), IDR_KDVM);
    }
    if (!st.Successful())
    {
        ::MessageBox(HWND_DESKTOP,
            String::sFormat(_T("Cannot create KDBAZIS.DLL: %s"), st.GetMostInformativeText().c_str()).c_str(),
            NULL,
            MB_ICONERROR);
        return 0;
    }

    if (!monitorLocation.empty())
    {
        HWND hMonitor = FindWindow(NULL, _T("DDKLaunchMonitor"));
        if (hMonitor)
        {
            ::SendMessage(hMonitor, WM_CLOSE, 0, 0);
            Sleep(1000);
        }

        fp = monitorLocation;
        fp = Path::Combine(fp, _T("DDKLaunchMonitor.exe"));

        WOW64FSRedirHolder holder;
        st = SaveResourceToFile(fp, _T("VDDKMON"), is64Bit ? IDR_VDDKMON64 : IDR_VDDKMON32);

        RegistryKey key(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));
        key[_T("DDKLaunchMonitor")] = fp.c_str();

        if (!st.Successful())
        {
            ::MessageBox(HWND_DESKTOP,
                String::sFormat(_T("Cannot create DDKLaunchMonitor.exe: %s"), st.GetMostInformativeText().c_str()).c_str(),
                NULL,
                MB_ICONERROR);
            return 0;
        }
    }

    return 1;
}

static OSVERSIONINFOEXW GetOSVersion()
{
    typedef void (WINAPI* RtlGetVersion)(OSVERSIONINFOEXW*);
    OSVERSIONINFOEXW info = { 0 };
    info.dwOSVersionInfoSize = sizeof(info);
    ((RtlGetVersion)GetProcAddress(GetModuleHandleW(L"ntdll"), "RtlGetVersion"))(&info);
    return info;
}

bool IsVistaOrLater()
{
    OSVERSIONINFOEXW info = GetOSVersion();
    return info.dwMajorVersion >= 6;
}

bool IsWin8OrLater()
{
    OSVERSIONINFOEXW info = GetOSVersion();
    if (info.dwMajorVersion > 6)
    {
        return true;
    }
    else if (info.dwMajorVersion == 6)
    {
        return info.dwMinorVersion >= 2;
    }

    return false;
}

bool IsWin10OrLater()
{
    OSVERSIONINFOEXW info = GetOSVersion();
    return info.dwMajorVersion >= 10;
}

bool IsWin11OrLater()
{
    OSVERSIONINFOEXW info = GetOSVersion();

    if (info.dwMajorVersion > 10)
    {
        return true;
    }
    else if (info.dwMajorVersion < 10)
    {
        return false;
    }

    return info.dwBuildNumber >= 22000;
}

bool IsReactOS()
{
    return !IsVistaOrLater() && BazisLib::Win32::File::Exists(L"c:\\freeldr.ini") && !BazisLib::Win32::File::Exists(L"c:\\boot.ini");
}

bool IsRunningVMWareHypervisor()
{
    RegistryKey key(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS");
    if (!key.Valid())
    {
        return false;
    }

    String strBIOSVersion;
    if (!key[L"BIOSVersion"].ReadValue(&strBIOSVersion).Successful())
    {
        return false;
    }

    return strBIOSVersion.compare(0, _countof(L"VMW") - 1, L"VMW") == 0;
}

bool IsWinloadPatchApplicable(bool* pbRecommendWinloadPatch)
{
    *pbRecommendWinloadPatch = false;

#ifdef _WIN64
    if (!IsWin11OrLater())
    {
        return false;
    }

    *pbRecommendWinloadPatch = !IsRunningVMWareHypervisor();

    return true;
#else
    return false;
#endif
}

bool IsLegacyBoot()
{
    OSVERSIONINFOEXW info = GetOSVersion();

    if (info.dwMajorVersion < 6 ||
        (info.dwMajorVersion == 6 && info.dwMinorVersion == 0 && info.wServicePackMajor == 0))
    {
        return true;
    }

    WCHAR wcFirmwareType[_countof("UEFI")];
    if (!GetEnvironmentVariableW(L"%FIRMWARE_TYPE%", wcFirmwareType, sizeof(wcFirmwareType)))
    {
        return false;
    }

    return !!wcscmp(wcFirmwareType, L"UEFI");
}

bool IsSecureBootEnabled()
{
    if (!IsWin8OrLater())
    {
        return false;
    }

    RegistryKey key(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\SecureBoot\\State");
    unsigned int dwUEFISecureBootEnabled = 0;
    if (!key.Valid())
    {
        return false;
    }

    if (!key[L"UEFISecureBootEnabled"].ReadValue(&dwUEFISecureBootEnabled).Successful())
    {
        return false;
    }

    return !!dwUEFISecureBootEnabled;
}
