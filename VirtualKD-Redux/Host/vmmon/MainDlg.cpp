/*! \file MainDlg.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: MainDlg.cpp,v 1.12 2009-12-31 13:06:42 Bazis Exp $
    \brief Contains implementation for main VMMON dialog
*/

#include "stdafx.h"
#include "resource.h"
#include <BazisLib/bzscore/cmndef.h>
#include <BazisLib/bzshlp/Win32/findproc.h>
#include <BazisLib/bzscore/Win32/registry.h>
#include <BazisLib/bzshlp/ratecalc.h>
#include <BazisLib/bzshlp/Win32/wow64.h>

#include "MainDlg.h"
#include "trparams.h"

#include "../kdclient/patchapi.h"
#include "rpcdispatch/permdesc.h"

enum
{
	ProcessInactive,
	ProcessPending,
	ProcessPatched,
	ProcessFailed
};

using namespace BazisLib;

static const TCHAR tszRegistryPath[] = _T("SOFTWARE\\BazisSoft\\KdVMWare\\Monitor");

//! Creates a log pipe with a name corresponding to a specified PID
HANDLE CreateLogPipe(unsigned PID)
{
	PermissiveSecurityDescriptor desc;
	TCHAR tszPipeName[MAX_PATH];
	_sntprintf_s(tszPipeName, __countof(tszPipeName), _TRUNCATE, tszLogPipeNameFormat, PID);
	return CreateNamedPipe(tszPipeName, PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE, 2, 65536, 65536, 1000, desc);
}

static LPCTSTR GetVMTypeName(VMType type)
{
	switch (type)
	{
	case kVMWare32:
		return _T("VMWare");
	case kVBox32:
		return _T("VirtualBox");
	case kVMWare64:
		return _T("VMWare x64");
	case kVBox64:
		return _T("VirtualBox x64");
	default:
		return _T("Unknown");
	}
}

static void EnableDebugPrivilege()
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

CMainDlg::CMainDlg()
	: m_DbgToolsPath(_T(""))
	, m_bValidVMsWereFound(false)
{
	EnableDebugPrivilege();
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	m_ProcessorCount = sysinfo.dwNumberOfProcessors;

	RegistryKey key(HKEY_LOCAL_MACHINE, tszRegistryPath);
	Win32::RegistrySerializer::Deserialize(key, m_Params);

	String toolsPath(_T(""));
	if (m_Params.ToolsPath.empty())
	{
		RegistryKey key2(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"));
		toolsPath = (String)key2[_T("ProgramFilesDir")];
		toolsPath = Path::Combine(toolsPath, _T("Debugging Tools for Windows"));
	}
	else
		toolsPath = m_Params.ToolsPath;
	if (File::Exists(Path::Combine(toolsPath, _T("windbg.exe"))) && File::Exists(Path::Combine(toolsPath, _T("kd.exe"))))
		m_DbgToolsPath = toolsPath;

	m_pCallServerContext = StartRemoteCallServer();
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	m_ListView.m_hWnd = GetDlgItem(IDC_LIST1);
	m_StatsView.m_hWnd = GetDlgItem(IDC_STATS);
	m_DebugLevel.m_hWnd = GetDlgItem(IDC_DBGLEVEL);

	DlgResize_Init();

	//The lines in the combo box should correspond to DebugXXX enumeration values in status.h
	m_DebugLevel.InsertString(-1, _T("No debug messages"));
	m_DebugLevel.InsertString(-1, _T("Patcher debug messages"));
	//The following level of debug messages is NOT supported in release builds of KDCLIENT.DLL, as
	//it considerably slows down everything. KdTraceEnabled constant controls it.
#ifdef _DEBUG
	m_DebugLevel.InsertString(-1, _T("Patcher and KDCOM protocol messages (normal)"));
	m_DebugLevel.InsertString(-1, _T("Patcher and KDCOM protocol messages (detailed)"));
#endif

	if (m_Params.DebugLevel < (unsigned)m_DebugLevel.GetCount())
		m_DebugLevel.SetCurSel(m_Params.DebugLevel);

	m_ListView.AddColumn(_T("PID"), 0);
	m_ListView.AddColumn(_T("VM type"), 1);
	m_ListView.AddColumn(_T("Uptime"), 2);
	m_ListView.AddColumn(_T("CPU"), 3);
	m_ListView.AddColumn(_T("Pipe name"), 4);
	m_ListView.AddColumn(_T("Packets"), 5);
	m_ListView.AddColumn(_T("Resets"), 6);
	m_ListView.AddColumn(_T("OS"), 7);
	m_ListView.AddColumn(_T("Debugger"), 8);
	m_ListView.AddColumn(_T("Poll rate"), 9);

	m_ImageList.Create(16, 16, ILC_COLOR4 | ILC_MASK, 4, 0);
	m_ImageList.AddIcon(LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_GREY)));
	m_ImageList.AddIcon(LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_YELLOW)));
	m_ImageList.AddIcon(LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_GREEN)));
	m_ImageList.AddIcon(LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_RED)));

	m_ListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
	m_ListView.ModifyStyle(0, LVS_SINGLESEL);

	m_ListView.SetColumnWidth(0, 60);
	m_ListView.SetColumnWidth(1, 100);
	m_ListView.SetColumnWidth(2, 60);
	m_ListView.SetColumnWidth(3, 40);
	m_ListView.SetColumnWidth(4, 100);
	m_ListView.SetColumnWidth(5, 70);

	m_ListView.SetImageList(m_ImageList, LVSIL_SMALL);

	SendDlgItemMessage(IDC_STARTDBG,  BM_SETCHECK, m_Params.AutoInvokeDebugger ? BST_CHECKED : 0, 0);
	SendDlgItemMessage(IDC_STOPDBG,   BM_SETCHECK, m_Params.AutoCloseDebugger? BST_CHECKED : 0, 0);
	SendDlgItemMessage(IDC_BREAKIN,   BM_SETCHECK, m_Params.InitialBreakIn ? BST_CHECKED : 0, 0);
	switch(m_Params.DebuggerType)
	{
	case 0:
		SendDlgItemMessage(IDC_USEKD, BM_SETCHECK, BST_CHECKED);
		break;
	case 1:
		SendDlgItemMessage(IDC_USEWINDBG, BM_SETCHECK, BST_CHECKED);
		break;
	case 2:
		SendDlgItemMessage(IDC_USECUSTOM, BM_SETCHECK, BST_CHECKED);
		break;
    case 3:
        SendDlgItemMessage(IDC_USEWINDBGPREVIEW, BM_SETCHECK, BST_CHECKED);
        GetDlgItem(IDC_BREAKIN).EnableWindow(FALSE);
        GetDlgItem(IDC_DBGPATH).EnableWindow(m_Params.ToolsPath.empty() ? TRUE : FALSE);
        break;
	}

	OnDebuggerPathChanged();

	SetTimer(0, 200);

	CFont font((HFONT)GetStockObject(SYSTEM_FONT));
	LOGFONT logFont;
	font.GetLogFont(logFont);
	_tcsncpy(logFont.lfFaceName, _T("Courier New"), __countof(logFont.lfFaceName));
	HFONT hFont = CreateFontIndirect(&logFont);
	GetDlgItem(IDC_EDIT1).SetFont(hFont, TRUE);

	SetDlgItemText(IDC_DBGTEMPLATE, m_Params.CustomDebuggerTemplate.c_str());
	UpdateParamControls();

	m_StatsView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_StatsView.AddColumn(_T("Parameter"), 0);
	m_StatsView.AddColumn(_T("Value"), 1);

	int i = 0;
	m_StatsView.InsertItem(i++, _T("Bytes received"));
	m_StatsView.InsertItem(i++, _T("Bytes send"));
	m_StatsView.InsertItem(i++, _T("Packets received"));
	m_StatsView.InsertItem(i++, _T("Packets sent"));
	m_StatsView.InsertItem(i++, _T("IN packet rate"));
	m_StatsView.InsertItem(i++, _T("OUT packet rate"));
	m_StatsView.InsertItem(i++, _T("Reset count"));
	m_StatsView.InsertItem(i++, _T("Send rate"));
	m_StatsView.InsertItem(i++, _T("Receive rate"));
	m_StatsView.InsertItem(i++, _T("Max. send rate"));
	m_StatsView.InsertItem(i++, _T("Max. recv. rate"));
	m_StatsView.InsertItem(i++, _T("CPU usage"));
	m_StatsView.InsertItem(i++, _T("Avg. CPU usage"));

	m_StatsView.SetColumnWidth(0, 100);
	m_StatsView.SetColumnWidth(1, 65);

