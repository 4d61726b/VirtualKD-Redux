#pragma once

#include <BazisLib/bzshlp/serializer.h>

#define DEBUGGER_TYPE_KD                (0)
#define DEBUGGER_TYPE_WINDBG            (1)
#define DEBUGGER_TYPE_CUSTOM            (2)
#define DEBUGGER_TYPE_WINDBGPREVIEW     (3)
#define DEBUGGER_TYPE_UNKNOWN           (999)

#define VKD_REGISTRY_CONFIG_PATH        L"SOFTWARE\\VirtualKD-Redux"

#define VKD_REGISTRY_TRACE_ASSIST_PATH  VKD_REGISTRY_CONFIG_PATH L"\\TraceAssist"
#define VKD_REGISTRY_PATCHER_PATH       VKD_REGISTRY_CONFIG_PATH L"\\Patcher"
#define VKD_REGISTRY_MONITOR_PATH       VKD_REGISTRY_CONFIG_PATH L"\\Monitor"
#define VKD_REGISTRY_VIRTUALBOX_PATH    VKD_REGISTRY_CONFIG_PATH L"\\VirtualBoxIntegration"

DECLARE_SERIALIZEABLE_STRUC6_I(PatchingParams,
    bool, AllowPatchingAtTableStart, true,
    bool, DefaultPatchingAtTableStart, false,
    bool, AllowReplacingFirstCommand, false,
    bool, WaitForNonZeroFirstCommand, true,
    unsigned, MinRunTimeBeforePatch, 10000,
    bool, DisablePatchDatabase, true);

DECLARE_SERIALIZEABLE_STRUC1_I(KdClientParams,
    unsigned, VersionOverride, 0);

DECLARE_SERIALIZEABLE_STRUC4_I(TraceAssistParams,
    bool, TraceAssistEnabled, false,
    BazisLib::String, MessagePrefix, L"",
    BazisLib::String, LogFileDirectory, L"",
    bool, OverwriteFileOnStart, false);

DECLARE_SERIALIZEABLE_STRUC10_I(MonitorParams,
    BazisLib::String, ToolsPath, L"",
    BazisLib::String, PreviewPath, L"",
    bool, AutoInvokeDebugger, true,
    bool, AutoCloseDebugger, false,
    unsigned, DebuggerType, DEBUGGER_TYPE_UNKNOWN,
    bool, WaitForOS, true,
    bool, InitialBreakIn, true,
    unsigned, DebugLevel, 1,
    unsigned, PatchDelay, 3,
    BazisLib::String, CustomDebuggerTemplate, L"cmd.exe /c \"$(toolspath)\\test.cmd\" $(pipename)");