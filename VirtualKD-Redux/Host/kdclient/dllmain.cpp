/*! \file dllmain.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: dllmain.cpp,v 1.7 2009-09-15 11:06:50 Administrator Exp $
    \brief Contains the DllMain() implementation
*/

#include "stdafx.h"
#include <BazisLib/bzscore/Win32/registry.h>

#include "regconfig.h"

bool HookVmware(HINSTANCE hThisDLL);

void OnDLLProcessDetach();

HMODULE g_hThisDll = 0;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hThisDll = hModule;
        if (!HookVmware((HINSTANCE)hModule))
            return FALSE;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        OnDLLProcessDetach();
        break;
    }
    return TRUE;
}

extern "C" bool DllRegisterServer()
{
    TCHAR tszThisDLL[MAX_PATH];
    GetModuleFileName(g_hThisDll, tszThisDLL, __countof(tszThisDLL));
    BazisLib::RegistryKey key(HKEY_LOCAL_MACHINE, VKD_REGISTRY_VIRTUALBOX_PATH);
    key[_T("KDClientDLLPath")] = tszThisDLL;

    return true;
}