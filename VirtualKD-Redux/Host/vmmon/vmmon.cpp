/*! \file vmmon.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: vmmon.cpp,v 1.5 2009-12-31 13:06:42 Bazis Exp $
    \brief Contains entry point for VMMON application
*/

#include "stdafx.h"

#include "resource.h"

#include "MainDlg.h"
#include <BazisLib/bzscore/Win32/memdbg.h>
#include <BazisLib/bzshlp/Win32/RegistrySerializer.h>

#include "regconfig.h"
#include "vkdversion.h"

CAppModule _Module;

// Writes the initial configuration if needed. We have to do it here since we are elevated.
// The process with kdclient loaded in it might not have the permissions to write.
static void CreateRegConfig()
{
    KdClientParams kcParams;
    BazisLib::Win32::RegistryKey regkeyConfig(HKEY_LOCAL_MACHINE, VKD_REGISTRY_CONFIG_PATH);
    BazisLib::Win32::RegistrySerializer::Deserialize(regkeyConfig, kcParams);
    BazisLib::Win32::RegistrySerializer::Serialize(regkeyConfig, kcParams);

    PatchingParams patchParams;
    BazisLib::Win32::RegistryKey regkeyPatcher(HKEY_LOCAL_MACHINE, VKD_REGISTRY_PATCHER_PATH);
    BazisLib::Win32::RegistrySerializer::Deserialize(regkeyPatcher, patchParams);
    BazisLib::Win32::RegistrySerializer::Serialize(regkeyPatcher, patchParams);

    TraceAssistParams taParams;
    BazisLib::Win32::RegistryKey regkeyTraceAssist(HKEY_LOCAL_MACHINE, VKD_REGISTRY_TRACE_ASSIST_PATH);
    BazisLib::Win32::RegistrySerializer::Deserialize(regkeyTraceAssist, taParams);
    BazisLib::Win32::RegistrySerializer::Serialize(regkeyTraceAssist, taParams);

    MonitorParams monParams;
    BazisLib::Win32::RegistryKey regkeyMonitor(HKEY_LOCAL_MACHINE, VKD_REGISTRY_MONITOR_PATH);
    BazisLib::Win32::RegistrySerializer::Deserialize(regkeyMonitor, monParams);
    BazisLib::Win32::RegistrySerializer::Serialize(regkeyMonitor, monParams);
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
    LPCWSTR szMutexName = L"Global\\virtualkd_redux_vmmon";
    HANDLE hMutex = CreateMutexW(NULL, TRUE, szMutexName);
    if (hMutex)
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            MessageBoxW(NULL, L"VirtualKD-Redux Virtual Machine Monitor is already running.", L"Error", MB_ICONERROR);
            return 1;
        }
    }

    CreateRegConfig();

    BazisLib::MemoryLeakDetector leakDetector;
    UNREFERENCED_PARAMETER(leakDetector);
    //HRESULT hRes = ::CoInitialize(NULL);
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

    hRes = _Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    int nRet = 0;
    // BLOCK: Run application
    {
        CMainDlg dlgMain;
        nRet = (int)dlgMain.DoModal();
    }

    _Module.Term();
    ::CoUninitialize();

    //C_ASSERT(sizeof(void *) == sizeof(CMainDlg::OnAppAbout));
    if (hMutex)
    {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }

    return nRet;
}
