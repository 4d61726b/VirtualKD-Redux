#include "stdafx.h"

#define NtQueryInformationProcess NtQueryInformationProcessDummy
#include <winternl.h>
#undef NtQueryInformationProcess

#include "cmdline.h"

#pragma comment(lib, "ntdll")

extern "C" NTSTATUS __stdcall
NtQueryInformationProcess (
						   IN HANDLE ProcessHandle,
						   IN PROCESSINFOCLASS ProcessInformationClass,
						   OUT PVOID ProcessInformation,
						   IN ULONG ProcessInformationLength,
						   OUT PULONG ReturnLength OPTIONAL
						   );

//http://undocumented.ntinternals.net/UserMode/Structures/RTL_USER_PROCESS_PARAMETERS.html
typedef struct 
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags; 
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	HANDLE StdInputHandle;
	HANDLE StdOutputHandle;
	HANDLE StdErrorHandle;
	UNICODE_STRING CurrentDirectoryPath;
	HANDLE CurrentDirectoryHandle; 
	UNICODE_STRING DllPath; 
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment; 
	ULONG StartingPositionLeft; 
	ULONG StartingPositionTop;
	ULONG Width; 
	ULONG Height;
	ULONG CharWidth;
	ULONG CharHeight; 
	ULONG ConsoleTextAttributes; 
	ULONG WindowFlags; 
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopName;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
} RTL_USER_PROCESS_PARAMETERS_FULL;

struct PEB_DETAILED { 
	BYTE Reserved1[2]; 
	BYTE BeingDebugged;  
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	PVOID Ldr;  
	RTL_USER_PROCESS_PARAMETERS_FULL *ProcessParameters;
	BYTE Reserved4[104];  
	PVOID Reserved5[52];
	PVOID PostProcessInitRoutine; 
	BYTE Reserved6[128];
	PVOID Reserved7[1];  
	ULONG SessionId;
};

using namespace BazisLib;

static bool QueryAndReadProcessMemory(HANDLE hProcess, PVOID pAddr, CBuffer &buf)
{
	MEMORY_BASIC_INFORMATION info = {0,};
	if (!VirtualQueryEx(hProcess, pAddr, &info, sizeof(info)))
		return false;

	size_t todo = ((INT_PTR)info.BaseAddress + info.RegionSize - (INT_PTR)pAddr);
	if (todo > buf.GetAllocated())
		todo = buf.GetAllocated();

	SIZE_T dwDone = 0;
	ReadProcessMemory(hProcess, pAddr, buf.GetData(), todo, &dwDone);
	buf.SetSize(dwDone);
	return true;
}

template<class _Struct> static bool ReadProcessStructure(HANDLE hProcess, PVOID pAddr, _Struct *pStruct)
{
	SIZE_T done = 0;
	if (!ReadProcessMemory(hProcess, pAddr, pStruct, sizeof(_Struct), &done))
		return false;
	if (done != sizeof(_Struct))
		return false;
	return true;
}

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

static void ParseEnvironmentStrings(const wchar_t *pwszBlock, size_t blockLength, RemoteProcessInfo &info)
{
	size_t startOff = 0;
	for(size_t i = 0; !blockLength || (i < blockLength); i++)
	{
		if (!pwszBlock[i])
		{
			if (startOff == i)
				break;

			const wchar_t *pwszStr = &pwszBlock[startOff];
			const wchar_t *pwszEq = wcschr(pwszStr, '=');
			if (pwszEq)
				info.Environment[DynamicStringW(pwszStr, pwszEq - pwszStr)] = pwszEq + 1;

			startOff = i + 1;
		}
	}

}

RemoteProcessInfo GetLocalProcessInfo()
{
	RemoteProcessInfo info;
	info.CommandLine = GetCommandLineW();
	LPWCH pW = GetEnvironmentStringsW();
	ParseEnvironmentStrings(pW, 0, info);
	FreeEnvironmentStringsW(pW);
	return info;
}

RemoteProcessInfo GetRemoteProcessInfo(DWORD PID)
{
	if (!PID)
		return GetLocalProcessInfo();

	HANDLE hProc = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, PID);
	if ((hProc == INVALID_HANDLE_VALUE) || !hProc)
		return RemoteProcessInfo();

	PROCESS_BASIC_INFORMATION basicInfo = { 0 };
	ULONG done = 0;
	NTSTATUS st = NtQueryInformationProcess(hProc, ProcessBasicInformation, &basicInfo, sizeof(basicInfo), &done);
	if (!NT_SUCCESS(st) || (done != sizeof(basicInfo)))
	{
		CloseHandle(hProc);
		return RemoteProcessInfo();
	}

	PEB_DETAILED peb = { 0 };
	RTL_USER_PROCESS_PARAMETERS_FULL userProcessParams = {0,};
	if (!ReadProcessStructure(hProc, basicInfo.PebBaseAddress, &peb) || !ReadProcessStructure(hProc, peb.ProcessParameters, &userProcessParams))
	{
		CloseHandle(hProc);
		return RemoteProcessInfo();
	}

	RemoteProcessInfo info;
	size_t lengthInChars = userProcessParams.CommandLine.Length / 2;
	SIZE_T done2 = 0;

	if (!ReadProcessMemory(hProc, userProcessParams.CommandLine.Buffer, info.CommandLine.PreAllocate(lengthInChars, false), lengthInChars * 2, &done2) || (done2 != (lengthInChars * 2)))
	{
		CloseHandle(hProc);
		return RemoteProcessInfo();
	}
	info.CommandLine.SetLength(lengthInChars);

	lengthInChars = userProcessParams.ImagePathName.Length / 2;
	if (!ReadProcessMemory(hProc, userProcessParams.ImagePathName.Buffer, info.EXEName.PreAllocate(lengthInChars, false), lengthInChars * 2, &done2) || (done2 != (lengthInChars * 2)))
	{
		CloseHandle(hProc);
		return RemoteProcessInfo();
	}
	info.EXEName.SetLength(lengthInChars);

	TypedBuffer<wchar_t> envBuf;
	envBuf.EnsureSize(1024 * 1024);
	if (!QueryAndReadProcessMemory(hProc, userProcessParams.Environment, envBuf))
	{
		CloseHandle(hProc);
		return info;
	}

	CloseHandle(hProc);
	ParseEnvironmentStrings((wchar_t *)envBuf.GetData(), envBuf.GetSize() / 2, info);
	return info;
}