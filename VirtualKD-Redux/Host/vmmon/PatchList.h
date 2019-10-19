/*! \file PatchList.h
    \author Ivan Shcherbakov (Bazis)
    $Id: PatchList.h,v 1.8 2009-06-07 15:31:55 Administrator Exp $
    \brief Contains declaration for patched process state structure
*/

#pragma once
#include <BazisLib/bzscore/datetime.h>
#include <BazisLib/bzscore/string.h>
#include <vector>
#include <set>
#include "rpcdispatch/status.h"
#include "../kdclient/patchapi.h"

//! Represents the mode of a patched VMWARE-VMX.EXE process
enum PatchState
{
    PatchPending,
    PatchInProgress,
    PatchInProgress2,
    PatchSuccessful,
    PatchFailed,
    PatchProcessTerminated,

    UnpatchInProgress,
    Unpatched,

    ProtocolMismatch,
};

//! Represents the state of a patched VMWARE-VMX.EXE process
class PatchedProcess
{
public:
    BazisLib::DateTime StartTime;
    BazisLib::DateTime StopTime;
    unsigned PID;
    PatchState State;
    PatchState PreviousState;
    bool StillRunning;
    BazisLib::String PipeName;

    BazisLib::DateTime PreviousCPUTime;
    BazisLib::DateTime PreviousSampleTime;
    unsigned CpuUsage;
    unsigned AvgCpuUsage;

    HANDLE hRemoteThread;

    DWORD idDebuggerProcess;

    BazisLib::String Log;
    HANDLE hLogPipe;
    bool LogChanged;

    HANDLE hMapping;
    KdClientStatus *pStatus;

    unsigned PrevSent;
    unsigned PrevRecvd;

    unsigned SendRate;
    unsigned RecvRate;

    unsigned MaxSendRate;
    unsigned MaxRecvRate;

    unsigned PrevPacketsSent;
    unsigned PrevPacketsRecvd;

    unsigned PacketSendRate;
    unsigned PacketRecvRate;

    unsigned PrevPolls;
    unsigned PollRate;

    bool SessionNameRetreived;
    VMType vmType;

    PatchedProcess()
        : PID(0)
        , StillRunning(false)
        , CpuUsage(-1)
        , AvgCpuUsage(0)
        , hRemoteThread(INVALID_HANDLE_VALUE)
        , idDebuggerProcess(0)
        , hLogPipe(INVALID_HANDLE_VALUE)
        , LogChanged(false)
        , hMapping(INVALID_HANDLE_VALUE)
        , pStatus(NULL)
        , PrevSent(0)
        , PrevRecvd(0)
        , SendRate(0)
        , RecvRate(0)
        , MaxSendRate(0)
        , MaxRecvRate(0)
        , PrevPacketsSent(0)
        , PrevPacketsRecvd(0)
        , PacketSendRate(0)
        , PacketRecvRate(0)
        , SessionNameRetreived(false)
        , PrevPolls(0)
        , PollRate(0)
        , vmType(kVMUnknown)
    {
    }
};

typedef std::vector<PatchedProcess> PatchedProcessList;
typedef std::set<unsigned> PatchedProcessSet;