#ifdef KDVMWARE_USE_PROXY
	SetWindowText(_T("Virtual Machine monitor (proxy mode)"));
#endif

	if (BazisLib::WOW64APIProvider::sIsWow64Process())
	{
		SetWindowText(_T("VirutalKD-Redux Virtual Machine monitor (32-bit version on 64-bit windows). Use VMMON64.EXE instead."));
	}

	return bHandled = FALSE;
}

void CMainDlg::UpdateParamControls()
{
	GetDlgItem(IDC_DBGTEMPLATE).EnableWindow((m_Params.DebuggerType == 2));
}

void CMainDlg::DebuggerTypeUnCheckHelper()
{
    std::vector<std::pair<int, int>> vec{ {0, IDC_USEKD}, {1, IDC_USEWINDBG}, {2, IDC_USECUSTOM}, {3, IDC_USEWINDBGPREVIEW} };

    for (const auto& e : vec)
    {
        if (e.first != m_Params.DebuggerType)
        {
            BOOL bPreviouslyEnabledState = !GetDlgItem(e.second).EnableWindow();
            SendDlgItemMessage(e.second, BM_SETCHECK, BST_UNCHECKED);
            GetDlgItem(e.second).EnableWindow(bPreviouslyEnabledState);
        }
    }
}

LRESULT CMainDlg::OnParamsChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Params.AutoInvokeDebugger = ((SendDlgItemMessage(IDC_STARTDBG,  BM_GETCHECK) & BST_CHECKED) != 0);
	m_Params.AutoCloseDebugger = ((SendDlgItemMessage(IDC_STOPDBG,  BM_GETCHECK) & BST_CHECKED) != 0);
	m_Params.InitialBreakIn = ((SendDlgItemMessage(IDC_BREAKIN,  BM_GETCHECK) & BST_CHECKED) != 0);
    if ((SendDlgItemMessage(IDC_USEKD, BM_GETCHECK) & BST_CHECKED) != 0)
        m_Params.DebuggerType = 0;
    else if ((SendDlgItemMessage(IDC_USEWINDBG, BM_GETCHECK) & BST_CHECKED) != 0)
        m_Params.DebuggerType = 1;
    else if ((SendDlgItemMessage(IDC_USECUSTOM, BM_GETCHECK) & BST_CHECKED) != 0)
        m_Params.DebuggerType = 2;
    if ((SendDlgItemMessage(IDC_USEWINDBGPREVIEW, BM_GETCHECK) & BST_CHECKED) != 0)
    {
        m_Params.DebuggerType = 3;
        SendDlgItemMessage(IDC_BREAKIN, BM_SETCHECK, BST_UNCHECKED);
        GetDlgItem(IDC_BREAKIN).EnableWindow(FALSE);
        GetDlgItem(IDC_DBGPATH).EnableWindow(m_Params.ToolsPath.empty() ? TRUE : FALSE);
    }
    else
    {
        GetDlgItem(IDC_BREAKIN).EnableWindow();
        GetDlgItem(IDC_DBGPATH).EnableWindow();
    }

    DebuggerTypeUnCheckHelper();

	TCHAR tsz[MAX_PATH * 2];
	GetDlgItemText(IDC_DBGTEMPLATE, tsz, __countof(tsz));
	m_Params.CustomDebuggerTemplate = tsz;
	UpdateParamControls();
	SaveParamsToRegistry();
	return 0;
}

