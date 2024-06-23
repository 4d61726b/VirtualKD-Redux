#pragma once

#include <BazisLib/bzscore/string.h>

#include "bootedit.h"

BazisLib::ActionStatus FindBestOSEntry(BazisLib::ManagedPointer<BootEditor::AIBootConfigurationEntry> *ppEntry,
    bool *pbAlreadyInstalled = NULL,
    BazisLib::ManagedPointer<BootEditor::AIBootConfigurationEditor> pEditor = NULL);

BazisLib::ActionStatus CreateVirtualKDBootEntry(bool CreateNewEntry = true, bool SetNewEntryDefault = true, LPCWSTR lpEntryName = NULL, unsigned Timeout = -1, bool replacingKdcom = false);

LRESULT DeployKdCom(bool replaceKdcom, BazisLib::String monitorLocation = L"");

bool IsVistaOrLater();
bool IsWin8OrLater();
bool IsWin10OrLater();
bool IsWin11OrLater();
bool IsReactOS();
bool IsRunningVMWareHypervisor();
bool IsWinloadPatchApplicable(bool* pbRecommendWinloadPatch);
bool IsLegacyBoot();
bool IsSecureBootEnabled();
