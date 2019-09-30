/*! \file patchapi.cpp
\author Ivan Shcherbakov (Bazis)
$Id: patchapi.cpp,v 1.8 2009-12-31 13:06:41 Bazis Exp $
\brief Contains implementation for API simplifying VM patching.
*/

#include "stdafx.h"
#include "patchapi.h"
#include "findproc.h"
#include <TlHelp32.h>
#include <assert.h>
#include "loader.h"
#include "hook64.h"
#include "32to64.h"
#include <BazisLib/bzshlp/Win32/wow64.h>
#include <ShellApi.h>
#include <BazisLib/bzscore/datetime.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlcomcli.h>
#include <atlsafe.h>
#include "vkdversion.h"

#include "VBoxCmdLine.h"
#include "VMAPI/VirtualBox.h"
#include "rpcdispatch/ServiceCommandProtocol.h"
#include "rpcdispatch/rpcdisp.h"
#include "rpcdispatch/kdcomdisp.h"

#include <string>

BazisLib::WOW64APIProvider g_Wow64;

static bool IsRundll64Required(unsigned PID)
{
#ifdef _WIN64
	return false;
#else
	return g_Wow64.IsWow64Process() && !g_Wow64.IsWow64Process(PID);
#endif
}

extern HMODULE g_hThisDll;

//! Enables debugging privilege for calling process. Required to debug system services.
void EnableDebugPrivilege()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL);

	CloseHandle(hToken);
}


HANDLE CreateVMSessionList()
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hSnap == INVALID_HANDLE_VALUE)
		return INVALID_HANDLE_VALUE;
	PROCESSENTRY32 entry = {0, };
	entry.dwSize = sizeof(entry);
	//First process should always be the system process. That way, we can simply ignore it and use Process32Next() every time we want to find a next session.
	Process32First(hSnap, &entry);
	assert(!_tcsstr(entry.szExeFile, _T("vmware-vmx.exe")));
	assert(!_tcsstr(entry.szExeFile, _T("vmware-vmx-debug.exe")));
	assert(!_tcsstr(entry.szExeFile, _T("vmware-vmx-stats.exe")));
	assert(!_tcsstr(entry.szExeFile, _T("VirtualBox.exe")));
	return hSnap;
}

void CloseVMSessionList( HANDLE hList )
{
	CloseHandle(hList);
}

unsigned GetNextVMSessionPID( HANDLE hList )
{
	return GetNextVMSessionPIDEx(hList, NULL);
}

unsigned GetNextVMSessionPIDEx( HANDLE hList, VMType *pVMType )
{
	TCHAR *tszNames[] = {_T("VirtualBox.exe"), _T("vmware-vmx.exe"), _T("vmware-vmx-debug.exe"), _T("vmware-vmx-stats.exe")};
	unsigned PID = 0;
	unsigned idxMatch = 0;
	for (;;)
	{
		PID = FindProcessByNames(hList, tszNames, __countof(tszNames), false, &idxMatch);
		if ((PID == -1) || !PID)
			return 0;
		if (idxMatch)
			break;
		//Skip VirtualBox processes not running any VMs
		wchar_t sessionName[MAX_PATH];
		if (GetVMSessionNameW(PID, sessionName, __countof(sessionName)))
			break;
	}
	if (pVMType)
	{
		if (!PID)
			*pVMType = kVMUnknown;
		{
			BazisLib::WOW64APIProvider wow;
#ifdef _WIN64
			bool Is64 = !wow.IsWow64Process(PID);
#else
			bool Is64 = wow.IsWow64Process() && !wow.IsWow64Process(PID);
#endif
			if (idxMatch)
				*pVMType = Is64 ? kVMWare64 : kVMWare32;
			else
				*pVMType = Is64 ? kVBox64 : kVBox32;
		}
	}
	return PID;
}

unsigned SessionNameFromVMWareCmdLineW(const wchar_t *pszCmdLineConst, wchar_t *pName, size_t MaxNameLength)
{
	BazisLib::DynamicStringW cmdLineCopy = pszCmdLineConst;
	wchar_t *pszCmdLine = const_cast<wchar_t *>(cmdLineCopy.c_str());
	wchar_t *pVmx = wcsstr(pszCmdLine, L".vmx");
	if (!pVmx)
		return 0;
	for (;;)
	{
		wchar_t *p = wcsstr(pVmx + 1, L".vmx");
		if (!p)
			break;
		pVmx = p;
	}
	if (pVmx)
		pVmx[0] = 0;

	wchar_t *end = wcsrchr(pszCmdLine, '\\');
	if (!end)
		return 0;
	end[0] = 0;
	wchar_t *start = wcsrchr(pszCmdLine, '\\');
	if (!start)
		return 0;
	else
		start++;

	size_t todo = end - start;
	if (todo >= MaxNameLength)
		todo = MaxNameLength - 1;
	memcpy(pName, start, todo * sizeof(wchar_t *));
	pName[todo] = 0;
	return (unsigned)todo;
}

