/*! \file status.h
    \author Ivan Shcherbakov (Bazis)
    $Id: status.h,v 1.9 2009-05-30 18:15:27 Administrator Exp $
    \brief Contains declarations for status reporting structures
*/

#pragma once

//! Contains status and statistics info reported by KDCLIENT.DLL for VMMON.EXE
/*!
	\remarks This structure is filled by zeroes at creation, so default values for members meaning
			 "operation pending" or "not yet started" should correspond to zero.
*/
struct KdClientStatus
{
	wchar_t PipeName[256];
	unsigned BytesSent;
	unsigned BytesReceived;
	unsigned PacketsSent;
	unsigned PacketsReceived;
	unsigned ResyncCount;
	bool DebuggerConnected;
	bool OSDetected;
	bool TraceAssistUpdatePending;
	unsigned DebugLevel;
	unsigned BytesDropped;
	//! Contains ErrorCode + 1. Default file mapping value of 0 corresponds to PENDING state.
	unsigned PatchErrorPlus1;
    struct ProtocolMismatchStatus {
        unsigned GuestVersion;
        unsigned HostVersion;
    } ProtocolMismatchStatus;
	
	//Added in version 1.2
	unsigned LogAllPackets;

	//Added in version 2.5
	unsigned PacketPollCallsServiced;
	unsigned HostSideDLLVersion;
};

//! Specifies debug message level for status reporter
enum
{
	DebugLevelNone = 0,
	DebugLevelTracePatching,
	DebugLevelTraceKdCom,	//Works only for debug builds
	DebugLevelTraceKdComAll,
};

#ifndef KDVMWARE_USE_PROXY
static const TCHAR tszMappingNameFormat[] = _T("BazisKdVmwareStatus_%04X");
static const TCHAR tszLogPipeNameFormat[] = _T("\\\\.\\pipe\\BazisKdVmwareLog_%04X");
#else
static const TCHAR tszMappingNameFormat[] = _T("BazisKdVmwareStatus_proxy");
static const TCHAR tszLogPipeNameFormat[] = _T("\\\\.\\pipe\\BazisKdVmwareLog_proxy");
#endif