//Based on https://msdn.microsoft.com/en-ca/library/windows/desktop/aa379620%28v=vs.85%29.aspx
#include "stdafx.h"
#include <stdio.h>
#include <accctrl.h>
#include <aclapi.h>
#include <BazisLib/bzscore/status.h>
#include <BazisLib/bzscore/Win32/security.h>

using namespace BazisLib;

ActionStatus SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
	)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		return MAKE_STATUS(ActionStatus::FromLastError());
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		return MAKE_STATUS(ActionStatus::FromLastError());
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		return MAKE_STATUS(ActionStatus::FromLastError());
	}

	return MAKE_STATUS(Success);
}


ActionStatus TakeOwnership(LPTSTR lpszOwnFile)
{
	HANDLE hToken = NULL;
	DWORD dwRes;
	ActionStatus st;

	BazisLib::Win32::Security::Sid sid(WinBuiltinAdministratorsSid);

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES,
		&hToken))
	{
		st = MAKE_STATUS(ActionStatus::FromLastError());
		goto Cleanup;
	}

	// Enable the SE_TAKE_OWNERSHIP_NAME privilege.
	st = SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE);
	if (!st.Successful())
	{
		goto Cleanup;
	}

	// Set the owner in the object's security descriptor.
	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,                 // name of the object
		SE_FILE_OBJECT,              // type of object
		OWNER_SECURITY_INFORMATION,  // change only the object's owner
		sid,                   // SID of Administrator group
		NULL,
		NULL,
		NULL);

	if (dwRes != ERROR_SUCCESS)
	{
		st = MAKE_STATUS(ActionStatus::FromWin32Error(dwRes));
		goto Cleanup;
	}

Cleanup:

	if (hToken)
		CloseHandle(hToken);

	return st;

}