void CMainDlg::SaveParamsToRegistry()
{
	RegistryKey key(HKEY_LOCAL_MACHINE, tszRegistryPath);
	BazisLib::Win32::RegistrySerializer::Serialize(key, m_Params);
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SearchForNewProcesses();
	UpdateProcessInfo();

	if (m_pCallServerContext)
		ProcessRemoteCallServerMessages(m_pCallServerContext);

	return 0;
}

static inline bool UpdateProcessStats(PatchedProcess &proc, unsigned PID, unsigned ProcessorCount)
{
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, PID);
	if (hProc == INVALID_HANDLE_VALUE)
	{
		proc.StillRunning = false;
		return false;
	}
	DateTime now = DateTime::Now();
	FILETIME StartTime = {0,}, EndTime = {0,};
	FILETIME KernelTime = {0,}, UserTime = {0,};
	GetProcessTimes(hProc, &StartTime, &EndTime, &KernelTime, &UserTime);
	if (WaitForSingleObject(hProc, 0) == WAIT_OBJECT_0)
	{
		proc.StillRunning = false;
		proc.StopTime = EndTime;
	}
	else
	{
		proc.StillRunning = true;
		proc.StopTime = now;
	}
	ULONGLONG sum = (*((ULONGLONG*)&KernelTime) + *((ULONGLONG*)&UserTime));
	FILETIME CPUTime = *((FILETIME *)&sum);
	CloseHandle(hProc);
	proc.PID = PID;
	proc.StartTime = StartTime;

	ULONGLONG timeReal = (now - proc.PreviousSampleTime).GetTotalMilliseconds();
	ULONGLONG timeSinceStart = (now - proc.StartTime).GetTotalMilliseconds();
	if (!timeReal)
		timeReal++;
	if (proc.CpuUsage != -1)
	{
		ULONGLONG timeCPU = (DateTime(CPUTime) - proc.PreviousCPUTime).GetTotalMilliseconds();
		proc.CpuUsage = ((int)((timeCPU * 1000) / timeReal)) / ProcessorCount;
	}
	else
		proc.CpuUsage = 0;

	proc.AvgCpuUsage = (unsigned)((TimeSpan(sum).GetTotalMilliseconds() * 1000) / (timeSinceStart * ProcessorCount));
	if (proc.pStatus)
	{
		if (proc.PrevRecvd > proc.pStatus->BytesReceived)
			proc.PrevRecvd = proc.pStatus->BytesReceived;
		if (proc.PrevPolls > proc.pStatus->PacketPollCallsServiced)
			proc.PrevPolls = proc.pStatus->PacketPollCallsServiced;
		if (proc.PrevSent > proc.pStatus->BytesSent)
			proc.PrevSent = proc.pStatus->BytesSent;
		if (proc.PrevPacketsRecvd > proc.pStatus->PacketsReceived)
			proc.PrevPacketsRecvd = proc.pStatus->PacketsReceived;
		if (proc.PrevPacketsSent > proc.pStatus->PacketsSent)
			proc.PrevPacketsSent = proc.pStatus->PacketsSent;

		if (timeReal > 10)
		{
			if (proc.PrevRecvd)
				proc.RecvRate = (unsigned)(((ULONGLONG)(proc.pStatus->BytesReceived - proc.PrevRecvd) * 1000) / timeReal);
			if (proc.PrevSent)
				proc.SendRate = (unsigned)(((ULONGLONG)(proc.pStatus->BytesSent- proc.PrevSent) * 1000) / timeReal);

			if (proc.PrevPolls)
				proc.PollRate = (unsigned)(((ULONGLONG)(proc.pStatus->PacketPollCallsServiced - proc.PrevPolls) * 1000) / timeReal);

			if (proc.PrevPacketsRecvd)
				proc.PacketRecvRate = (unsigned)(((ULONGLONG)(proc.pStatus->PacketsReceived - proc.PrevPacketsRecvd) * 1000) / timeReal);
			if (proc.PrevPacketsSent)
				proc.PacketSendRate = (unsigned)(((ULONGLONG)(proc.pStatus->PacketsSent - proc.PrevPacketsSent) * 1000) / timeReal);
		}

		proc.PrevRecvd = proc.pStatus->BytesReceived;
		proc.PrevSent = proc.pStatus->BytesSent;
		proc.PrevPolls = proc.pStatus->PacketPollCallsServiced;

		proc.PrevPacketsRecvd = proc.pStatus->PacketsReceived;
		proc.PrevPacketsSent = proc.pStatus->PacketsSent;

		if (proc.RecvRate > proc.MaxRecvRate)
			proc.MaxRecvRate = proc.RecvRate;
		if (proc.SendRate > proc.MaxSendRate)
			proc.MaxSendRate = proc.SendRate;
	}

	proc.PreviousSampleTime = now;
	proc.PreviousCPUTime = CPUTime;
	return true;
}

