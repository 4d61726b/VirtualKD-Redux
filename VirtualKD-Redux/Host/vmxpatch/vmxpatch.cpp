/*! \file vmxpatch.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: vmxpatch.cpp,v 1.11 2009-06-06 12:33:45 Administrator Exp $
    \brief Contains implementation for a simple console-based VM patcher
*/

#include "stdafx.h"
#include "../kdclient/patchapi.h"
#include "vkdversion.h"
#include <conio.h>

int _tmain(int argc, _TCHAR* argv[])
{
    printf("%s %s\n\n", PRODUCTNAME_STR, VIRTUALKD_REDUX_VERSION_STR);

    HANDLE hSnap = CreateVMSessionList();
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        printf("Unexpected error!\n");
        return -1;
    }

    int PIDs[9] = { 0, };
    int PIDCount = 0;

    do
    {
        PIDs[PIDCount++] = GetNextVMSessionPID(hSnap);
    } while (PIDs[PIDCount - 1] && (PIDCount < (sizeof(PIDs) / sizeof(PIDs[0]))));

    if (PIDCount && !PIDs[PIDCount])
        PIDCount--;

    CloseVMSessionList(hSnap);

    if (!PIDCount)
    {
        printf("Cannot find vmware-vmx.exe running!\n");
        return 1;
    }

    printf("Listing detected VM sessions:\n\n#\tPID\tName\n");
    for (int i = 0; i < PIDCount; i++)
    {
        wchar_t tszName[128] = { 0, };
        GetVMSessionNameW(PIDs[i], tszName, sizeof(tszName) / sizeof(tszName[0]));
        printf("%d\t%d\t%S\t%s\n", i + 1, PIDs[i], tszName, IsVMSessionPatched(PIDs[i]) ? "(already patched)" : "");
    }
    printf("------------------------------------\nWhich VM session to patch? (1-%d):", PIDCount);

    unsigned idx = _getche() - '1';
    printf("\n");
    if (idx >= (unsigned)PIDCount)
    {
        printf("Invalid index specified.\n");
        return 2;
    }

    if (!PatchVMSessionIfNeeded(PIDs[idx]))
    {
        printf("Failed to patch VM session.\n");
        return 3;
    }

    if (MessageBox(0, _T("The vmware-vmx.exe has been successfully patched. You can now use the debugger. Do you want to unpatch it back?"), _T("VM patcher"), MB_ICONQUESTION | MB_YESNO) != IDYES)
        return 0;

    if (!UnpatchVMSessionIfNeeded(PIDs[idx]))
    {
        printf("Failed to unpatch VM session.\n");
        return 3;
    }

    return 0;
}

