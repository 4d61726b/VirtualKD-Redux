/*! \file rpcdisp.h
    \author Ivan Shcherbakov (Bazis)
    $Id: rpcdisp.h,v 1.9 2009-06-07 10:51:22 Administrator Exp $
    \brief Contains host side of KdXXX() marshalling functions
*/

#pragma once
#include "kdvmguestlib/kdrpc.h"
#include "kdvmguestlib/kdxxx.h"
#include <BazisLib/bzscore/assert.h>

//! Contains declarations for KdSendPacket()/KdReceivePacket() versions called on host side.
/*!
	\remarks See KdComDispatcher class documentation for details.
*/
class IKdComDispatcher
{
public:
	virtual KD_RECV_CODE __stdcall KdReceivePacket(
		__in ULONG PacketType,
		__inout_opt PKD_BUFFER FirstBuffer,
		__inout_opt PKD_BUFFER SecondBuffer,
		__out_opt PULONG PayloadBytes,
		__inout_opt PKD_CONTEXT KdContext,
		unsigned *pPollIterationsToSkip)=0;

	virtual bool __stdcall	KdSendPacket(
		__in ULONG PacketType,
		__in PKD_BUFFER FirstBuffer,
		__in_opt PKD_BUFFER SecondBuffer,
		__inout PKD_CONTEXT KdContext)=0;

	virtual void ReportProtocolVersionError(int GuestVersion, int HostVersion)=0;
	virtual void SetKdDebuggerFlags(bool KdDebuggerNotPresent, unsigned char KdDebuggerEnabled)=0;
	virtual void GetKdDebuggerFlags(bool *pKdDebuggerNotPresent, unsigned char *pKdDebuggerEnabled)=0;

	virtual ~IKdComDispatcher(){}
};

#include "User/kdclient/patchapi.h"

//! Contains code unmarshalling parameters for ::KdSendPacket()/::KdReceivePacket()
/*! This class parses the data blocks containing function parameters and calls the corresponding methods
	from IKdComDispatcher class. The OnRequest() method is called by either \ref guestrpc handler, or
	the RpcProxy class. The method internally processes the VersionReport and TestConnection requests,
	and calls the corresponding method from IKdComDispatcher for ReceivePacket and SendPacket requests.
	For more details see the \ref kdrpc_proto protocol description.
*/
class KdRpcDispatcher
{
private:
	enum {REPLY_BUFFER_BASE = 262144};
	enum {MAX_BUFFER_SIZE = 65536};
	enum {REPLY_BUFFER_SIZE = REPLY_BUFFER_BASE + MAX_BUFFER_SIZE};
	char *m_pReplyBuffer;

private:
	IKdComDispatcher *m_pDispatcher;
	bool m_bProtocolVersionMatch;

	static inline void Serialize(char *pBase, unsigned &offset, const void *pBuffer, unsigned Size)
	{
		memcpy(pBase + offset, pBuffer, Size);
		offset += Size;
	}

public:
	char szTestReply[KDRPC_TEST_BUFFER_SIZE + sizeof(g_szRPCReplySignature)];

	KdRpcDispatcher(IKdComDispatcher *pDispatcher)
	{
		m_pDispatcher = pDispatcher;
		m_pReplyBuffer = new char[REPLY_BUFFER_SIZE];
		m_bProtocolVersionMatch = false;
	}

	~KdRpcDispatcher()
	{
		delete m_pReplyBuffer;
		delete m_pDispatcher;
	}