void CMainDlg::SearchForNewProcesses()
{
	HANDLE hList = CreateVMSessionList();
	unsigned PID = 0;
	VMType vmType = kVMUnknown;
	
	while (PID = GetNextVMSessionPIDEx(hList, &vmType))
	{
		if (m_PatchedSet.find(PID) == m_PatchedSet.end())
		{
			PatchedProcess proc;
			proc.State = PatchPending;
			proc.vmType = vmType;
			if (UpdateProcessStats(proc, PID, m_ProcessorCount))
			{
				m_PatchedSet.insert(PID);
				TCHAR tsz[32];
				_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d"), proc.PID);
				if (IsVMSessionPatched(PID))
				{
					proc.State = PatchSuccessful;
					CreateStatusMapping(proc, false);
				}
				wchar_t szName[MAX_PATH] = {0,};
				GetVMSessionNameW(PID, szName, _countof(szName));
				if (szName[0])
					proc.SessionNameRetreived = true;
				m_Processes.push_back(proc);
				size_t idx = m_Processes.size() - 1;

				m_ListView.InsertItem((UINT)idx, tsz, ProcessPending); 
				m_ListView.SetItem((UINT)idx, 1, LVIF_TEXT, GetVMTypeName(proc.vmType), 0, 0, 0, 0);
			}
		}
	};

	if (m_ListView.GetItemCount() && !m_bValidVMsWereFound)
	{
		m_bValidVMsWereFound = true;
		m_ListView.SelectItem(0);
	}

	CloseVMSessionList(hList);
}

static BOOL CALLBACK CloseEnumWndProc(HWND hWnd, LPARAM lParam)
{
	DWORD dwProcessID = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessID);
	if (dwProcessID == lParam)
	{
		TCHAR tsz[512] = {0,};
		RealGetWindowClass(hWnd, tsz, __countof(tsz));
		if (!_tcscmp(tsz, _T("WinDbgFrameClass")) || !_tcscmp(tsz, _T("ConsoleWindowClass")))
		{
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
			return FALSE;
		}
	}
	return TRUE;
}

static bool CloseDebugger(unsigned PID)
{
	if (!EnumWindows(CloseEnumWndProc, PID))
		return true;
	return false;
}	

void CMainDlg::CleanupProcessEntry(PatchedProcess &proc)
{
	if (proc.hLogPipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(proc.hLogPipe);
		proc.hLogPipe = INVALID_HANDLE_VALUE;
	}
	if (proc.hMapping != INVALID_HANDLE_VALUE)
	{
		if (proc.pStatus)
		{
			UnmapViewOfFile(proc.pStatus);
			proc.pStatus = NULL;
		}
		CloseHandle(proc.hMapping);
		proc.hMapping = INVALID_HANDLE_VALUE;
	}
}

