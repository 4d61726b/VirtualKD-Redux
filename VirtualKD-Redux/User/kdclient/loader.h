/*! \file loader.h
    \author Ivan Shcherbakov (Bazis)
    $Id: loader.h,v 1.4 2009-10-07 19:09:52 Bazis Exp $
    \brief Contains an inline class for injecting DLLs into processes
*/

#pragma once

#include "rpcdispatch/status.h"
#include "hook64.h"
#include <BazisLib/bzscore/cmndef.h>
#include <BazisLib/bzscore/assert.h>

//! Allows injecting DLLs into remote processes
class RemoteDllLoader
{
private:
    TCHAR m_DllName[MAX_PATH];

private:
    wchar_t *GetDllName(bool ShortName)
    {
        wchar_t *pDllName = m_DllName;
        if (ShortName)
        {
            pDllName = wcsrchr(pDllName, '\\');
            if (pDllName)
                pDllName++;
            else
                pDllName = m_DllName;
        }
        return pDllName;
    }

public:
    //! Initializes the object and stores the path for the DLL to be loaded in remote processes
    RemoteDllLoader(HINSTANCE hDll, bool FreeLibraryAfterInit = true)
    {
        memset(m_DllName, 0, sizeof(m_DllName));
        unsigned cnt = GetModuleFileName(hDll, m_DllName, __countof(m_DllName));
        ASSERT(cnt);
        if (FreeLibraryAfterInit)
            FreeLibrary(hDll);
    }

    ~RemoteDllLoader()
    {
    }

    static ULONGLONG GetRemoteProcAddress(unsigned PID, TCHAR *pDllName, char *pFuncName)
    {
        ULONGLONG pFunc = GetRemoteModuleHandle64Aware(PID, pDllName);
        if (!pFunc)
            return 0;
        size_t off = (char *)GetProcAddress(GetModuleHandle(pDllName), pFuncName) - (char *)GetModuleHandle(pDllName);
        pFunc += off;
        return pFunc;
    }

    //! Starts remote DLL loading and returns the handle to the remote thread
    HANDLE InitiateDLLLoading(unsigned PID, DWORD *pThreadID = NULL)
    {
        ULONGLONG p = GetRemoteProcAddress(PID, L"kernel32.dll", "LoadLibraryW");
        if (!p)
            return INVALID_HANDLE_VALUE;
        HANDLE hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, PID);
        if (hProc == INVALID_HANDLE_VALUE)
            return INVALID_HANDLE_VALUE;
        LPVOID lp = VirtualAllocEx(hProc, 0, 4096, MEM_COMMIT, PAGE_READWRITE);
        if (!lp)
        {
            CloseHandle(hProc);
            return INVALID_HANDLE_VALUE;
        }
        SIZE_T dwOk = 0;
        BOOL b = WriteProcessMemory(hProc, lp, m_DllName, wcslen(m_DllName) * 2 + 2, &dwOk);
        if (!b)
        {
            CloseHandle(hProc);
            return INVALID_HANDLE_VALUE;
        }
        DWORD ID = 0;
        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)p, lp, 0, &ID);
        CloseHandle(hProc);
        if (pThreadID)
            *pThreadID = ID;
        return hThread;
    }

    //! Starts DLL unloading and returns a handle to the created remote thread
    HANDLE InitiateDLLUnloading(unsigned PID, bool UseShortName = true, DWORD *pThreadID = NULL)
    {
        ULONGLONG p = GetRemoteProcAddress(PID, L"kdclient.dll", "VMWareUnhookThread");
        if (!p)
            p = GetRemoteProcAddress(PID, L"kdclient64.dll", "VMWareUnhookThread");
        if (!p)
            return INVALID_HANDLE_VALUE;
        HANDLE hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, PID);
        if (hProc == INVALID_HANDLE_VALUE)
            return INVALID_HANDLE_VALUE;
        DWORD ID = 0;
        HINSTANCE hInst = FindLibraryInProcess(PID, UseShortName);
        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)p, (LPVOID)hInst, 0, &ID);
        CloseHandle(hProc);
        if (pThreadID)
            *pThreadID = ID;
        return hThread;
    }

    //! Finds a library among the libraries loaded into a remote process
    HINSTANCE FindLibraryInProcess(unsigned PID, bool UseShortName = true)
    {
        wchar_t *pDllName = GetDllName(UseShortName);
        //		char szName[MAX_PATH] = {0,};
        //		WideCharToMultiByte(CP_ACP, 0, pDllName, -1, szName, sizeof(szName), "?", NULL);
        return (HINSTANCE)GetRemoteModuleHandle64Aware(PID, pDllName, UseShortName);
    }
};