#pragma once
#include <BazisLib/bzscore/string.h>
#include "../kdclient/cmdline.h"

unsigned VBoxCmdLineToVMNameW(const RemoteProcessInfo &info, wchar_t *pNameBuffer, size_t MaxLength);
unsigned VBoxCmdLineToPipeNameW(wchar_t *pNameBuffer, size_t MaxLength, unsigned PID = 0);
BazisLib::String VBoxCmdLineToMachineID(const wchar_t *pCmdLine);