/*! \file trparams.h
    \author Ivan Shcherbakov (Bazis)
    $Id: trparams.h,v 1.4 2008-12-31 13:25:05 Bazis Exp $
    \brief Contains inline TraceAssist parameters dialog
*/

#pragma once
#include "rpcdispatch/trassist.h"
#include <BazisLib/bzshlp/Win32/RegistrySerializer.h>

//! TraceAssist settings dialog
class CTraceParamsDlg : public CDialogImpl<CTraceParamsDlg>
{
private:
	TraceAssistParams m_Params;

public:
	enum { IDD = IDD_TRACEPARAMS};

	CTraceParamsDlg()
	{
	}

#pragma region Message maps

	BEGIN_MSG_MAP(CTraceParamsDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_ENABLETRACE, OnParamsChanged);
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnOK);
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel);
	END_MSG_MAP()


#pragma endregion
#pragma region Message handlers

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		BazisLib::RegistryKey key(HKEY_LOCAL_MACHINE, tszTraceAssistRegPath);
		BazisLib::Win32::RegistrySerializer::Deserialize(key, m_Params);
		SendDlgItemMessage(IDC_ENABLETRACE, BM_SETCHECK, m_Params.TraceAssistEnabled ? BST_CHECKED : 0);
		SetDlgItemText(IDC_LOGDIR, m_Params.LogFileDirectory.c_str());
		SetDlgItemText(IDC_TRACEPREFIX, m_Params.MessagePrefix.c_str());
		SendDlgItemMessage(IDC_REPLACELOG, BM_SETCHECK, m_Params.OverwriteFileOnStart ? BST_CHECKED : 0);
		return OnParamsChanged(0, 0, 0, bHandled);
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		TCHAR tsz[MAX_PATH];
		m_Params.TraceAssistEnabled = ((SendDlgItemMessage(IDC_ENABLETRACE, BM_GETCHECK) & BST_CHECKED) != 0);
		GetDlgItemText(IDC_LOGDIR, tsz, __countof(tsz));
		m_Params.LogFileDirectory = tsz;
		GetDlgItemText(IDC_TRACEPREFIX, tsz, __countof(tsz));
		m_Params.MessagePrefix = tsz;
		m_Params.OverwriteFileOnStart = ((SendDlgItemMessage(IDC_REPLACELOG, BM_GETCHECK) & BST_CHECKED) != 0);
		BazisLib::RegistryKey key(HKEY_LOCAL_MACHINE, tszTraceAssistRegPath);
		BazisLib::Win32::RegistrySerializer::Serialize(key, m_Params);
		EndDialog(IDOK);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	LRESULT OnParamsChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_Params.TraceAssistEnabled = ((SendDlgItemMessage(IDC_ENABLETRACE, BM_GETCHECK) & BST_CHECKED) != 0);
		GetDlgItem(IDC_LOGDIR).EnableWindow(m_Params.TraceAssistEnabled);
		GetDlgItem(IDC_TRACEPREFIX).EnableWindow(m_Params.TraceAssistEnabled);
		GetDlgItem(IDC_REPLACELOG).EnableWindow(m_Params.TraceAssistEnabled);
		GetDlgItem(IDC_LOGHINT).EnableWindow(m_Params.TraceAssistEnabled);
		return 0;
	}
#pragma endregion

};