void CMainDlg::CreateStatusMapping(PatchedProcess &proc, bool Reset)
{
	TCHAR tszMappingName[MAX_PATH];
	_sntprintf_s(tszMappingName, __countof(tszMappingName), _TRUNCATE, tszMappingNameFormat, proc.PID);
	PermissiveSecurityDescriptor descriptor;
	proc.hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, descriptor, PAGE_READWRITE, 0, sizeof(KdClientStatus), tszMappingName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		Reset = false;
	if (proc.hMapping != INVALID_HANDLE_VALUE)
	{
		proc.pStatus = (KdClientStatus *)MapViewOfFile(proc.hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (!proc.pStatus)
		{
			CloseHandle(proc.hMapping);
			proc.hMapping = INVALID_HANDLE_VALUE;
		}
		else
		{
			if (Reset)
				memset(proc.pStatus, 0, sizeof(KdClientStatus));
			proc.pStatus->DebugLevel = m_Params.DebugLevel;
			proc.pStatus->LogAllPackets = ((SendDlgItemMessage(IDC_LOGPACKETS,  BM_GETCHECK) & BST_CHECKED) != 0);
		}
	}
	if (proc.hMapping == INVALID_HANDLE_VALUE)
	{
		MessageBox(MAKE_STATUS(ActionStatus::FromLastError()).GetErrorText().c_str(),
					_T("Cannot create shared parameter block"),
					MB_ICONERROR);
	}
}

bool CMainDlg::InitiatePatching(PatchedProcess &proc)
{
	CreateStatusMapping(proc, true);
	if (proc.hLogPipe == INVALID_HANDLE_VALUE)
		proc.hLogPipe = CreateLogPipe(proc.PID);
	proc.hRemoteThread = StartVMSessionPatching(proc.PID);
	if (!proc.hRemoteThread)
	{
		proc.State = PatchSuccessful;
		return true;
	}
	else if (proc.hRemoteThread == INVALID_HANDLE_VALUE)
	{
		proc.State = PatchFailed;
		return false;
	}
	else
	{
		proc.State = PatchInProgress;
		return true;
	}
}

void CMainDlg::UpdateLog(PatchedProcess &proc)
{
	if (proc.hLogPipe != INVALID_HANDLE_VALUE)
	{
		DWORD bytesAvailable = 0;
		PeekNamedPipe(proc.hLogPipe, NULL, 0, NULL, &bytesAvailable, NULL);
		if (bytesAvailable)
		{
			CBuffer buf;
			buf.EnsureSize(bytesAvailable);
			DWORD dwOk = 0;
			ReadFile(proc.hLogPipe, buf.GetData(), bytesAvailable, &dwOk, NULL);
			if (dwOk == bytesAvailable)
			{
				proc.Log.append((TCHAR *)buf.GetData(), bytesAvailable / sizeof(TCHAR));
				proc.LogChanged = true;
			}
		}
	}
}

void CMainDlg::PerformProcessActions(PatchedProcess &proc, TimeSpan &runTime, bool *pPatchingStarted)
{
	if ((proc.State == PatchProcessTerminated) || (proc.State == Unpatched))
		CleanupProcessEntry(proc);
	else
	{
		if (proc.hLogPipe == INVALID_HANDLE_VALUE)
			proc.hLogPipe = CreateLogPipe(proc.PID);
	}

	if (proc.State == PatchPending)
	{
		if (runTime.GetTotalSeconds() > m_Params.PatchDelay)
		{
			bool started = InitiatePatching(proc);
			if (pPatchingStarted)
				*pPatchingStarted = started;
		}
	}

	if (proc.State == PatchInProgress)
	{
		if (WaitForSingleObject(proc.hRemoteThread, 0) == WAIT_OBJECT_0)
		{
			DWORD dwCode = 0;
			GetExitCodeThread(proc.hRemoteThread, &dwCode);
			if (dwCode)
				proc.State = PatchInProgress2;
			else
				proc.State = PatchFailed;
			CloseHandle(proc.hRemoteThread);
			proc.hRemoteThread = INVALID_HANDLE_VALUE;
		}
	}

	if ((proc.State == PatchInProgress2) || (proc.State == PatchSuccessful))
	{
		if (proc.pStatus)
		{
			switch (proc.pStatus->PatchErrorPlus1 - 1)
			{
			case ERROR_SUCCESS:
				proc.State = PatchSuccessful;
				break;
			case -1:
				proc.State = PatchInProgress2;
				break;
			default:
				proc.State = PatchFailed;
			}
		}
		if (proc.pStatus->ProtocolMismatchStatus)
		{
			proc.State = ProtocolMismatch;
			WORD expected = HIWORD(proc.pStatus->ProtocolMismatchStatus), found = LOWORD(proc.pStatus->ProtocolMismatchStatus);
			proc.pStatus->ProtocolMismatchStatus = 0;
			TCHAR tsz[256];
			_sntprintf(tsz, __countof(tsz), _T("Warning! KDBAZIS.DLL version %x.%02x was loaded by virtual machine, while version %x.%02x was expected. Debugging functions disabled!"),
				HIBYTE(found), LOBYTE(found), HIBYTE(expected), LOBYTE(expected));
			MessageBox(tsz, _T("Invalid KDBAZIS.DLL version"), MB_ICONWARNING);
		}
	}

	if (proc.State == UnpatchInProgress)
	{
		if (WaitForSingleObject(proc.hRemoteThread, 0) == WAIT_OBJECT_0)
		{
			CloseHandle(proc.hRemoteThread);
			proc.hRemoteThread = INVALID_HANDLE_VALUE;
			proc.State = Unpatched;
			UpdateLog(proc);
			CleanupProcessEntry(proc);
		}
	}

	UpdateLog(proc);
}

void CMainDlg::UpdateProcessInfo()
{
	DateTime now = DateTime::Now();
	int idx = m_ListView.GetSelectedIndex();
	for (unsigned i = 0; i < (unsigned)m_Processes.size(); i++)
	{
		PatchedProcess &proc = m_Processes[i];
		if (proc.StillRunning)
			UpdateProcessStats(proc, proc.PID, m_ProcessorCount);

		if (!proc.StillRunning)
			proc.State = PatchProcessTerminated;
		TimeSpan runTime = now - proc.StartTime;
		if (!proc.StillRunning)
			runTime = proc.StopTime - proc.StartTime;
		m_ListView.AddItem(i, 2, runTime.ToString().c_str());
		TCHAR tsz[128];
		_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d%%"), proc.CpuUsage / 10);
		m_ListView.AddItem(i, 3, tsz);
		bool patchStarted = false;
		PerformProcessActions(proc, runTime, &patchStarted);

		if (patchStarted)
			m_ListView.SelectItem(i), idx = i;

		if (idx == i)
		{
			if (proc.LogChanged)
			{
				SetDlgItemText(IDC_EDIT1, proc.Log.c_str());
				SendDlgItemMessage(IDC_EDIT1, WM_VSCROLL, SB_BOTTOM, 0);
				proc.LogChanged = false;
			}
			if (proc.State == PatchSuccessful)
			{
				m_StatsView.EnableWindow(TRUE);
				DisplayStats(proc);
			}
			else
				m_StatsView.EnableWindow(FALSE);
		}

		switch (proc.State)
		{
		case PatchInProgress:
		case PatchInProgress2:
			m_ListView.SetItem(i, 0, LVIF_IMAGE, NULL, ProcessPending, 0, 0, 0);
			m_ListView.AddItem(i, 4, _T("loading..."));
			break;
		case UnpatchInProgress:
			m_ListView.SetItem(i, 0, LVIF_IMAGE, NULL, ProcessPending, 0, 0, 0);
			m_ListView.AddItem(i, 4, _T("unloading..."));
			break;
		case PatchPending:
			m_ListView.SetItem(i, 0, LVIF_IMAGE, NULL, ProcessPending, 0, 0, 0);
			m_ListView.AddItem(i, 4, _T(""));
			break;
		case PatchSuccessful:
			m_ListView.SetItem(i, 0, LVIF_IMAGE, NULL, ProcessPatched, 0, 0, 0);
			if (proc.pStatus)
			{
				const KdClientStatus &status = *proc.pStatus;
				m_ListView.AddItem(i, 4, status.PipeName + 9);
				proc.PipeName = status.PipeName;

#ifdef _DEBUG
				{
					wchar_t wszT[MAX_PATH] = {0,};
					GetVMPipeNameW(proc.PID, wszT, __countof(wszT), true);
					ASSERT(!wcscmp(wszT, status.PipeName));
				}
#endif

				_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d/%d"), status.PacketsSent, status.PacketsReceived);
				m_ListView.AddItem(i, 5, tsz);
				_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d"), status.ResyncCount);
				m_ListView.AddItem(i, 6, tsz);
				m_ListView.AddItem(i, 7, status.OSDetected ? _T("yes") : _T(""));
				m_ListView.AddItem(i, 8, status.DebuggerConnected ? _T("yes") : _T(""));
				_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d"), proc.PollRate);
				m_ListView.AddItem(i, 9, tsz);
				if (!proc.idDebuggerProcess)
				{
					if ((m_Params.AutoInvokeDebugger && status.PipeName[0]) && (!m_Params.WaitForOS || status.OSDetected))
					{
						RunDebugger(i);
					}
				}
			}
			else
			{
				m_ListView.AddItem(i, 4, _T("loading..."));
				m_ListView.AddItem(i, 7, _T(""));
				m_ListView.AddItem(i, 8, _T(""));
			}
			break;
		case PatchFailed:
			m_ListView.SetItem(i, 0, LVIF_IMAGE, NULL, ProcessFailed, 0, 0, 0);
			m_ListView.AddItem(i, 4, _T(""));
			break;
		case ProtocolMismatch:
			m_ListView.SetItem(i, 0, LVIF_IMAGE, NULL, ProcessFailed, 0, 0, 0);
			m_ListView.AddItem(i, 4, _T("Old KDBAZIS.DLL/KDVM.DLL loaded in guest"));
			break;
		case PatchProcessTerminated:
		case Unpatched:
			m_ListView.SetItem(i, 0, LVIF_IMAGE, NULL, ProcessInactive, 0, 0, 0);
			m_ListView.AddItem(i, 4, (proc.State == Unpatched) ? _T("(KDCLIENT.DLL unloaded)") : _T("(terminated)"));
			if (proc.idDebuggerProcess)
			{
				if (m_Params.AutoCloseDebugger)
				{
					if (CloseDebugger(proc.idDebuggerProcess))
						proc.idDebuggerProcess = 0;
				}
			}
			break;
		}
		if (proc.State != proc.PreviousState)
		{
			if (idx == i)
				UpdateUnpatchButton(i);
			proc.PreviousState = proc.State;
		}
		if ((proc.State != PatchProcessTerminated) && !proc.SessionNameRetreived)
		{
			wchar_t szName[MAX_PATH] = {0,};
			GetVMSessionNameW(proc.PID, szName, _countof(szName));
//			m_ListView.SetItem((UINT)i, 1, LVIF_TEXT, szName, 0, 0, 0, 0);
			if (szName[0])
				proc.SessionNameRetreived = true;
		}
	}
}

static inline void FindAndReplace(String &src, const String& find, const String& replace) 
{ 
	size_t pos; 
	for (;(pos = src.find( find )) != std::string::npos;) 
	{ 
		src.replace( pos, find.length(), replace ); 
	} 
}

static BOOL IsWindbgPreviewInstalled()
{
    SHELLEXECUTEINFOW exInfo = { 0 };
    exInfo.cbSize = sizeof(exInfo);
    exInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    exInfo.nShow = SW_HIDE;
    exInfo.lpVerb = L"open";
    exInfo.lpFile = L"WindbgX.exe";
    
    if (ShellExecuteExW(&exInfo) == TRUE)
    {
        TerminateProcess(exInfo.hProcess, 0);
        CloseHandle(exInfo.hProcess);
        return TRUE;
    }

    return FALSE;
}

void CMainDlg::RunDebugger(unsigned entryIndex)
{
    PatchedProcess &proc = m_Processes[entryIndex];
    if (proc.PipeName.empty() || proc.idDebuggerProcess == ~0UL)
        return;
    String debugger;
    TCHAR tszCmdLine[MAX_PATH * 2] = { 0, };
    proc.idDebuggerProcess = ~0UL;
    if (m_Params.DebuggerType == 0 || m_Params.DebuggerType == 1)
    {
        debugger = Path::Combine(m_DbgToolsPath, m_Params.DebuggerType ? _T("windbg.exe") : _T("kd.exe"));
        LPCTSTR lpFormat = _T("\"%s\" %s-k com:pipe,resets=0,reconnect,port=%s");
        _sntprintf_s(tszCmdLine, __countof(tszCmdLine), _TRUNCATE, lpFormat, debugger.c_str(), m_Params.InitialBreakIn ? _T("-b ") : _T(""), proc.PipeName.c_str());
    }
    else if (m_Params.DebuggerType == 2)
    {
        String cmdLine = m_Params.CustomDebuggerTemplate;
        FindAndReplace(cmdLine, _T("$(toolspath)"), m_DbgToolsPath);
        FindAndReplace(cmdLine, _T("$(pipename)"), proc.PipeName.c_str());
        _tcsncpy(tszCmdLine, cmdLine.c_str(), __countof(tszCmdLine));
    }
    else if (m_Params.DebuggerType == 3)
    {
        LPCTSTR lpFormat = _T("-k com:pipe,resets=0,reconnect,port=%s");
        _sntprintf_s(tszCmdLine, __countof(tszCmdLine), _TRUNCATE, lpFormat, proc.PipeName.c_str());

        SHELLEXECUTEINFOW exInfo = { 0 };
        exInfo.cbSize = sizeof(exInfo);
        exInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
        exInfo.lpVerb = L"open";
        exInfo.lpFile = L"WindbgX.exe";
        exInfo.lpParameters = tszCmdLine;
        if (ShellExecuteExW(&exInfo) == TRUE)
        {
            proc.idDebuggerProcess = GetProcessId(exInfo.hProcess);
            CloseHandle(exInfo.hProcess);
        }
        else
        {
            proc.idDebuggerProcess = 0;
        }
        return;
    }
    else
    {
        return;
    }

    STARTUPINFO startupInfo = { 0, };
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInfo = { 0, };
    CreateProcess(debugger.empty() ? NULL : debugger.c_str(), tszCmdLine, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP, NULL, m_DbgToolsPath.c_str(), &startupInfo, &processInfo);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    proc.idDebuggerProcess = processInfo.dwProcessId;
}


LRESULT CMainDlg::OnRunDebugger(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_ListView.GetSelectedIndex();
	if ((idx != -1) && ((size_t)idx < m_Processes.size()))
		RunDebugger(idx);
	else
		MessageBox(_T("No Virtual Machine selected"), _T("Error"), MB_ICONERROR);
	return 0;
}

LRESULT CMainDlg::OnSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	int idx = m_ListView.GetSelectedIndex();
	if (idx != -1)
	{
		PatchedProcess &proc = m_Processes[idx];
		SetDlgItemText(IDC_EDIT1, proc.Log.c_str());
		proc.LogChanged = false;
		if (proc.pStatus && proc.pStatus->DebugLevel)
			m_DebugLevel.SetCurSel(proc.pStatus->DebugLevel);
		UpdateUnpatchButton(idx);

		if (proc.pStatus)
		{
			if (proc.pStatus->LogAllPackets)
				SendDlgItemMessage(IDC_LOGPACKETS, BM_SETCHECK, BST_CHECKED);
			else
				SendDlgItemMessage(IDC_LOGPACKETS, BM_SETCHECK, BST_UNCHECKED);
		}

		DisplayStats(proc);
	}
	else
		m_StatsView.EnableWindow(FALSE);
	return 0;
}