bool VersionNumbersFromVMCmdLineW(const RemoteProcessInfo &info, DWORD& dwMajor, DWORD& dwMinor, DWORD& dwBugfix)
{
	TCHAR* pStart = _tcsstr((TCHAR*)info.CommandLine.c_str(), _T(";version="));
	pStart += _tcslen(_T(";version="));

	dwMajor = 0;
	dwMinor = 0;
	dwBugfix = 0;

	if (pStart >= (TCHAR*)info.CommandLine.c_str() + _tcslen((TCHAR*)info.CommandLine.c_str()))
	{
		return false;
	}
#ifdef UNICODE
	WCHAR* wszDotOne = wcschr(pStart, L'.');
	if (wszDotOne)
	{
		WCHAR* wszDotTwo = wcschr(wszDotOne + 1, L'.');
		if (wszDotTwo)
		{
			WCHAR* wszEnd = wcschr(wszDotTwo, L';');
			if (wszEnd)
			{
				dwMajor = wcstoul(pStart, &wszDotOne, 10);
				dwMinor = wcstoul(wszDotOne+1, &wszDotTwo, 10);
				dwBugfix = wcstoul(wszDotTwo+1, &wszEnd, 10);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
#else
#error "Not Implemented"
#endif
	return true;
}

#include "cmdline.h"

unsigned SessionNameFromVMCmdLineW(const RemoteProcessInfo &info, wchar_t *pName, size_t MaxNameLength)
{
	int argc = 0;
	LPWSTR *pArgs = CommandLineToArgvW(info.CommandLine.c_str(), &argc);
	if (!pArgs)
		return 0;

	BazisLib::String exeName;

	if (argc && pArgs[0])
		exeName = pArgs[0];

	LocalFree(pArgs);
	pArgs = NULL;

	if (wcsstr(exeName.c_str(), L"vmware-vmx.exe") || wcsstr(exeName.c_str(), L"vmware-vmx-debug.exe") || wcsstr(exeName.c_str(), L"vmware-vmx-stats.exe"))
		return SessionNameFromVMWareCmdLineW(info.CommandLine.c_str(), pName, MaxNameLength);
	else if ((wcsstr(exeName.c_str(), L"VirtualBox.exe") || wcsstr(info.EXEName.c_str(), L"VirtualBox.exe")))
		return VBoxCmdLineToVMNameW(info, pName, MaxNameLength);
	else
		return 0;
}

#include "cmdline.h"

unsigned GetVMSessionNameW(unsigned PID, wchar_t *pName, size_t MaxNameLength)
{
#ifndef _WIN64
	if (IsRundll64Required(PID))
	{
		return (unsigned)Call64BitKDCLIENT(kGetVMSessionName, PID, pName, MaxNameLength);
	}
#endif
	extern HMODULE g_hThisDll;
	EnableDebugPrivilege();
	if (!PID || !pName)
		return 0;
	return SessionNameFromVMCmdLineW(GetRemoteProcessInfo(PID), pName, MaxNameLength);
}

bool IsVMSessionPatched( unsigned PID )
{
#ifndef _WIN64
	if (IsRundll64Required(PID))
	{
		return (Call64BitKDCLIENT(kIsSessionPatched, PID) == 1);
	}
#endif
	if (GetRemoteModuleHandle64Aware(PID, _T("VBoxKD.dll")) || GetRemoteModuleHandle64Aware(PID, _T("VBoxKD64.dll")))
		return true;
	RemoteDllLoader ldr(g_hThisDll, false);
	return (ldr.FindLibraryInProcess(PID, true) != 0);
}

static bool s_bUserWarnedAboutVBox = false;
static bool s_bUserWarnedAboutVersion = false;

HANDLE StartPatcherThread( unsigned PID, DWORD *pPatcherThreadID = NULL)
{
	EnableDebugPrivilege();
	if (GetRemoteModuleHandle64Aware(PID, _T("VBoxDD.dll")))
	{
		//We are inside VirtualBox

#ifdef _WIN64
		if (GetRemoteModuleHandle64Aware(PID, _T("kdclient64.dll")))
#else
		if (GetRemoteModuleHandle64Aware(PID, _T("kdclient.dll")))
#endif
			return 0;


		if (!s_bUserWarnedAboutVBox)
		{
			s_bUserWarnedAboutVBox = true;
			MessageBox(0, _T("VirtualKD-Redux cannot patch VirtualBox on-the-fly.\r\nPlease register the VirtualKD device for VirtualBox by running \"regsvr32 VBoxKD64.dll\". If this does not help, close all instances of VirtualBox and terminate VBoxSVC.exe and try again."),
				_T("VirtualKD-Redux"),
				MB_ICONWARNING | MB_TASKMODAL);
		}
		return INVALID_HANDLE_VALUE;
	}
	RemoteDllLoader ldr(g_hThisDll, false);
	if (ldr.FindLibraryInProcess(PID))
		return NULL;
	return ldr.InitiateDLLLoading(PID, pPatcherThreadID);
}


HANDLE StartUnpatcherThread( unsigned PID, DWORD *pPatcherThreadID = NULL)
{
	if (GetRemoteModuleHandle64Aware(PID, _T("VBoxDD0.dll")))
		return INVALID_HANDLE_VALUE;
	EnableDebugPrivilege();
	RemoteDllLoader ldr(g_hThisDll, false);
	if (!ldr.FindLibraryInProcess(PID))
		return NULL;
	return ldr.InitiateDLLUnloading(PID, true, pPatcherThreadID);
}

static bool DoSynchronousThreadOperation(HANDLE hThread)
{
	if (hThread == NULL)
		return true;
	else if (hThread == INVALID_HANDLE_VALUE)
		return false;
	else
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		DWORD dwCode = 0;
		GetExitCodeThread(hThread, &dwCode);
		return (dwCode != 0);
	}
}

bool PatchVMSessionIfNeeded(unsigned PID)
{
#ifndef _WIN64
	if (IsRundll64Required(PID))
	{
		return (Call64BitKDCLIENT(kPatchAndWait, PID) != 0);
	}
#endif
	return DoSynchronousThreadOperation(StartPatcherThread(PID));
}

bool UnpatchVMSessionIfNeeded(unsigned PID)
{
#ifndef _WIN64
	if (IsRundll64Required(PID))
	{
		return (Call64BitKDCLIENT(kUnpatchAndWait, PID) != 0);
	}
#endif
	return DoSynchronousThreadOperation(StartUnpatcherThread(PID));
}

HTHREAD StartVMSessionPatching(unsigned PID)
{
#ifndef _WIN64
	if (IsRundll64Required(PID))
	{
		DWORD dwID = (DWORD)Call64BitKDCLIENT(kStartVMSessionPatching, PID);
		EnableDebugPrivilege();
		if(!dwID)
			return 0;
		else if (dwID == -1)
			return INVALID_HANDLE_VALUE;
		else
		{
			HANDLE h = OpenThread(THREAD_ALL_ACCESS, FALSE, dwID);
			if (!h || (h == INVALID_HANDLE_VALUE))	//Thread already finished
				return 0;
			return h;
		}
	}
#endif
	return StartPatcherThread(PID);
}

HTHREAD StartVMSessionUnpatching(unsigned PID)
{
#ifndef _WIN64
	if (IsRundll64Required(PID))
	{
		DWORD dwID = (DWORD)Call64BitKDCLIENT(kStartVMSessionUnpatching, PID);
		EnableDebugPrivilege();
		if(!dwID)
			return 0;
		else if (dwID == -1)
			return INVALID_HANDLE_VALUE;
		else
		{
			HANDLE h = OpenThread(THREAD_ALL_ACCESS, FALSE, dwID);
			if (!h || (h == INVALID_HANDLE_VALUE))	//Thread already finished
				return 0;
			return h;
		}
	}
#endif
	return StartUnpatcherThread(PID);
}

int FindVMSessionByNameW( const wchar_t *pName )
{
	if (!pName)
		return 0;
	wchar_t wsz[512] = {0,};
	
	HANDLE hSnap = CreateVMSessionList();
	if (hSnap == INVALID_HANDLE_VALUE)
		return 0;
	int PID = 0;
	while (PID = GetNextVMSessionPID(hSnap))
	{
		if (!GetVMSessionNameW(PID, wsz, sizeof(wsz)/sizeof(wsz[0])))
			continue;
		if (!_wcsicmp(wsz, pName))
			break;
	}
	CloseVMSessionList(hSnap);
	return PID;
}

unsigned GetVMPipeNameW(unsigned PID, wchar_t *pName, size_t MaxNameLength, bool TryReconstructingIfNotAvailable)
{
	if (!pName || !PID)
		return 0;
	pName[0] = 0;

	TCHAR tszMappingName[MAX_PATH];
	_sntprintf_s(tszMappingName, __countof(tszMappingName), _TRUNCATE, tszMappingNameFormat, PID);
	//HANDLE hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY, 0, sizeof(KdClientStatus), tszMappingName);
	HANDLE hMapping = OpenFileMapping(FILE_MAP_READ, FALSE, tszMappingName);
	bool bOldVersion = false;
	KdClientStatus *pStatus = (KdClientStatus *)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, sizeof(KdClientStatus));
	if (!pStatus)
	{
		bOldVersion = true;
		pStatus = (KdClientStatus *)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	}
	BazisLib::DateTime startTime = BazisLib::DateTime::Now();
	if ((hMapping == INVALID_HANDLE_VALUE) || !pStatus)
	{
		if (hMapping != INVALID_HANDLE_VALUE)
			CloseHandle(hMapping);
		if (!TryReconstructingIfNotAvailable)
			return 0;
		
		TCHAR tszSession[MAX_PATH] = {0,};
		GetVMSessionNameW(PID, tszSession, __countof(tszSession));
		if (!tszSession[0])
			return 0;
		unsigned len = _snwprintf(pName, MaxNameLength, L"\\\\.\\pipe\\kd_%s", tszSession);
		for (int i = 0; pName[i]; i++)
			if (pName[i] == ' ')
				pName[i] = '_';
		return len;
	}
	unsigned len = (unsigned)wcslen(pStatus->PipeName);
	wcsncpy_s(pName, MaxNameLength, pStatus->PipeName, _TRUNCATE);

	if ((bOldVersion || (pStatus->HostSideDLLVersion != VIRTUALKD_VER_INT)) && !s_bUserWarnedAboutVersion)
	{
		unsigned oldVersion = 0, newVersion = VIRTUALKD_VER_INT;
		if (pStatus)
			oldVersion = pStatus->HostSideDLLVersion;

		oldVersion >>= 16;
		newVersion >>= 16;

		s_bUserWarnedAboutVersion = true;
		MessageBox(0, 
			BazisLib::String::sFormat(_T("One of the virtual machines has an old version of KDCLIENT.DLL/VBOXDD.DLL loaded (%d.%d.%d expected, %d.%d.%d found). Please install the latest file versions, as described on VirtualKD-Redux website, and restart your virtual machines."),
			(oldVersion >> 12) & 0x0F,
			(oldVersion >> 8) & 0x0F,
			(oldVersion >> 4) & 0x0F,
			(newVersion >> 12) & 0x0F,
			(newVersion >> 8) & 0x0F,
			(newVersion >> 4) & 0x0F).c_str(),
			_T("VirtualKD-Redux"),
			MB_ICONWARNING | MB_TASKMODAL);
	}

	UnmapViewOfFile(pStatus);
	CloseHandle(hMapping);
	return len;
}

bool ForceInstantBreakInByPID(unsigned PID)
{
	wchar_t wszName[512];
	if (!GetVMPipeNameW(PID, wszName, _countof(wszName), true))
		return false;
	return ForceInstantBreakInByPipeNameW(wszName);
}

bool RevertVMToLastSnapshotByPID(unsigned PID)
{
	wchar_t wszName[512];
	if (!GetVMPipeNameW(PID, wszName, _countof(wszName), true))
		return false;
	return RevertVMToLastSnapshotByPipeNameW(wszName);
}

static HRESULT ExecuteServiceCommand(const wchar_t *pPipeName, ServiceProtocolCommand cmd)
{
	wchar_t wszName[512];
	_snwprintf(wszName, _countof(wszName),  L"%s%s", pPipeName, VIRTUALKD_SERVICE_PROTOCOL_PIPE_SUFFIX);

	ServiceProtocolPacket packet;
	packet.Command = cmd;
	ServiceProtocolReply reply;
	DWORD dwDone = 0;

	if (!CallNamedPipe(wszName, &packet, sizeof(packet), &reply, sizeof(reply), &dwDone, 100))
		return E_FAIL;
	if (dwDone != sizeof(reply))
		return E_FAIL;
	return reply.Status;
}

bool ForceInstantBreakInByPipeNameW(const wchar_t *pPipeName)
{
	return SUCCEEDED(ExecuteServiceCommand(pPipeName, kInstantBreak));
}

bool RevertVMToLastSnapshotByPipeNameW(const wchar_t *pPipeName)
{
	return SUCCEEDED(ExecuteServiceCommand(pPipeName, kRevertToLastSnapshot));
}

