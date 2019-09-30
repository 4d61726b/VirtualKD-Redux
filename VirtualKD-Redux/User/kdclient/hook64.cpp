/*! \file
    \author Ivan Shcherbakov (Bazis)
    $Id: hook64.cpp,v 1.5 2009-12-31 13:06:41 Bazis Exp $
    \brief Contains implementation for GetRemoteModuleHandle64Aware()
*/

#include "stdafx.h"
#include "hook64.h"

#include <windows.h>
#include <winternl.h>

#include <tlhelp32.h>
#include <BazisLib/bzshlp/Win32/wow64.h>

#pragma comment(lib, "ntdll")

/*
//! Rtl functions from http://native-nt-toolkit.googlecode.com/svn/trunk/ndk/rtlfuncs.h
extern "C"
{
	NTSYSAPI
		PVOID
		NTAPI
		RtlCreateQueryDebugBuffer(
		IN ULONG Size,
		IN BOOLEAN EventPair
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		RtlDestroyQueryDebugBuffer(IN PVOID DebugBuffer);

	NTSYSAPI
		NTSTATUS
		NTAPI
		RtlQueryProcessDebugInformation(
		IN ULONG ProcessId,
		IN ULONG DebugInfoClassMask,
		IN OUT PVOID DebugBuffer
		);
}

template <typename PVOID_NATIVE> class ModuleFinder
{
public:
//Structure definitions from here: http://native-nt-toolkit.googlecode.com/svn/trunk/ndk/rtltypes.h
	typedef struct _RTL_PROCESS_MODULE_INFORMATION
	{
		ULONG Section;
		PVOID_NATIVE MappedBase;
		PVOID_NATIVE ImageBase;
		ULONG ImageSize;
		ULONG Flags;
		USHORT LoadOrderIndex;
		USHORT InitOrderIndex;
		USHORT LoadCount;
		USHORT OffsetToFileName;
		CHAR FullPathName[256];
	} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

	typedef struct _RTL_PROCESS_MODULES
	{
		ULONG NumberOfModules;
		RTL_PROCESS_MODULE_INFORMATION Modules[1];
	} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

	typedef struct _RTL_PROCESS_MODULE_INFORMATION_EX
	{
		ULONG NextOffset;
		RTL_PROCESS_MODULE_INFORMATION BaseInfo;
		ULONG ImageCheckSum;
		ULONG TimeDateStamp;
		PVOID_NATIVE DefaultBase;
	} RTL_PROCESS_MODULE_INFORMATION_EX, *PRTL_PROCESS_MODULE_INFORMATION_EX;

	typedef struct _RTL_DEBUG_INFORMATION
	{
		PVOID_NATIVE SectionHandleClient;
		PVOID_NATIVE ViewBaseClient;
		PVOID_NATIVE ViewBaseTarget;
		PVOID_NATIVE ViewBaseDelta;
		PVOID_NATIVE EventPairClient;
		PVOID_NATIVE EventPairTarget;
		PVOID_NATIVE TargetProcessId;
		PVOID_NATIVE TargetThreadHandle;
		PVOID_NATIVE Flags;
		PVOID_NATIVE OffsetFree;
		PVOID_NATIVE CommitSize;
		PVOID_NATIVE ViewSize;
		union
		{
			PRTL_PROCESS_MODULES Modules;
			PVOID_NATIVE ModulesEx;
		};
		PVOID_NATIVE BackTraces;
		PVOID_NATIVE Heaps;
		PVOID_NATIVE Locks;
		HANDLE SpecificHeap;
		HANDLE TargetProcessHandle;
		PVOID_NATIVE VerifierOptions;
		HANDLE ProcessHeap;
		HANDLE CriticalSectionHandle;
		HANDLE CriticalSectionOwnerThread;
		PVOID_NATIVE Reserved[4];
	} RTL_DEBUG_INFORMATION, *PRTL_DEBUG_INFORMATION;


	static ULONGLONG GetRemoteModuleHandle64Aware(unsigned PID, LPCSTR lpModuleName, bool ShortName)
	{
		PRTL_DEBUG_INFORMATION pDbg = (PRTL_DEBUG_INFORMATION)RtlCreateQueryDebugBuffer(0, FALSE);
		NTSTATUS st = RtlQueryProcessDebugInformation(PID, 1, pDbg);
		if (st != 0)
			return 0;
		for (unsigned i = 0 ; i < pDbg->Modules->NumberOfModules; i++)
		{
			bool match;
			if (ShortName)
				match = !_stricmp(pDbg->Modules->Modules[i].FullPathName + pDbg->Modules->Modules[i].OffsetToFileName, lpModuleName);
			else
				match = !_stricmp(pDbg->Modules->Modules[i].FullPathName, lpModuleName);

			if (match)
			{
				ULONGLONG base = (ULONGLONG)pDbg->Modules->Modules[i].ImageBase;
				RtlDestroyQueryDebugBuffer(pDbg);
				return base;
			}
		}
		RtlDestroyQueryDebugBuffer(pDbg);
		return 0LL;
	}
};
*/

ULONGLONG GetRemoteModuleHandle64Aware(unsigned PID, LPCTSTR lpModuleName, bool ShortName)
{
#ifdef _WIN64
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, PID);
#else
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
#endif
	if (hSnap == INVALID_HANDLE_VALUE)
		return 0;
	MODULEENTRY32 mod = {0,};
	mod.dwSize = sizeof(mod);
	Module32First(hSnap, &mod);
	do 
	{
		if (ShortName)
		{
			if (!_tcsicmp(mod.szModule, lpModuleName))
			{
				CloseHandle(hSnap);
				return (ULONGLONG)mod.modBaseAddr;
			}
		}
		else
		{
			if (!_tcsicmp(mod.szExePath, lpModuleName))
			{
				CloseHandle(hSnap);
				return (ULONGLONG)mod.modBaseAddr;
			}
		}
	} while (Module32Next(hSnap, &mod));
	CloseHandle(hSnap);
	return 0;	
	//return ModuleFinder<PVOID>::GetRemoteModuleHandle64Aware(PID, lpModuleName, ShortName);
}
