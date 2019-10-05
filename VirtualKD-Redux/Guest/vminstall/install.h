#pragma once

#include "bootedit.h"

BazisLib::ActionStatus FindBestOSEntry(BazisLib::ManagedPointer<BootEditor::AIBootConfigurationEntry> *ppEntry,
									  bool *pbAlreadyInstalled = NULL,
									  BazisLib::ManagedPointer<BootEditor::AIBootConfigurationEditor> pEditor = NULL);

BazisLib::ActionStatus CreateVirtualKDBootEntry(bool CreateNewEntry = true, bool SetNewEntryDefault = true, LPCWSTR lpEntryName = NULL, unsigned Timeout = -1, bool replacingKdcom = false);
