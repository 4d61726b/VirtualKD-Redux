/*! \file
    \author Ivan Shcherbakov (Bazis)
    $Id: 32to64.cpp,v 1.3 2009-12-31 13:06:41 Bazis Exp $
    \brief Contains implementation for \ref vm32to64 functions
*/

#include "stdafx.h"
#include "32to64.h"
#include <BazisLib/bzscore/cmndef.h>
#include <BazisLib/bzshlp/Win32/wow64.h>
#include <shellapi.h>
#include <BazisLib/bzscore/file.h>
#include <BazisLib/bzshlp/Win32/process.h>
#include <BazisLib/bzscore/datetime.h>

#include "patchapi.h"
#include "vkdversion.h"
#include "rpcdispatch/permdesc.h"

extern HMODULE g_hThisDll;

static void ProcessRemoteCall(LPCTSTR lpCall, struct PipeMessage *pResult, unsigned *pCallerPID = NULL);

using namespace BazisLib;

struct PipeMessage
{
    ULONGLONG LongResult;
    wchar_t wszStringResult[256];
};

static const TCHAR tszPipeFmt[] = _T("\\\\.\\pipe\\KDCLIENT_32TO64PIPE_%X");

static const TCHAR tszVVMONCommandPipe32[] = _T("\\\\.\\pipe\\VMMON_Control32");
static const TCHAR tszVVMONCommandPipe64[] = _T("\\\\.\\pipe\\VMMON_Control64");

class RemoteCallServerContext
{
public:
    HANDLE m_hPipe;

    RemoteCallServerContext()
        : m_hPipe(INVALID_HANDLE_VALUE)
    {
        CreateAnotherInstance();
    }

    ~RemoteCallServerContext()
    {
        if (m_hPipe != INVALID_HANDLE_VALUE)
            CloseHandle(m_hPipe);
    }

    void CreateAnotherInstance()
    {
        HANDLE hOldPipe = m_hPipe;
#ifdef _WIN64
        const TCHAR *pPipeName = tszVVMONCommandPipe64;
#else
        const TCHAR *pPipeName = tszVVMONCommandPipe32;
#endif
        PermissiveSecurityDescriptor desc;
        m_hPipe = CreateNamedPipe(pPipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE, PIPE_UNLIMITED_INSTANCES, 32768, 32768, 0, desc);

        if (hOldPipe != INVALID_HANDLE_VALUE)
            CloseHandle(hOldPipe);
    }

};

__declspec(dllexport) PVOID StartRemoteCallServer()
{
    return new RemoteCallServerContext();
}

__declspec(dllexport) void EndRemoteCallServer(PVOID pContext)
{
    delete ((RemoteCallServerContext *)pContext);
}

__declspec(dllexport) void ProcessRemoteCallServerMessages(PVOID pContext)
{
    if (!pContext)
        return;
    RemoteCallServerContext *pSrv = (RemoteCallServerContext *)pContext;

    DWORD dwAvail = 0;
    if (!PeekNamedPipe(pSrv->m_hPipe, NULL, 0, NULL, &dwAvail, NULL))
    {
        if (GetLastError() == ERROR_BROKEN_PIPE)
            pSrv->CreateAnotherInstance();

        return;
    }
    if (!dwAvail)
        return;

    String request;
    DWORD dwDone = 0;
    ReadFile(pSrv->m_hPipe, request.PreAllocate(dwAvail, false), dwAvail, &dwDone, NULL);
    request.SetLength(dwDone);
    if (dwDone != dwAvail)
        return;

    PipeMessage msg = { 0, };
    ProcessRemoteCall(request.c_str(), &msg);

    WriteFile(pSrv->m_hPipe, &msg, sizeof(msg), &dwDone, NULL);
    pSrv->CreateAnotherInstance();
}


#ifndef _WIN64

static bool s_bUserWarnedAboutMissing64bitDll = false;

