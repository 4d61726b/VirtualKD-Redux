#pragma once

#if __has_include("C:\Program Files (x86)\VMware\VMware VIX\vix.h")
#include "C:\Program Files (x86)\VMware\VMware VIX\vix.h"
#define VIX_C_SUPPORTED
#endif

#ifdef VIX_C_SUPPORTED
#include <BazisLib\bzscore\Win32\registry.h>

class VixDLL
{
private:
    typedef decltype(&VixHost_Connect) fVixHost_Connect;
    typedef decltype(&VixJob_Wait) fVixJob_Wait;
    typedef decltype(&Vix_ReleaseHandle) fVix_ReleaseHandle;
    typedef decltype(&VixVM_Open) fVixVM_Open;
    typedef decltype(&VixVM_GetCurrentSnapshot) fVixVM_GetCurrentSnapshot;
    typedef decltype(&VixVM_RevertToSnapshot) fVixVM_RevertToSnapshot;
    typedef decltype(&VixHost_Disconnect) fVixHost_Disconnect;
    HMODULE m_hVixDLL = nullptr;
    fVixHost_Connect m_VixHost_Connect = nullptr;
    fVixJob_Wait m_VixJob_Wait = nullptr;
    fVix_ReleaseHandle m_Vix_ReleaseHandle = nullptr;
    fVixVM_Open m_VixVM_Open = nullptr;
    fVixVM_GetCurrentSnapshot m_VixVM_GetCurrentSnapshot = nullptr;
    fVixVM_RevertToSnapshot m_VixVM_RevertToSnapshot = nullptr;
    fVixHost_Disconnect m_VixHost_Disconnect = nullptr;

public:
    VixDLL()
    {
        BazisLib::Win32::RegistryKey key (HKEY_LOCAL_MACHINE, L"SOFTWARE\\VMware, Inc.\\VMware Workstation", 0, false);
        BazisLib::DynamicStringW dllpath = key[L"InstallPath"];

        if (!dllpath.empty())
        {
            SetDllDirectoryW(dllpath.c_str());
            m_hVixDLL = LoadLibraryW(L"vix.dll");
        }

        if (m_hVixDLL)
        {
            m_VixHost_Connect = (fVixHost_Connect)GetProcAddress(m_hVixDLL, "VixHost_Connect");
            m_VixJob_Wait = (fVixJob_Wait)GetProcAddress(m_hVixDLL, "VixJob_Wait");
            m_Vix_ReleaseHandle = (fVix_ReleaseHandle)GetProcAddress(m_hVixDLL, "Vix_ReleaseHandle");
            m_VixVM_Open = (fVixVM_Open)GetProcAddress(m_hVixDLL, "VixVM_Open");
            m_VixVM_GetCurrentSnapshot = (fVixVM_GetCurrentSnapshot)GetProcAddress(m_hVixDLL, "VixVM_GetCurrentSnapshot");
            m_VixVM_RevertToSnapshot = (fVixVM_RevertToSnapshot)GetProcAddress(m_hVixDLL, "VixVM_RevertToSnapshot");
            m_VixHost_Disconnect = (fVixHost_Disconnect)GetProcAddress(m_hVixDLL, "VixHost_Disconnect");
        }
    }

    bool VixDLLIsValid()
    {
        return (m_hVixDLL != nullptr && m_VixHost_Connect != nullptr && m_VixJob_Wait != nullptr && m_Vix_ReleaseHandle != nullptr && m_VixVM_Open != nullptr &&
                m_VixVM_GetCurrentSnapshot != nullptr && m_VixVM_RevertToSnapshot != nullptr && m_VixHost_Disconnect != nullptr);
    }


    VixHandle VixHost_Connect(int apiVersion,
        VixServiceProvider hostType,
        const char* hostName,
        int hostPort,
        const char* userName,
        const char* password,
        VixHostOptions options,
        VixHandle propertyListHandle,
        VixEventProc* callbackProc,
        void* clientData)
    {
        return m_VixHost_Connect(apiVersion, hostType, hostName, hostPort, userName, password, options, propertyListHandle, callbackProc, clientData);
    }

    VixError VixJob_Wait(VixHandle jobHandle,
        VixPropertyID firstPropertyID)
    {
        return m_VixJob_Wait(jobHandle, firstPropertyID);
    }

    VixError VixJob_Wait(VixHandle jobHandle,
        VixPropertyID firstPropertyID,
        VixHandle* v1,
        VixPropertyID v2)
    {
        return m_VixJob_Wait(jobHandle, firstPropertyID, v1, v2);
    }

    void Vix_ReleaseHandle(VixHandle handle)
    {
        return m_Vix_ReleaseHandle(handle);
    }

    VixHandle VixVM_Open(VixHandle hostHandle,
        const char* vmxFilePathName,
        VixEventProc* callbackProc,
        void* clientData)
    {
        return m_VixVM_Open(hostHandle, vmxFilePathName, callbackProc, clientData);
    }

    VixError VixVM_GetCurrentSnapshot(VixHandle vmHandle,
        VixHandle* snapshotHandle)
    {
        return m_VixVM_GetCurrentSnapshot(vmHandle, snapshotHandle);
    }

    VixHandle VixVM_RevertToSnapshot(VixHandle vmHandle,
        VixHandle snapshotHandle,
        int options,
        VixHandle propertyListHandle,
        VixEventProc* callbackProc,
        void* clientData)
    {
        return m_VixVM_RevertToSnapshot(vmHandle, snapshotHandle, options, propertyListHandle, callbackProc, clientData);
    }
    
    void VixHost_Disconnect(VixHandle hostHandle)
    {
        m_VixHost_Disconnect(hostHandle);
    }

    ~VixDLL()
    {
        if (m_hVixDLL)
        {
            FreeLibrary(m_hVixDLL);
        }
    }
};
#endif