	//! This method is called when a data packet is received from KDVMWARE.DLL via \ref guestrpc.
	unsigned OnRequest(const char *pRequest, unsigned RequestSize, char **ppReply)
	{
		ASSERT(pRequest && ppReply);
		switch (pRequest[0])
		{
		case VersionReport:
			*ppReply = m_pReplyBuffer;
			if (*((int *)(pRequest + 1)) == KDRPC_PROTOCOL_VERSION)
				m_bProtocolVersionMatch = true;
			else
			{
				m_pDispatcher->ReportProtocolVersionError(*((int *)(pRequest + 1)), KDRPC_PROTOCOL_VERSION);
			}
			memcpy(m_pReplyBuffer, g_szRPCReplySignature, sizeof(g_szRPCReplySignature) - 1);
			*((int *)(m_pReplyBuffer + sizeof(g_szRPCReplySignature) - 1)) = KDRPC_PROTOCOL_VERSION;
			return sizeof(int) + sizeof(g_szRPCReplySignature) - 1;
		case TestConnection:
			if (!m_bProtocolVersionMatch)
			{
				m_pDispatcher->ReportProtocolVersionError(0x100, KDRPC_PROTOCOL_VERSION);
				*ppReply = szTestReply;
				return 0;
			}
			*ppReply = szTestReply;
			memcpy(szTestReply, g_szRPCReplySignature, sizeof(g_szRPCReplySignature));
			for (int i = 0; i < KDRPC_TEST_BUFFER_SIZE; i++)
				szTestReply[i + sizeof(g_szRPCReplySignature) - 1] = (char)(i ^ 0x55);
			return sizeof(g_szRPCReplySignature) + KDRPC_TEST_BUFFER_SIZE - 1;
		case ReceivePacket:
			{
				*ppReply = m_pReplyBuffer;
				if (RequestSize != (1 + 2 * sizeof(ULONG) + 2 * sizeof(SendableKdBuffer) + sizeof(KD_CONTEXT)))
					return 0;
				KD_BUFFER Buf1 = {0,};
				KD_BUFFER Buf2 = {0,};
				ULONG globals = *((ULONG *)(pRequest + 1 + sizeof(ULONG)));
				m_pDispatcher->SetKdDebuggerFlags((globals & kKDDebuggerNotPresentSet) != 0, (globals >> 8) & 0xFF);
				memcpy(&Buf1, (pRequest + 1 + 2 * sizeof(ULONG)), sizeof(SendableKdBuffer));
				memcpy(&Buf2, (pRequest + 1 + 2 * sizeof(ULONG) + sizeof(SendableKdBuffer)), sizeof(SendableKdBuffer));
				unsigned firstBufOffset = sizeof(g_szRPCReplySignature) + 2 * sizeof(SendableKdBuffer) + KDRPC_RECV_RETURNED_ULONGS * sizeof(ULONG) + sizeof(KD_CONTEXT);
				Buf1.pData = (PUCHAR) &m_pReplyBuffer[firstBufOffset];
				Buf2.pData = (PUCHAR) &m_pReplyBuffer[REPLY_BUFFER_BASE];
				PKD_CONTEXT pContext = (PKD_CONTEXT)(pRequest + 1 + 2 * sizeof(ULONG) + 2 * sizeof(SendableKdBuffer));

				ULONG PayloadBytes = 0;
#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
				unsigned PollIterationsToSkip = 0;
#endif
				unsigned Status = m_pDispatcher->KdReceivePacket(*((ULONG *)(pRequest + 1)),
															   Buf1.MaxLength ? &Buf1 : NULL,
															   Buf2.MaxLength ? &Buf2 : NULL,
															   &PayloadBytes,
															   pContext,
#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
															   &PollIterationsToSkip);
#else
															   NULL);
#endif
				unsigned offset = sizeof(g_szRPCReplySignature);
				memcpy(m_pReplyBuffer, g_szRPCReplySignature, sizeof(g_szRPCReplySignature) - 1);
				m_pReplyBuffer[sizeof(g_szRPCReplySignature)-1] = ReceivePacket;
				Serialize(m_pReplyBuffer,offset, &Buf1, sizeof(SendableKdBuffer));
				Serialize(m_pReplyBuffer,offset, &Buf2, sizeof(SendableKdBuffer));
				Serialize(m_pReplyBuffer,offset, pContext, sizeof(KD_CONTEXT));

				unsigned Size1 = Buf1.Length, Size2 = Buf2.Length;
				globals = kKDDebuggerEnabledValueAvailable;

				bool notPresent = false;
				unsigned char kdDebuggerEnabled = 0;
				m_pDispatcher->GetKdDebuggerFlags(&notPresent, &kdDebuggerEnabled);
				if (notPresent)
					globals |= kKDDebuggerNotPresentSet;

				globals |= (kdDebuggerEnabled << 8);

				ULONG *pAdditionalInfo = (ULONG *)(m_pReplyBuffer + offset);
				pAdditionalInfo[0] = Status;
				pAdditionalInfo[1] = PayloadBytes;
				pAdditionalInfo[2] = Size1;
				pAdditionalInfo[3] = Size2;
				pAdditionalInfo[4] = globals;
#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
				pAdditionalInfo[5] = PollIterationsToSkip;
#endif
				offset += KDRPC_RECV_RETURNED_ULONGS * sizeof(ULONG);
				ASSERT(offset == firstBufOffset);
				offset += Size1;
				memcpy(m_pReplyBuffer + offset, &m_pReplyBuffer[REPLY_BUFFER_BASE], Size2);
				offset += Size2;
				return offset;
			}
			break;
		case SendPacket:
			{
				*ppReply = m_pReplyBuffer;
				unsigned buf1Offset = 1 + sizeof(ULONG) * KDRPC_SEND_PASSED_ULONGS + 2 * sizeof(SendableKdBuffer) + sizeof(KD_CONTEXT);
				if (RequestSize < buf1Offset)
					return 0;
				KD_BUFFER Buf1 = {0,};
				KD_BUFFER Buf2 = {0,};
				memcpy(&Buf1, (pRequest + 1), sizeof(SendableKdBuffer));
				memcpy(&Buf2, (pRequest + 1 + sizeof(SendableKdBuffer)), sizeof(SendableKdBuffer));
				PKD_CONTEXT pContext = (PKD_CONTEXT)(pRequest + 1 + 2 * sizeof(SendableKdBuffer));
				
				ULONG *pParams = (ULONG *)(pRequest + 1 + 2 * sizeof(SendableKdBuffer) + sizeof(KD_CONTEXT));
				if (RequestSize != (buf1Offset + pParams[1] + pParams[2]))
					return 0;

				ULONG globals = pParams[3];
				m_pDispatcher->SetKdDebuggerFlags((globals & kKDDebuggerNotPresentSet) != 0, (globals >> 8) & 0xFF);


				Buf1.pData = (PUCHAR) &pRequest[buf1Offset];
				Buf2.pData = (PUCHAR) &pRequest[buf1Offset + pParams[1]];

				bool sendResult = m_pDispatcher->KdSendPacket(pParams[0],
															  Buf1.Length ? &Buf1 : NULL,
															  Buf2.Length ? &Buf2 : NULL,
															  pContext);

				unsigned offset = sizeof(g_szRPCReplySignature);
				memcpy(m_pReplyBuffer, g_szRPCReplySignature, sizeof(g_szRPCReplySignature) - 1);
				m_pReplyBuffer[sizeof(g_szRPCReplySignature)-1] = SendPacket;
				Serialize(m_pReplyBuffer,offset, pContext, sizeof(KD_CONTEXT));
				
				globals = kKDDebuggerEnabledValueAvailable;

				bool notPresent = false;
				unsigned char kdDebuggerEnabled = 0;
				m_pDispatcher->GetKdDebuggerFlags(&notPresent, &kdDebuggerEnabled);
				if (notPresent)
					globals |= kKDDebuggerNotPresentSet;

				globals |= (kdDebuggerEnabled << 8);

				if (!sendResult)
					globals |= kRetryKdSendPacket;

				Serialize(m_pReplyBuffer,offset, &globals, sizeof(ULONG));
				return offset;
			}
			break;
		}
		return 0;
	}
};