static ActionStatus TryCallOutprocClientUsingVMMON(KDClientCommand cmd, unsigned PID, LPWSTR lpStringResult, size_t StringSize, ULONGLONG *pResult)
{
    unsigned pidCurrent = GetCurrentProcessId();
    String virtualCmdLine;
    virtualCmdLine.Format(_T("rundll32 KdClient32To64Entry %d %d %d"), pidCurrent, cmd, PID);

    PipeMessage msg = { 0, };
    DWORD dwDone = 0;

    if (lpStringResult)
        lpStringResult[0] = 0;

    if (!CallNamedPipe(WOW64APIProvider::sIsWow64Process() ? tszVVMONCommandPipe64 : tszVVMONCommandPipe32,
        (LPVOID)virtualCmdLine.c_str(),
        virtualCmdLine.SizeInBytes(true),
        &msg,
        sizeof(msg),
        &dwDone,
        1000))
        return MAKE_STATUS(ActionStatus::FailFromLastError());

    if (dwDone != sizeof(msg))
        return MAKE_STATUS(UnknownError);

    if (lpStringResult)
        wcsncpy(lpStringResult, msg.wszStringResult, min(StringSize, __countof(msg.wszStringResult)));

    if (pResult)
        *pResult = msg.LongResult;

    return MAKE_STATUS(Success);
}

static ActionStatus TryCall64BitClientUsingRundll(KDClientCommand cmd, unsigned PID, LPWSTR lpStringResult, size_t StringSize, ULONGLONG *pResult)
{
    *pResult = -1;
    if (lpStringResult)
        lpStringResult[0] = 0;

    TCHAR tszModule[MAX_PATH + 6] = { 0, };
    GetModuleFileName(g_hThisDll, tszModule, __countof(tszModule));
    TCHAR *p = _tcsrchr(tszModule, '.');
    if (!p)
        return MAKE_STATUS(UnknownError);
    _tcsncpy(p, _T("64.dll"), __countof(tszModule) - (p - tszModule));
    if (!File::Exists(tszModule))
    {
        if (!s_bUserWarnedAboutMissing64bitDll)
        {
            TCHAR tszMsg[MAX_PATH + 100] = { 0, };
            _sntprintf(tszMsg, __countof(tszMsg), _T("Cannot patch 64-bit VM!\r\n%s not found.\r\nInstall the DLL and retry patching."), tszModule);
            s_bUserWarnedAboutMissing64bitDll = true;
            MessageBox(0, tszMsg, 0, MB_ICONERROR | MB_TASKMODAL);
        }
        return MAKE_STATUS(ActionStatus::FailFromLastError());
    }
    s_bUserWarnedAboutMissing64bitDll = false;


    TCHAR tszCmdLine[MAX_PATH + 100];
    unsigned pidCurrent = GetCurrentProcessId();

    _sntprintf(tszCmdLine, __countof(tszCmdLine), tszPipeFmt, pidCurrent);
    HANDLE hPipe = CreateNamedPipe(tszCmdLine, PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE, 1, 0, 0, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE)
        return MAKE_STATUS(ActionStatus::FailFromLastError());

    _sntprintf(tszCmdLine, __countof(tszCmdLine), _T("rundll32 \"%s\",KdClient32To64Entry %d %d %d"), tszModule, pidCurrent, cmd, PID);
    STARTUPINFO SI = { 0, };
    SI.cb = sizeof(SI);
    PROCESS_INFORMATION PI = { 0, };

    BOOL bCreated = FALSE;
    {
        BazisLib::WOW64FSRedirHolder holder;
        bCreated = CreateProcess(NULL, tszCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &SI, &PI);
    }

    if (!bCreated)
    {
        int err = GetLastError();
        if (hPipe != INVALID_HANDLE_VALUE)
            CloseHandle(hPipe);
        if (err == ERROR_SUCCESS)
            return MAKE_STATUS(UnknownError);
        else
            return MAKE_STATUS(ActionStatus::FromWin32Error(err));
    }

    OVERLAPPED pipeOverlapped = { 0, };
    pipeOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ConnectNamedPipe(hPipe, &pipeOverlapped);

    HANDLE hhWait[2] = { pipeOverlapped.hEvent, PI.hProcess };
    PipeMessage msg = { 0, };

    if ((GetLastError() == ERROR_PIPE_CONNECTED) || (WaitForMultipleObjects(2, hhWait, FALSE, INFINITE) == WAIT_OBJECT_0))
    {
        DWORD dwDone = 0;
        if (ReadFile(hPipe, &msg, sizeof(msg), &dwDone, NULL) || WaitForMultipleObjects(2, hhWait, FALSE, INFINITE) == WAIT_OBJECT_0)
        {
            if (!dwDone)
                GetOverlappedResult(hPipe, &pipeOverlapped, &dwDone, TRUE);
            if (dwDone != sizeof(msg))
                memset(&msg, 0, sizeof(msg));
        }
    }
    msg.wszStringResult[__countof(msg.wszStringResult) - 1] = 0;

    if (lpStringResult)
        wcsncpy(lpStringResult, msg.wszStringResult, min(StringSize, __countof(msg.wszStringResult)));

    CloseHandle(hPipe);
    WaitForSingleObject(PI.hProcess, INFINITE);
    CloseHandle(PI.hProcess);
    CloseHandle(PI.hThread);
    *pResult = msg.LongResult;

    return MAKE_STATUS(Success);
}

