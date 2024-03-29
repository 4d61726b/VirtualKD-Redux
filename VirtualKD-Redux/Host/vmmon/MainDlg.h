/*! \file MainDlg.h
    \author Ivan Shcherbakov (Bazis)
    $Id: MainDlg.h,v 1.11 2009-12-31 13:06:42 Bazis Exp $
    \brief Contains declaration of main VMMON dialog class
*/

#pragma once

#include "patchlist.h"
#include "regconfig.h"
#include <BazisLib/bzscore/file.h>

//! Main VMMON.EXE dialog
class CMainDlg : public CDialogImpl<CMainDlg>, public CDialogResize<CMainDlg>
{
private:
    PatchedProcessList m_Processes;
    PatchedProcessSet m_PatchedSet;

private:
    CListViewCtrl m_ListView, m_StatsView;
    CImageList m_ImageList;

    unsigned m_ProcessorCount;

    MonitorParams m_Params;
    BazisLib::String m_DbgToolsPath;
    BazisLib::String m_SideloadedDbgPreviewPath;

    CComboBox m_DebugLevel;
    PVOID m_pCallServerContext;
    bool m_bValidVMsWereFound;	//First time when a valid VMs are found, the first one is selected in the dialog

    void DebuggerTypeUnCheckHelper(bool bCheckDebuggerType = false);

public:
    enum { IDD = IDD_MAINDLG };

    CMainDlg();
    ~CMainDlg();

#pragma region Message maps

    BEGIN_MSG_MAP(CMainDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        COMMAND_HANDLER(IDC_CLEARLOG, BN_CLICKED, OnBnClickedClearlog)
        COMMAND_HANDLER(IDC_CLEANUPVMS, BN_CLICKED, OnBnClickedClearVMs)
        COMMAND_HANDLER(IDC_UNPATCH, BN_CLICKED, OnBnClickedUnpatch)
        COMMAND_HANDLER(IDC_DBGPATH, BN_CLICKED, OnBnClickedDbgPath)
        COMMAND_HANDLER(IDC_WINDBGPREVIEWPATH, BN_CLICKED, OnBnClickedWindbgPreviewPath)
        COMMAND_HANDLER(IDC_TRACEASSIST, BN_CLICKED, OnBnClickedTraceassist)
        COMMAND_HANDLER(IDC_REVERTSNAPSHOT, BN_CLICKED, OnBnClickedRevertsnapshot)
        COMMAND_HANDLER(IDC_INSTANTBREAK, BN_CLICKED, OnBnClickedInstantbreak)
        CHAIN_MSG_MAP(CDialogResize)
        COMMAND_ID_HANDLER(IDC_RUNDBG, OnRunDebugger);

    COMMAND_ID_HANDLER(IDC_STARTDBG, OnParamsChanged);
    COMMAND_ID_HANDLER(IDC_STOPDBG, OnParamsChanged);
    COMMAND_ID_HANDLER(IDC_USEKD, OnParamsChanged);
    COMMAND_ID_HANDLER(IDC_USEWINDBG, OnParamsChanged);
    COMMAND_ID_HANDLER(IDC_USEWINDBGPREVIEW, OnParamsChanged);
    COMMAND_ID_HANDLER(IDC_USECUSTOM, OnParamsChanged);
    COMMAND_ID_HANDLER(IDC_BREAKIN, OnParamsChanged);
    COMMAND_ID_HANDLER(IDC_LOGPACKETS, OnLogPacketsChanged);
    COMMAND_HANDLER(IDC_DBGTEMPLATE, EN_KILLFOCUS, OnParamsChanged);

    NOTIFY_HANDLER(IDC_LIST1, LVN_ITEMCHANGED, OnSelChanged)
        COMMAND_HANDLER(IDC_DBGLEVEL, CBN_SELCHANGE, OnDebugLevelChanged)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CMainDlg)
        DLGRESIZE_CONTROL(IDC_LIST1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_STARTDBG, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_STOPDBG, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_USEKD, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_USEWINDBG, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_USEWINDBGPREVIEW, DLSZ_MOVE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_WINDBGPREVIEWPATH, DLSZ_MOVE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_RUNDBG, DLSZ_MOVE_Y | DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_REVERTSNAPSHOT, DLSZ_MOVE_Y | DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_INSTANTBREAK, DLSZ_MOVE_Y | DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_EDIT1, DLSZ_MOVE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_DBGMSGTEXT, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_DBGLEVEL, DLSZ_MOVE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_UNPATCH, DLSZ_MOVE_Y | DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_USECUSTOM, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_DBGTEMPLATE, DLSZ_MOVE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_CLEARLOG, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_CLEANUPVMS, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_DBGPATH, DLSZ_MOVE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_STATS, DLSZ_SIZE_Y | DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_MOVE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_TRACEASSIST, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_BREAKIN, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_LOGPACKETS, DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

#pragma endregion
#pragma region Message handlers

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnParamsChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnParamsChanged();
    LRESULT OnLogPacketsChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnRunDebugger(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
    LRESULT OnDebugLevelChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

#pragma endregion

protected:
    void SearchForNewProcesses();
    void UpdateProcessInfo();
    void RunDebugger(unsigned entryIndex);
    void PerformProcessActions(PatchedProcess &proc, BazisLib::TimeSpan &runTime, bool *pPatchingStarted);
    void CleanupProcessEntry(PatchedProcess &proc);
    void SaveParamsToRegistry();

    void CreateStatusMapping(PatchedProcess &proc, bool Reset);
    bool InitiatePatching(PatchedProcess &proc);
    void UpdateLog(PatchedProcess &proc);
    void UpdateUnpatchButton(int SelectionIndex);

    void UpdateParamControls();
    void OnDebuggerPathChanged();
    void DisplayStats(PatchedProcess &proc);
public:
    LRESULT OnBnClickedClearlog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedClearVMs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedUnpatch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedDbgPath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedTraceassist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedRevertsnapshot(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedInstantbreak(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedWindbgPreviewPath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
