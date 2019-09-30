#include "stdafx.h"
#include "VBoxCmdLine.h"
#include <atlbase.h>
#include <atlcomcli.h>
#include <BazisLib/bzshlp/Win32/process.h>
#include "../kdclient/vmapi/VirtualBox.h"
#include <BazisLib/bzscore/Win32/datetime.h>

extern HINSTANCE g_hThisDll;

HRESULT StartRevertingCurrentVMToLastSnapshot_VBox()
{
	TCHAR tszDll[MAX_PATH];
	GetModuleFileName(g_hThisDll, tszDll, _countof(tszDll));

	BazisLib::String machineID = VBoxCmdLineToMachineID(GetCommandLineW()); 

	BazisLib::Win32::Process process(BazisLib::String::sFormat(L"rundll32 \"%s\",VMSnapshotRevertingEntry_VBox --startvm %s", tszDll, machineID.c_str()).c_str(), (WORD)SW_SHOW);
	return S_OK;
}

static HRESULT WaitForCompletion(CComPtr<IProgress> pProgress)
{
	for (;;)
	{
		HRESULT hR = pProgress->WaitForCompletion(100);
		if (!SUCCEEDED(hR))
			return hR;
		BOOL done = 0;
		hR = pProgress->get_Completed(&done);
		if (!SUCCEEDED(hR))
			return hR;
		if (done)
		{
			LONG code = 0;
			hR = pProgress->get_ResultCode(&code);
			if (SUCCEEDED(hR))
				return hR;
			return code;
		}
	}
}

static HRESULT DoRevertVMToSnapshot()
{
	BazisLib::String rawMachineID = VBoxCmdLineToMachineID(GetCommandLineW());
	if (rawMachineID.empty())
		return E_FAIL;

	CComBSTR machineID = rawMachineID.c_str();

	CComPtr<IVirtualBox> pVirtualBox;
	HRESULT hR = pVirtualBox.CoCreateInstance(CLSID_VirtualBox, NULL, CLSCTX_LOCAL_SERVER);
	if (!SUCCEEDED(hR))
		return hR;

	CComPtr<IMachine> pMachine;
	hR = pVirtualBox->FindMachine(machineID, &pMachine);
	if (!SUCCEEDED(hR))
		return hR;

	CComPtr<ISession> pSession;
	hR = pSession.CoCreateInstance(CLSID_Session, NULL, CLSCTX_INPROC_SERVER);
	if (!SUCCEEDED(hR))
		return hR;

	hR = pMachine->LockMachine(pSession, LockType_Shared);
	if (!SUCCEEDED(hR))
	{
		if (hR == E_FAIL)
			MessageBox(HWND_DESKTOP, L"Cannot connect to VirtualBox. Ensure that both VirtualBox and Visual Studio are running from the same user account, either both elevated (UAC), or both not.", L"Error", MB_ICONERROR);
		return hR;
	}

	CComPtr<IConsole> pConsole;
	hR = pSession->get_Console(&pConsole);
	if (!SUCCEEDED(hR))
		return hR;

	CComPtr<ISnapshot> pSnapshot;
	hR = pMachine->get_CurrentSnapshot(&pSnapshot);
	if (!SUCCEEDED(hR))
	{
		pSession->UnlockMachine();
		return hR;
	}

	CComBSTR snapshotID;
	hR = pSnapshot->get_Id(&snapshotID);
	if (!SUCCEEDED(hR))
		return hR;

	CComPtr<IProgress> pProgress;
	hR = pConsole->PowerDown(&pProgress);
	if (!SUCCEEDED(hR))
		return hR;

	hR = WaitForCompletion(pProgress);
	if (!SUCCEEDED(hR))
		return hR;

	pProgress = NULL;
	pSession->UnlockMachine();

	BazisLib::DateTime dtStart = BazisLib::DateTime::Now();
	
	for (;;)
	{
		hR = pMachine->LockMachine(pSession, LockType_Shared);
		if (!SUCCEEDED(hR) && (dtStart.MillisecondsElapsed() >= 10000))
			return hR;
		if (SUCCEEDED(hR))
			break;
		Sleep(100);
	}

	pConsole = NULL;
	hR = pSession->get_Console(&pConsole);
	if (!SUCCEEDED(hR))
		return hR;

	hR = pConsole->RestoreSnapshot(pSnapshot, &pProgress);
	if (!SUCCEEDED(hR))
		return hR;

	hR = WaitForCompletion(pProgress);
	if (!SUCCEEDED(hR))
		return hR;
	
	pSession->UnlockMachine();

	CComBSTR sessionType = L"gui";
	pProgress = NULL;

	hR = pMachine->LaunchVMProcess(pSession, sessionType, NULL, &pProgress);
	if (!SUCCEEDED(hR))
		return hR;

	hR = WaitForCompletion(pProgress);
	if (!SUCCEEDED(hR))
		return hR;

	pSession->UnlockMachine();


	pProgress = NULL;

	return S_OK;
}

extern "C" void __declspec(dllexport) VMSnapshotRevertingEntry_VBox()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hR = DoRevertVMToSnapshot();
	CoUninitialize();
	ExitProcess(hR);
}