LRESULT CMainDlg::OnDebugLevelChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Params.DebugLevel = m_DebugLevel.GetCurSel();
	int idx = m_ListView.GetSelectedIndex();
	if (idx != -1)
	{
		PatchedProcess &proc = m_Processes[idx];
		if ((proc.State == PatchSuccessful) && (proc.pStatus))
			proc.pStatus->DebugLevel = m_Params.DebugLevel;
	}
	SaveParamsToRegistry();
	return 0;
}

LRESULT CMainDlg::OnBnClickedClearlog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_ListView.GetSelectedIndex();
	if (idx != -1)
	{
		PatchedProcess &proc = m_Processes[idx];
		proc.Log.clear();
		SetDlgItemText(IDC_EDIT1, proc.Log.c_str());
		proc.LogChanged = false;
	}
	return 0;
}


LRESULT CMainDlg::OnBnClickedClearVMs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	for (unsigned i = 0; i < (unsigned)m_Processes.size(); i++)
	{
		if (m_Processes[i].State == PatchProcessTerminated)
		{
			CleanupProcessEntry(m_Processes[i]);
			m_Processes.erase(m_Processes.begin() + i);
			m_ListView.DeleteItem(i);
			i--;
		}
	}
	return 0;
}

LRESULT CMainDlg::OnBnClickedUnpatch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_ListView.GetSelectedIndex();
	if (idx != -1)
	{
		PatchedProcess &proc = m_Processes[idx];
		if (proc.State == PatchSuccessful)
		{
			proc.hRemoteThread = (HANDLE)StartVMSessionUnpatching(proc.PID);
			if (proc.hRemoteThread == INVALID_HANDLE_VALUE)
				MessageBox(_T("Cannot unpatch process"), _T("Error"), MB_ICONERROR);
			else
			{
				if (proc.hRemoteThread)
					proc.State = UnpatchInProgress;
				else
					proc.State = Unpatched;
			}
		}
		else if (proc.State == Unpatched)
		{
			proc.State = PatchPending;
		}
		UpdateUnpatchButton(idx);
	}
	return 0;
}

