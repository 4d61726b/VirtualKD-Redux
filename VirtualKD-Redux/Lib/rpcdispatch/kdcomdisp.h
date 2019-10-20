/*! \file kdcomdisp.h
    \author Ivan Shcherbakov (Bazis)
    $Id: kdcomdisp.h,v 1.13 2009-09-15 11:06:50 Administrator Exp $
    \brief Contains declaration for class implementing KdComDispatcher::KdReceivePacket() and KdComDispatcher::KdSendPacket()
*/

#pragma once
#include "pipesrv.h"
#include "trassist.h"
#include "PacketLog.h"
#include <BazisLib/bzscore/buffer.h>
#include "Host/kdclient/TerminationNotifier.h"
#include <BazisLib/bzscore/datetime.h>
#include "ServiceCommandHandler.h"

#ifdef _DEBUG
static const int KdTraceEnabled = 1;
#else
static const int KdTraceEnabled = 0;
#endif

//! Contains implementations for KdComDispatcher::KdSendPacket() and KdComDispatcher::KdReceivePacket() functions that are executed on host side.
/*!
	This class contains host-side implementations of ::KdSendPacket() and ::KdReceivePacket() functions.
	These methods are called by KdRpcDispatcher class via IKdComDispatcher interface.
	\remarks See the kdxxx.h file reference for info about ::KdSendPacket() and ::KdReceivePacket().
	\remarks For information about the 'restore snapshot while paused' issue and the workaround, see \ref terminate_from_pause "this page"

	\page terminate_from_pause 'Restore snapshot while paused' problem
		Microsoft Debugging Engine (WinDbg/DbgEng) hangs, if the virtual machine is restored from a snapshot (or restarted), while it was stopped
		by debugger. This creates a big problem for VisualDDK: accidentially restoring the VM to the last snapshot after a bugcheck, without resuming
		execution first, will cause entire Visual Studio process to hang due to the debug engine. VirtualKD 2.2 and higher contains workaround for that:
			- When VirtualKD detects that the VM is reset, it notifies VisualDDK and waits for reply.
			- When VisualDDK receives such a notification, it forces Visual Studio to execute the "run" command and sends a reply to VirtualKD.
			- After VisualDDK has resumed execution, VirtualKD simulates the OS shutdown. As the target is already running, the shutdown notification 
			  gets processed normally.
*/
class KdComDispatcher : public IKdComDispatcher
{
private:
	PipeServer<false, true, true> m_Pipe;

	unsigned KdCompNumberRetries;
	unsigned KdCompRetryCount;

	unsigned KdCompNextPacketIdToSend;
	unsigned KdCompPacketIdExpected;

	bool KD_DEBUGGER_NOT_PRESENT;
	unsigned char KdDebuggerEnabled;

	enum {KdMaxBufferSize = 0xFA0};

	//The following members are used in synchronization bug workaround
	enum {KdMinPacketCountForSuccessfulLink = 5};

	unsigned m_PacketsSentFromLastReset;
	unsigned m_PacketsGotFromLastReset;
	
	unsigned m_SequentialResetCount;

	TraceAssistant m_TraceAssistant;

	PacketLogger m_PacketLogger;

	BazisLib::CBuffer m_RandomValidContextRecord;	//Used to produce 'get context' response while simulating target shutdown

	SessionTerminationNotifier m_Notifier; //Sends message to VisualDDK, that the session is being forcibly terminated.

	ServiceCommandHandler m_ServiceCommandServer;

public:
	KdComDispatcher(const TCHAR *pszPipeName, VMType vmType)
		: m_Pipe(pszPipeName, 1000)
		, m_PacketLogger(pszPipeName)
		, m_ServiceCommandServer(pszPipeName, vmType)
		, KdCompNumberRetries(0)
		, KdCompRetryCount(10)
		, m_SequentialResetCount(0)
		, m_TraceAssistant(pszPipeName)
		, m_Notifier(pszPipeName)
	{
		//The following fields should be overwritten when packets are received
		KD_DEBUGGER_NOT_PRESENT = false;
		KdDebuggerEnabled = 0;

		KdResetPacketNumbering(true);
		m_TraceAssistant.ReloadParams();
	}

	//! Called internally to reset the packet numbering scheme
	void KdResetPacketNumbering(bool InitialSetting)
	{
	  KdCompNextPacketIdToSend = InitialSetting ? 0x80800800 : 0x80800000;
      KdCompPacketIdExpected = 0x80800000;
	  m_PacketsSentFromLastReset = m_PacketsGotFromLastReset = 0;
	}

	virtual void SetKdDebuggerFlags(bool kdDebuggerNotPresent, unsigned char kdDebuggerEnabled)
	{
		KD_DEBUGGER_NOT_PRESENT = kdDebuggerNotPresent;
		KdDebuggerEnabled = kdDebuggerEnabled;
	}

	virtual void GetKdDebuggerFlags(bool *pKdDebuggerNotPresent, unsigned char *pKdDebuggerEnabled)
	{
		*pKdDebuggerNotPresent = KD_DEBUGGER_NOT_PRESENT;
		*pKdDebuggerEnabled = KdDebuggerEnabled;
	}


	//! Called internally to call a bodyless control packet (such as ACK or RETRY)
	void KdpSendControlPacket(ULONG PacketType, ULONG NextPacketId);

	KD_RECV_CODE KdCompReceivePacketLeader(ULONG /*PacketType*/,
										   ULONG *pSignature,
										   PKD_CONTEXT pContext);

	//! Called to compute checksum of a data block
	unsigned KdpComputeChecksum(const void *pBuffer, unsigned Size)
	{
		unsigned char *p = (unsigned char *)pBuffer;
		unsigned sum = 0;
		for (unsigned i = 0; i < Size; i++)
			sum += p[i];
		return sum;
	}

	//! Called inside VMWARE-VMX.EXE process space with parameters unmarshalled from ::KdReceivePacket() call in KDVMWARE.DLL.
	KD_RECV_CODE __stdcall KdReceivePacket(ULONG PacketType,
										   PKD_BUFFER FirstBuffer,
										   PKD_BUFFER SecondBuffer,
										   PULONG PayloadBytes,
										   PKD_CONTEXT KdContext,
										   unsigned *pPollIterationsToSkip);

	//! Called inside VMWARE-VMX.EXE process space with parameters unmarshalled from ::KdSendPacket() call in KDVMWARE.DLL.
	bool __stdcall	KdSendPacket(ULONG PacketType,
								 PKD_BUFFER FirstBuffer,
								 PKD_BUFFER SecondBuffer,
								 PKD_CONTEXT KdContext);
	
	//! Called by KdRpcDispatcher to report protocol version mismatch
	virtual void ReportProtocolVersionError(unsigned GuestVersion, unsigned HostVersion);


	//! Sends a packet stating that NTOSKRNL was unloaded, that makes WinDbg/DBGENG end the session and prevents it from hanging when VM unloads.
	void SimulateWindowsTermination();

	~KdComDispatcher()
	{
		SimulateWindowsTermination();
	}
};