ActionStatus TryLaunchVVMON(const String &exePath, bool WaitForPipeAvailable = true)
{
    const int kVMMONStartTimeout = 10000;

    ActionStatus st;
    BazisLib::Win32::Process vmmon(exePath.c_str(), true, &st);
    if (!st.Successful())
        return st;

    if (!WaitForPipeAvailable)
        return MAKE_STATUS(Success);

    DateTime start = DateTime::Now();
    for (;;)
    {
        Sleep(100);
        if ((DateTime::Now() - start).GetTotalMilliseconds() >= kVMMONStartTimeout)
            return MAKE_STATUS(UnknownError);
        ULONGLONG result = 0;
        if (TryCallOutprocClientUsingVMMON(kQueryVersion, 0, NULL, 0, &result).Successful())
            break;
        if (!vmmon.IsRunning())
            return MAKE_STATUS(UnknownError);
    }
    return MAKE_STATUS(Success);
}

static bool s_bDoNotSuggestLaunchingVMMON = false;

ULONGLONG Call64BitKDCLIENT(KDClientCommand cmd, unsigned PID, LPWSTR lpStringResult, size_t StringSize)
{
    ULONGLONG result = 0;
    if (TryCallOutprocClientUsingVMMON(cmd, PID, lpStringResult, StringSize, &result).Successful())
        return result;
    if (TryCall64BitClientUsingRundll(cmd, PID, lpStringResult, StringSize, &result).Successful())
        return result;
    for (;;)
    {
        if (s_bDoNotSuggestLaunchingVMMON)
            return -1;
        s_bDoNotSuggestLaunchingVMMON = true;

        TCHAR tszModule[MAX_PATH] = { 0, };
        GetModuleFileName(g_hThisDll, tszModule, __countof(tszModule));
        String vmmonExe = Path::Combine(Path::GetDirectoryName(TempStrPointerWrapper(tszModule)), WOW64APIProvider::sIsWow64Process() ? _T("vmmon64.exe") : _T("vmmon.exe"));
        if (File::Exists(vmmonExe))
        {
            switch (MessageBox(HWND_DESKTOP, _T("Failed to query VM information. This is most likely caused by insufficient privilege level. Do you want to start Virtual Machine monitor (VMMON) in Administrator mode to fix this problem?"), _T("VirtualKD-Redux"), MB_ICONQUESTION | MB_TOPMOST | MB_YESNO))
            {
            case IDYES:
                break;
            case IDNO:
                return -1;
            }
        }
        else
        {
            MessageBox(HWND_DESKTOP, _T("Failed to query VM information. This is most likely caused by insufficient privilege level. Running VMMON/VMMON64 will solve the problem."), _T("VirtualKD-Redux"), MB_ICONERROR | MB_TOPMOST | MB_OK);
            return -1;
        }

        ActionStatus st = TryLaunchVVMON(vmmonExe);
        if (!st.Successful())
        {
            MessageBox(HWND_DESKTOP, st.GetMostInformativeText().c_str(), _T("Failed to launch VMMON"), MB_ICONERROR | MB_TOPMOST | MB_OK);
            return -1;
        }
        s_bDoNotSuggestLaunchingVMMON = false;
        if (TryCallOutprocClientUsingVMMON(cmd, PID, lpStringResult, StringSize, &result).Successful())
            return result;
    }
    return result;
}