void CMainDlg::UpdateUnpatchButton(int SelectionIndex)
{
	if (SelectionIndex == -1)
		return;
	PatchedProcess &proc = m_Processes[SelectionIndex];
	switch (proc.State)
	{
	case PatchSuccessful:
		SetDlgItemText(IDC_UNPATCH, _T("Unpatch process"));
		GetDlgItem(IDC_UNPATCH).EnableWindow();
		break;
	case Unpatched:
		SetDlgItemText(IDC_UNPATCH, _T("Repatch process"));
		GetDlgItem(IDC_UNPATCH).EnableWindow();
		break;
	default:
		GetDlgItem(IDC_UNPATCH).EnableWindow(FALSE);
		break;
	}

}

void CMainDlg::OnDebuggerPathChanged()
{
	bool bDbgToolsPath = !m_DbgToolsPath.empty();
    bool bWindbgPreview = IsWindbgPreviewInstalled();
	GetDlgItem(IDC_STARTDBG).EnableWindow(bDbgToolsPath || bWindbgPreview);
	GetDlgItem(IDC_STOPDBG).EnableWindow(bDbgToolsPath || bWindbgPreview);
	GetDlgItem(IDC_USEKD).EnableWindow(bDbgToolsPath);
	GetDlgItem(IDC_USEWINDBG).EnableWindow(bDbgToolsPath);
    GetDlgItem(IDC_USEWINDBGPREVIEW).EnableWindow(bWindbgPreview);
	GetDlgItem(IDC_RUNDBG).EnableWindow(bDbgToolsPath || bWindbgPreview);
	GetDlgItem(IDC_USECUSTOM).EnableWindow(bDbgToolsPath);
}

