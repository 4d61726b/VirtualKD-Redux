// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "resource2.h"

#include <ImageHlp.h>

#include "MainDlg.h"
#include "install.h"
#include "TakeOwnership.h"
#include <BazisLib/bzscore/Win32/security.h>
#include <BazisLib/bzscore/file.h>
#include <BazisLib/bzshlp/Win32/wow64.h>
#include <BazisLib/bzscore/Win32/registry.h>
#include <BazisLib/bzshlp/Win32/filemap.h>

#include "vkdversion.h"

using namespace BazisLib;
using namespace BootEditor;

static PVOID memmem(const PVOID pHaystack, SIZE_T szHaystackLen, const PVOID pNeedle, SIZE_T szNeedleLen)
{
    if (!szNeedleLen || szHaystackLen < szNeedleLen)
    {
        return NULL;
    }

    for (PCHAR pcBegin = (PCHAR)pHaystack, pcLastPossible = (PCHAR)pHaystack + szHaystackLen - szNeedleLen;
        pcBegin <= pcLastPossible;
        ++pcBegin)
    {
        if (pcBegin[0] == ((PCHAR)pNeedle)[0] && !memcmp(&pcBegin[1], ((PCHAR)pNeedle + 1), szNeedleLen - 1))
        {
            return pcBegin;
        }
    }

    return NULL;
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

    bHandled = FALSE;

    ManagedPointer<AIBootConfigurationEntry> pEntry;
    bool bAlreadyInstalled = false;

    ManagedPointer<AIBootConfigurationEditor> pEditor = CreateConfigurationEditor();
    if (!pEditor)
    {
        ::MessageBox(HWND_DESKTOP,
            _T("Cannot access boot configuration data"),
            NULL,
            MB_ICONERROR);
        EndDialog(0);
        return 0;
    }

    ActionStatus st = FindBestOSEntry(&pEntry, &bAlreadyInstalled, pEditor);
    if (!st.Successful())
    {
        ::MessageBox(HWND_DESKTOP,
            String::sFormat(_T("Cannot access boot configuration data: %s"), st.GetMostInformativeText().c_str()).c_str(),
            NULL,
            MB_ICONERROR);
        EndDialog(0);
        return 0;
    }

    m_ExistingEntryName = pEntry->GetDescription();
    SetDlgItemText(IDC_REUSEENTRY, String::sFormat(_T("Use existing entry \"%s\""), m_ExistingEntryName.c_str()).c_str());
    SendDlgItemMessage(IDC_SETDEFAULT, BM_SETCHECK, BST_CHECKED);

    if (IsWin10OrLater())
    {
        SendDlgItemMessage(IDC_KDCOM, BM_SETCHECK, BST_CHECKED);
    }

    bool bRecommendWinloadPatch = false;
    if (IsWinloadPatchApplicable(&bRecommendWinloadPatch))
    {
        if (bRecommendWinloadPatch)
        {
            SendDlgItemMessage(IDC_WINLOAD, BM_SETCHECK, BST_CHECKED);
        }
    }
    else
    {
        GetDlgItem(IDC_WINLOAD).EnableWindow(FALSE);
    }

    SendDlgItemMessage(bAlreadyInstalled ? IDC_REUSEENTRY : IDC_NEWENTRY, BM_SETCHECK, BST_CHECKED);
    ::EnableWindow(GetDlgItem(IDC_ENTRYNAME), !bAlreadyInstalled);

    if (m_ExistingEntryName.ifind(_T("[VKD-Redux]")) != -1)
    {
        SendDlgItemMessage(IDC_ADDSUFFIX, BM_SETCHECK, BST_UNCHECKED);
        ::EnableWindow(GetDlgItem(IDC_ADDSUFFIX), FALSE);
        SetDlgItemText(IDC_ENTRYNAME, m_ExistingEntryName.c_str());
    }
    else
    {
        SendDlgItemMessage(IDC_ADDSUFFIX, BM_SETCHECK, BST_CHECKED);
        SetDlgItemText(IDC_ENTRYNAME, IsVistaOrLater() ? _T("Disable Signature Enforcement Manually!!! (Press F8) [VKD-Redux]") : _T("[VKD-Redux]"));
    }

    SetDlgItemInt(IDC_TIMEOUT, pEditor->GetTimeout());

#ifdef SUPPORT_VISUALDDK
    SendDlgItemMessage(IDC_INSTALLVDDK, BM_SETCHECK, BST_CHECKED);

    TCHAR tszWinDir[MAX_PATH] = { 0, };
    GetWindowsDirectory(tszWinDir, _countof(tszWinDir));

    TCHAR *p = _tcschr(tszWinDir, '\\');
    if (p)
        p[1] = 0;

    SetDlgItemText(IDC_VDDKLOCATION, tszWinDir);

    SetWindowText(_T("Install VirtualKD-Redux and VisualDDK Monitor on Virtual Machine"));

#else
    ::EnableWindow(GetDlgItem(IDC_INSTALLVDDK), FALSE);
    ::EnableWindow(GetDlgItem(IDC_VDDKLOCATION), FALSE);
#endif

    if (_tcsstr(GetCommandLine(), _T("/AUTO")))
        PostMessage(WM_COMMAND, IDOK);

    SetWindowTextA(m_hWnd, "Install VirtualKD-Redux " VIRTUALKD_REDUX_VERSION_STR " on Virtual Machine");

    return TRUE;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    bool createNewEntry = false, setDefault = false, replaceKdcom = false, patchWinLoad = false;
    String entryName, monitorLocation;
    unsigned timeout = -1;

    if (SendDlgItemMessage(IDC_SETDEFAULT, BM_GETCHECK) == BST_CHECKED)
        setDefault = true, timeout = GetDlgItemInt(IDC_TIMEOUT);

    if (SendDlgItemMessage(IDC_KDCOM, BM_GETCHECK) == BST_CHECKED)
        replaceKdcom = true;

    if (SendDlgItemMessage(IDC_WINLOAD, BM_GETCHECK) == BST_CHECKED)
        patchWinLoad = true;

    if (SendDlgItemMessage(IDC_NEWENTRY, BM_GETCHECK) == BST_CHECKED)
    {
        int len = ::GetWindowTextLength(GetDlgItem(IDC_ENTRYNAME));
        entryName.SetLength(GetDlgItemText(IDC_ENTRYNAME, entryName.PreAllocate(len, false), len + 1));
        createNewEntry = true;
    }
    else
    {
        entryName = m_ExistingEntryName;
        if (SendDlgItemMessage(IDC_ADDSUFFIX, BM_GETCHECK) == BST_CHECKED)
            entryName += _T(" [VKD-Redux]");
    }

#ifdef SUPPORT_VISUALDDK
    if (SendDlgItemMessage(IDC_INSTALLVDDK, BM_GETCHECK) == BST_CHECKED)
    {
        size_t len = ::GetWindowTextLength(GetDlgItem(IDC_VDDKLOCATION));
        monitorLocation.SetLength(GetDlgItemText(IDC_VDDKLOCATION, monitorLocation.PreAllocate(len, false), len + 1));
    }
#endif

    if (patchWinLoad)
    {
        String ext = IsLegacyBoot() ? L".exe" : L".efi";
        String file = L"winload" + ext;
        String fp = Path::Combine(Path::GetSpecialDirectoryLocation(dirSystem), file);
        String winloadBackup = Path::Combine(Path::GetSpecialDirectoryLocation(dirSystem), L"winload_old" + ext);

        for (DWORD i = 2; File::Exists(winloadBackup); ++i)
        {
            String winloadBackupCurrentName;
            winloadBackupCurrentName.AppendFormat(L"winload_old_%u.%ls", i, ext.c_str());
            winloadBackup = Path::Combine(Path::GetSpecialDirectoryLocation(dirSystem), winloadBackupCurrentName);
        }

        ActionStatus st;
        {
            st = TakeOwnership(const_cast<LPTSTR>(String(fp).c_str()));
            if (!st.Successful())
            {
                ::MessageBox(HWND_DESKTOP,
                    String::sFormat(_T("Cannot replace owner on %s: %s"), file.c_str(), st.GetMostInformativeText().c_str()).c_str(),
                    NULL,
                    MB_ICONERROR);
                return 0;
            }

            Win32::Security::TranslatedAcl dacl = File::GetDACLForPath(fp, &st);
            if (!st.Successful())
            {
                ::MessageBox(HWND_DESKTOP,
                    String::sFormat(_T("Cannot query permissions on %s: %s"), file.c_str(), st.GetMostInformativeText().c_str()).c_str(),
                    NULL,
                    MB_ICONERROR);
                return 0;
            }
            dacl.AddAllowingAce(STANDARD_RIGHTS_ALL | SPECIFIC_RIGHTS_ALL, BazisLib::Win32::Security::Sid::CurrentUserSid());
            st = File::SetDACLForPath(fp, dacl);
            if (!st.Successful())
            {
                ::MessageBox(HWND_DESKTOP,
                    String::sFormat(_T("Cannot set permissions on %s: %s"), file.c_str(), st.GetMostInformativeText().c_str()).c_str(),
                    NULL,
                    MB_ICONERROR);
                return 0;
            }

            if (!CopyFileW(fp.c_str(), winloadBackup.c_str(), TRUE))
            {
                ::MessageBox(HWND_DESKTOP,
                    String::sFormat(_T("Cannot copy old %s: %s"), file.c_str(), MAKE_STATUS(ActionStatus::FromLastError()).GetMostInformativeText().c_str()).c_str(),
                    NULL,
                    MB_ICONERROR);
                return 0;
            }

            {
                BazisLib::Win32::MemoryMappedFile mmWinLoad(fp.c_str());

                // 66 0F 6F 05 F6 7F 11 00       movdqa  xmm0, cs:__xmm@fffffffffffffffe0000000100000000
                // 48 8D 45 F0                   lea     rax, [rbp+var_10]
                // C7 44 24 30 11 00 00 00       mov     [rsp+50h+var_20], 11h
                // 48 8D 4D 48                   lea     rcx, [rbp+arg_8]
                // 48 89 44 24 28                mov     [rsp+50h+var_28], rax
                // 83 64 24 20 00                and     [rsp+50h+var_30], 0
                // F3 0F 7F 45 F0                movdqu  [rbp+var_10], xmm0
                // E8 3E 65 02 00                call    BlMmAllocatePhysicalPagesInRange
                // 85 C0                         test    eax, eax
                // 0F 89 8C 00 00 00             jns     loc_1800A4B22

                BYTE bAndMovDquCallNeedle[] = { 0x83, 0x64, 0x24, 0x20, 0x00, 0xf3, 0x0f, 0x7f, 0x45, 0xf0, 0xe8 };
                PBYTE pbMovDquCallStart = (PBYTE)memmem(mmWinLoad, (SIZE_T)mmWinLoad.GetFileSize(), bAndMovDquCallNeedle, sizeof(bAndMovDquCallNeedle));
                if (!pbMovDquCallStart)
                {
                    ::MessageBox(HWND_DESKTOP,
                        String::sFormat(L"Unable to find `movdqu, call` pattern in winload").c_str(),
                        NULL,
                        MB_ICONERROR);
                    return 0;
                }

                SIZE_T szSizeRemaining = (SIZE_T)(mmWinLoad.GetFileSize() - (pbMovDquCallStart - (PBYTE)((PVOID)mmWinLoad)));
                if (memmem(pbMovDquCallStart + 1, szSizeRemaining - 1, bAndMovDquCallNeedle, sizeof(bAndMovDquCallNeedle)))
                {
                    ::MessageBox(HWND_DESKTOP,
                        String::sFormat(L"Found multiple patterns in winload").c_str(),
                        NULL,
                        MB_ICONERROR);
                    return 0;
                }

                BYTE bTestEaxEaxNeedle[] = { 0x85, 0xc0 };
                PBYTE pbTestEaxEaxStart = (PBYTE)memmem(pbMovDquCallStart, 50, bTestEaxEaxNeedle, sizeof(bTestEaxEaxNeedle));
                if (!pbTestEaxEaxStart)
                {
                    ::MessageBox(HWND_DESKTOP,
                        String::sFormat(L"Unable to find `test eax, eax` pattern in winload").c_str(),
                        NULL,
                        MB_ICONERROR);
                    return 0;
                }

                if (*(pbTestEaxEaxStart - 5) != 0xe8)
                {
                    ::MessageBox(HWND_DESKTOP,
                        String::sFormat(L"Missing `call` pattern in winload").c_str(),
                        NULL,
                        MB_ICONERROR);
                    return 0;
                }

                if (*(pbTestEaxEaxStart + 2) != 0x0f || *(pbTestEaxEaxStart + 3) != 0x89)
                {
                    ::MessageBox(HWND_DESKTOP,
                        String::sFormat(L"Missing `jns` pattern in winload").c_str(),
                        NULL,
                        MB_ICONERROR);
                    return 0;
                }

                while (pbMovDquCallStart != pbTestEaxEaxStart + 8)
                {
                    *pbMovDquCallStart++ = 0x90;
                }
            }

            {
                DWORD dwOrigChecksum, dwNewChecksum;
                if (MapFileAndCheckSumW(fp.c_str(), &dwOrigChecksum, &dwNewChecksum) != CHECKSUM_SUCCESS)
                {
                    ::MessageBox(HWND_DESKTOP,
                        String::sFormat(L"Unable to recalculate winload checksum").c_str(),
                        NULL,
                        MB_ICONERROR);
                    return 0;
                }

                BazisLib::Win32::MemoryMappedFile mmWinLoad(fp.c_str());
                PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)(PVOID)mmWinLoad;
                PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((PBYTE)((PVOID)mmWinLoad) + pDOSHeader->e_lfanew);
                pNTHeader->OptionalHeader.CheckSum = dwNewChecksum;
            }
        }

    }

    DeployKdCom(replaceKdcom, monitorLocation);

    ActionStatus st = CreateVirtualKDBootEntry(createNewEntry, setDefault, entryName.c_str(), timeout, replaceKdcom);
    if (!st.Successful())
    {
        ::MessageBox(HWND_DESKTOP,
            String::sFormat(_T("Cannot create VirtualKD-Redux boot entry: %s"), st.GetMostInformativeText().c_str()).c_str(),
            NULL,
            MB_ICONERROR);
        return 0;
    }
    else
    {
        bool doReboot = false;

        if (_tcsstr(GetCommandLine(), _T("/AUTO")))
        {
            if (_tcsstr(GetCommandLine(), _T("/REBOOT")))
                doReboot = true;
        }
        else
        {
            if (IsVistaOrLater())
            {
                MessageBox(_T("Warning: You will have to disable driver signature enforcement MANUALLY at every boot to use VirtualKD-Redux! Please refer to https://git.io/JelPr for more details!"), _T("Warning"), MB_ICONWARNING);
            }
            if (MessageBox(_T("VirtualKD-Redux was successfully installed. Please do not forget to run vmmmon64.exe on the host machine, or use VisualDDK for debugging.\r\nDo you want to restart your computer now?"),
                _T("Question"),
                MB_ICONQUESTION | MB_YESNO) == IDYES)
                doReboot = true;
        }

        if (doReboot)
        {
            BazisLib::Win32::Security::PrivilegeHolder holder(SE_SHUTDOWN_NAME);
            ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_SOFTWARE | SHTDN_REASON_MINOR_INSTALLATION);
        }
    }

    EndDialog(1);
    return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(0);
    return 0;
}

LRESULT CMainDlg::OnBnClickedNewentry(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ::EnableWindow(GetDlgItem(IDC_ENTRYNAME), TRUE);
    return 0;
}

LRESULT CMainDlg::OnBnClickedReuseentry(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ::EnableWindow(GetDlgItem(IDC_ENTRYNAME), FALSE);
    return 0;
}

LRESULT CMainDlg::OnBnClickedSetdefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ::EnableWindow(GetDlgItem(IDC_TIMEOUT),
        SendDlgItemMessage(IDC_SETDEFAULT, BM_GETCHECK) == BST_CHECKED);
    return 0;
}
