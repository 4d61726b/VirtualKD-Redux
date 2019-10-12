#include "stdafx.h"
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

            if ((pEntry->GetDebuggerMode() == kdCustom) && (!pEntry->GetCustomKDName().icompare(_T("kdbazis.dll"))))
            {
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