#endif

HANDLE StartPatcherThread(unsigned PID, DWORD *pPatcherThreadID = NULL);
HANDLE StartUnpatcherThread(unsigned PID, DWORD *pPatcherThreadID = NULL);

ULONGLONG KdClient32To64Dispatch(KDClientCommand cmd, unsigned PID, LPWSTR lpStringResult, size_t StringSize)
{
    DWORD ID = 0;
    HANDLE hThread;

    switch (cmd)
    {
    case kGetVMSessionName:
        return GetVMSessionNameW(PID, lpStringResult, StringSize);
    case kIsSessionPatched:
        return IsVMSessionPatched(PID);
    case kStartVMSessionPatching:
    case kStartVMSessionUnpatching:
        hThread = (cmd == kStartVMSessionPatching) ? StartPatcherThread(PID, &ID) : StartUnpatcherThread(PID, &ID);
        if (hThread == NULL)
            return 0;	//Already done
        else if (hThread == INVALID_HANDLE_VALUE)
            return -1LL;
        else
        {
            CloseHandle(hThread);
            return ID;
        }
        return -1LL;
    case kPatchAndWait:
        return PatchVMSessionIfNeeded(PID);
    case kUnpatchAndWait:
        return UnpatchVMSessionIfNeeded(PID);
    case kQueryVersion:
        return VIRTUALKD_VER_INT;
    default:
        return -1LL;
    }
}

static void ProcessRemoteCall(LPCTSTR lpCall, PipeMessage *pResult, unsigned *pCallerPID)
{
    int argc = 0;
    LPWSTR *pArgv = CommandLineToArgvW(lpCall, &argc);
    if (!pArgv)
        return;

    if (argc < 5)
    {
        LocalFree(pArgv);
        return;
    }

    int CallerPID = _wtoi(pArgv[2]);
    int Cmd = _wtoi(pArgv[3]);
    int TargetPID = _wtoi(pArgv[4]);

    LocalFree(pArgv);
    pArgv = NULL;

    if (pCallerPID)
        *pCallerPID = CallerPID;

    pResult->LongResult = KdClient32To64Dispatch((KDClientCommand)Cmd, TargetPID, pResult->wszStringResult, __countof(pResult->wszStringResult));
}

#ifdef _WIN64

void KdClient32To64Entry()
{
    TCHAR *pCmdLine = GetCommandLine();

    PipeMessage msg = { 0, };
    unsigned CallerPID = 0;

    ProcessRemoteCall(pCmdLine, &msg, &CallerPID);

    TCHAR tszPipeName[MAX_PATH];
    unsigned pidCurrent = GetCurrentProcessId();

    _sntprintf(tszPipeName, __countof(tszPipeName), tszPipeFmt, CallerPID);
    HANDLE hPipe = CreateFile(tszPipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hPipe == INVALID_HANDLE_VALUE)
        return;

    DWORD dwDone = 0;
    WriteFile(hPipe, &msg, sizeof(msg), &dwDone, NULL);
    CloseHandle(hPipe);
}

#endif
