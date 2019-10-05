/*! \file findproc.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: findproc.cpp,v 1.3 2009-12-31 13:06:41 Bazis Exp $
    \brief Contains functions for enumerating running processes
*/

#include "stdafx.h"
#include <tlhelp32.h>
#include "findproc.h"

#ifdef UNDER_CE
#pragma comment(lib, "toolhelp")
#else
#define CloseToolhelp32Snapshot CloseHandle
#endif

int FindProcessByNames(HANDLE hSnapshot, TCHAR **pszNames, unsigned NameCount, bool RestartSearch, unsigned *pMatchedIndex)
{
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;
    int PID = 0;
    PROCESSENTRY32 pe = { 0, };
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (RestartSearch)
        Process32First(hSnapshot, &pe);
    else
        Process32Next(hSnapshot, &pe);
    do
    {
        for (unsigned i = 0; i < NameCount; i++)
        {
            if (!_tcsicmp(pe.szExeFile, pszNames[i]))
            {
                PID = pe.th32ProcessID;
                if (pMatchedIndex)
                    *pMatchedIndex = i;
                return PID;
            }
        }
    } while (Process32Next(hSnapshot, &pe));
    return PID;
}

int FindProcessByName(HANDLE hSnapshot, TCHAR *pszName, bool RestartSearch)
{
    return FindProcessByNames(hSnapshot, &pszName, 1, RestartSearch, NULL);
}


int FindProcessByName(TCHAR *pszName)
{
    HANDLE hShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    int PID = FindProcessByName(hShot, pszName, true);
    CloseToolhelp32Snapshot(hShot);
    return PID;
}

LPVOID GetModuleFromProcess(int PID, TCHAR *pszModuleName)
{
    HANDLE hShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
    if (hShot == INVALID_HANDLE_VALUE)
        return NULL;
    LPVOID pMod = 0;

    MODULEENTRY32 me;
    me.dwSize = sizeof(me);
    Module32First(hShot, &me);
    bool FoundModule = false;
    do
    {
        if (!_tcsicmp(me.szModule, pszModuleName))
        {
            pMod = me.modBaseAddr;
            break;
        }
    } while (Module32Next(hShot, &me));

    CloseToolhelp32Snapshot(hShot);
    return pMod;
}

int GetFirstThreadID(int PID)
{
    HANDLE hShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, PID);
    if (hShot == INVALID_HANDLE_VALUE)
        return 0;
    LPVOID pMod = 0;

    THREADENTRY32 te;
    te.dwSize = sizeof(te);
    Thread32First(hShot, &te);
    int ID = 0;
    do
    {
        if (te.th32OwnerProcessID == PID)
            ID = te.th32ThreadID;
    } while (Thread32Next(hShot, &te));
    CloseToolhelp32Snapshot(hShot);
    return ID;
}