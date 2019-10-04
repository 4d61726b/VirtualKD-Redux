#include "stdafx.h"
#include "VBoxCmdLine.h"
#include <BazisLib/bzscore/autofile.h>
#include <BazisLib/bzshlp/textfile.h>
#include <BazisLib/bzscore/cmndef.h>

using namespace BazisLib;

#define UNIX_LINE_ENDING_SEQUENCE_A		'\n'
#define UNIX_LINE_ENDING_SEQUENCE_LEN	1	

typedef _Core::_TextFile<char, UNIX_LINE_ENDING_SEQUENCE_A, UNIX_LINE_ENDING_SEQUENCE_LEN> TextUnixAnsiFileReader;

//D:\PROGRA~1\Sun\XVMVIR~1\VirtualBox.exe --startvm 94fccf02-4269-4e30-b7f9-8bc9a5cf67b6

BazisLib::String VBoxCmdLineToMachineID(const wchar_t *pCmdLine)
{
	const wchar_t *pGuid = wcsstr(pCmdLine, L"--startvm ");
	if (!pGuid)
		return BazisLib::String();
	return BazisLib::String(pGuid + 10, min(36, wcslen(pGuid + 10)));
}

unsigned VBoxCmdLineToVMNameW( const RemoteProcessInfo &info, wchar_t *pNameBuffer, size_t MaxLength )
{
	if (info.CommandLine.empty() || !pNameBuffer || !MaxLength)
		return 0;

	size_t idx = info.CommandLine.find(L"--comment");
	if (idx != info.CommandLine.npos)
	{
		size_t vmOff = info.CommandLine.find_first_not_of(L" ", idx + 9);
		if (vmOff != info.CommandLine.npos)
		{
			size_t vmEnd;
			if (info.CommandLine[vmOff] == '\"')
				vmEnd = info.CommandLine.find('\"', ++vmOff);
			else
				vmEnd = info.CommandLine.find(' ', vmOff);

			if (vmEnd != info.CommandLine.npos)
			{
				size_t len = vmEnd - vmOff;
				wcsncpy(pNameBuffer, info.CommandLine.c_str() + vmOff, len);
				pNameBuffer[len] = 0;
				return (unsigned int)len;
			}

		}

	}

	DynamicStringW VBoxHome = info.GetEnvironmentVariable(L"VBOX_USER_HOME");
	if (VBoxHome.empty())
		VBoxHome = info.GetEnvironmentVariable(_T("USERPROFILE")) + _T("\\.VirtualBox");

	ManagedPointer<TextUnixAnsiFileReader> pReader = new TextUnixAnsiFileReader(new ACFile((VBoxHome + L"\\VirtualBox.xml"), FileModes::OpenReadOnly.ShareAll()));
	if (!pReader->Valid())
		return 0;
	
	String machineID = VBoxCmdLineToMachineID(info.CommandLine.c_str());
	if (machineID.empty())
		return 0;

	DynamicStringA strGuid = StringToANSIString(machineID);

	do
	{
		DynamicStringA str = pReader->ReadLine();
		if (str.empty())
			continue;
		off_t off = (off_t)str.find("<MachineEntry uuid=\"{");
		if (off == -1)
			continue;
		if (_memicmp(str.c_str() + off + 21, strGuid.c_str(), 36))
			continue;
		off = (off_t)str.find("\" src=\"", off);
		if (off == -1)
			continue;

		off_t offDot = (off_t)str.find_last_of('.');
		if (offDot == -1)
			offDot = (off_t)str.length();
		off_t offSlash = (off_t)str.find_last_of('\\');
		if (offSlash == -1)
			offSlash = off + 7;
		else
			offSlash++;
		if (offDot <= offSlash)			
			offDot = (off_t)str.length();

		size_t len = offDot - offSlash;
		if (len >= MaxLength)
			len = MaxLength - 1;

		wcsncpy(pNameBuffer, ANSIStringToString(str).c_str() + offSlash, len);
		pNameBuffer[len] = 0;

		wchar_t *pSlash = wcsrchr(pNameBuffer, '/');
		if (pSlash)
		{
			len -= (pSlash - pNameBuffer) + 1;
			memmove(pNameBuffer, pSlash + 1, (len + 1) * sizeof(wchar_t));
		}

		return (unsigned)len;

	} while (!pReader->IsEOF());

	return 0;
}


#ifndef VBOXDD_EXPORTS
unsigned VBoxCmdLineToPipeNameW(wchar_t *pNameBuffer, size_t MaxLength, unsigned PID )
{
	wchar_t wszVMName[128] = {0,};

	if (!VBoxCmdLineToVMNameW(GetRemoteProcessInfo(PID) ,wszVMName, __countof(wszVMName)) || wcschr(wszVMName, '/'))
		_snwprintf(wszVMName, __countof(wszVMName), L"VBOX%d", PID ? PID : GetCurrentProcessId());

	_snwprintf(pNameBuffer, MaxLength, L"\\\\.\\pipe\\kd_%s", wszVMName);
	for (int i = 0; pNameBuffer[i]; i++)
		if (pNameBuffer[i] == ' ')
			pNameBuffer[i] = '_';
	return (unsigned)wcslen(pNameBuffer);
}
#endif