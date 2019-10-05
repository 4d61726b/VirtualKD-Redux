/*! \file patchapi.h
\author Ivan Shcherbakov (Bazis)
$Id: patchapi.h,v 1.5 2009-12-31 13:06:41 Bazis Exp $
\brief Contains declarations for API simplifying VM patching.
*/

#pragma once

enum VMType
{
    kVMUnknown = 0,
    kVMWare32,
    kVMWare64,
    kVBox32,
    kVBox64,
};

extern "C"
{
    //! Creates a list of active VM sessions.
    /*!
        \return If this function succeeds, it returns a handle that can be used with GetNextVMSessionPID().
                If the function fails, it returns INVALID_HANDLE_VALUE.
        \remarks All handles obtained by this function should be closed with CloseVMSessionList().
    */
    __declspec(dllexport) HANDLE CreateVMSessionList();

    //! Frees a list of active VM sessions.
    __declspec(dllexport) void CloseVMSessionList(HANDLE hList);

    //! Gets a PID of a next VM session from a given list.
    /*!
        \return If no more VM sessions exist in the list, this function returns zero.
        \remarks To get a full list of VM session PIDs, simply call GetNextVMSessionPID() with a given
                 list handle repeatedly, until it returns zero.
    */
    __declspec(dllexport) unsigned GetNextVMSessionPID(HANDLE hList);

    __declspec(dllexport) unsigned GetNextVMSessionPIDEx(HANDLE hList, VMType *pVMType);

    //! Retrieves the name of a virtual machine (VM directory name), corresponding to a given session PID
    __declspec(dllexport) unsigned GetVMSessionNameW(unsigned PID, wchar_t *pName, size_t MaxNameLength);

    //! Determines, whether a given VM session is already patched by KDVM.
    __declspec(dllexport) bool IsVMSessionPatched(unsigned PID);

    //! Loads KDCLIENT.DLL into VM session, if it is not already loaded.
    __declspec(dllexport) bool PatchVMSessionIfNeeded(unsigned PID);

    //! Unloads KDCLIENT.DLL from a VM session, if it was loaded.
    __declspec(dllexport) bool UnpatchVMSessionIfNeeded(unsigned PID);

    typedef HANDLE HTHREAD;

    //! Unloads KDCLIENT.DLL from a VM session, if it was loaded. Returns NULL if already done.
    __declspec(dllexport) HTHREAD StartVMSessionPatching(unsigned PID);

    //! Unloads KDCLIENT.DLL from a VM session, if it was loaded. Returns NULL if already done.
    __declspec(dllexport) HTHREAD StartVMSessionUnpatching(unsigned PID);

    //! Returns a PID of a VM session with a given name (VM dir name), or 0 if it does not exist.
    __declspec(dllexport) int FindVMSessionByNameW(const wchar_t *pName);

    //! Gets a pipe name for a given VM session
    __declspec(dllexport) unsigned GetVMPipeNameW(unsigned PID, wchar_t *pName, size_t MaxNameLength, bool TryReconstructingIfNotAvailable);

    __declspec(dllexport) PVOID StartRemoteCallServer();
    __declspec(dllexport) void EndRemoteCallServer(PVOID pContext);
    __declspec(dllexport) void ProcessRemoteCallServerMessages(PVOID pContext);

    __declspec(dllexport) bool RevertVMToLastSnapshotByPID(unsigned PID);
    __declspec(dllexport) bool RevertVMToLastSnapshotByPipeNameW(const wchar_t *pPipeName);

    __declspec(dllexport) bool ForceInstantBreakInByPID(unsigned PID);
    __declspec(dllexport) bool ForceInstantBreakInByPipeNameW(const wchar_t *pPipeName);
};