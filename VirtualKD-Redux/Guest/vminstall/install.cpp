#include "stdafx.h"

#include <Windows.h>

#include <BazisLib/bzscore/file.h>
#include <BazisLib/bzshlp/Win32/BCD.h>
#include <BazisLib/bzscore/strfast.h>
#include "bootedit.h"
#include "install.h"

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