LRESULT CMainDlg::OnBnClickedDbgpath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR *pFileName = NULL;
	String fileName;
	if (!m_DbgToolsPath.empty())
	{
		fileName = Path::Combine(m_DbgToolsPath, _T("windbg.exe"));
		pFileName = fileName.c_str();
	}
	CFileDialog dlg(TRUE, _T("exe"), pFileName, OFN_HIDEREADONLY, _T("Debugging tools\0windbg.exe;kd.exe\0Executable files (*.exe)\0*.exe\0All files(*.*)\0*.*\0\0"));
	if (dlg.DoModal() == IDOK)
	{
		String dbgPath = dlg.m_szFileName;
		dbgPath = Path::GetDirectoryName(dbgPath);
		if (File::Exists(Path::Combine(dbgPath, _T("windbg.exe"))))
		{
			m_DbgToolsPath = dbgPath;
			m_Params.ToolsPath = m_DbgToolsPath;
			SaveParamsToRegistry();
		}
		OnDebuggerPathChanged();
	}
	return 0;
}

void CMainDlg::DisplayStats(PatchedProcess &proc)
{
	TCHAR tsz[512];
	if (!proc.pStatus)
	{
		m_StatsView.EnableWindow(FALSE);
		return;
	}
	int i = 0;
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, RateCalculator::FormatByteCount(proc.pStatus->BytesSent).c_str(), 0, 0, 0, 0);
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, RateCalculator::FormatByteCount(proc.pStatus->BytesReceived).c_str(), 0, 0, 0, 0);
	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d"), proc.pStatus->PacketsSent);
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);
	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d"), proc.pStatus->PacketsReceived);
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);
	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%s/s"), RateCalculator::FormatByteCount(proc.PacketSendRate).c_str());
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);
	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%s/s"), RateCalculator::FormatByteCount(proc.PacketRecvRate).c_str());
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);

	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d"), proc.pStatus->ResyncCount);
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);

	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%s/s"), RateCalculator::FormatByteCount(proc.RecvRate).c_str());
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);
	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%s/s"), RateCalculator::FormatByteCount(proc.SendRate).c_str());
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);
	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%s/s"), RateCalculator::FormatByteCount(proc.MaxRecvRate).c_str());
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);
	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%s/s"), RateCalculator::FormatByteCount(proc.MaxSendRate).c_str());
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);

	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d%%"), proc.CpuUsage / 10);
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);

	_sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("%d%%"), proc.AvgCpuUsage / 10);
	m_StatsView.SetItem(i++, 1, LVIF_TEXT, tsz, 0, 0, 0, 0);

}

LRESULT CMainDlg::OnBnClickedTraceassist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTraceParamsDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		for (size_t i = 0; i < m_Processes.size(); i++)
		{
			PatchedProcess &proc = m_Processes[i];
			if (proc.pStatus)
				proc.pStatus->TraceAssistUpdatePending = true;
		}
	}
	return 0;
}

LRESULT CMainDlg::OnLogPacketsChanged( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
	bool bLogPackets = ((SendDlgItemMessage(IDC_LOGPACKETS,  BM_GETCHECK) & BST_CHECKED) != 0);
	int idx = m_ListView.GetSelectedIndex();
	if (idx != -1)
	{
		PatchedProcess &proc = m_Processes[idx];
		if ((proc.State == PatchSuccessful) && (proc.pStatus))
			proc.pStatus->LogAllPackets = bLogPackets;
	}
	return 0;
}

CMainDlg::~CMainDlg()
{
	if (m_pCallServerContext)
		EndRemoteCallServer(m_pCallServerContext);
}

LRESULT CMainDlg::OnBnClickedRevertsnapshot(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_ListView.GetSelectedIndex();
	if ((idx == -1) || ((size_t)idx >= m_Processes.size()))
		return 0;
	PatchedProcess &proc = m_Processes[idx];
	if (proc.PipeName.empty())
		return 0;

	if (!RevertVMToLastSnapshotByPipeNameW(proc.PipeName.c_str()))
		MessageBox(_T("Cannot revert VM to latest snapshot. Check KDCLIENT(64).DLL version and snapshot tree."), NULL, MB_ICONERROR);
	return 0;
}

LRESULT CMainDlg::OnBnClickedInstantbreak(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_ListView.GetSelectedIndex();
	if ((idx == -1) || ((size_t)idx >= m_Processes.size()))
		return 0;
	PatchedProcess &proc = m_Processes[idx];
	if (proc.PipeName.empty())
		return 0;

	if (!ForceInstantBreakInByPipeNameW(proc.PipeName.c_str()))
		MessageBox(_T("Cannot force break-in. Check KDCLIENT(64).DLL version."), NULL, MB_ICONERROR);
	return 0;
}
