/*! \file kdcomdisp.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: kdcomdisp.cpp,v 1.14 2009-10-07 19:09:53 Bazis Exp $
    \brief Contains implementation for KdReceivePacket()/KdSendPacket()
*/

#include "stdafx.h"
#include "kdcomdisp.h"
#include "reporter.h"

#define DbgKdContinueApi                    0x00003136
#define DbgKdContinueApi2                   0x0000313C
#define DbgKdReadControlSpaceApi            0x00003137
#define DbgKdSetContextApi                  0x00003133
/*
    The code in this module implements the KDCOM protocol (protocol used to debug kernel over COM port).
    When some things are simplified/changed compared to full protocol description, the comment SIMPLIFY:
    is added.
*/


KD_RECV_CODE KdComDispatcher::KdCompReceivePacketLeader(ULONG /*PacketType*/,
    ULONG *pSignature,
    PKD_CONTEXT pContext)
{
    ASSERT(pContext && pSignature);
    unsigned repeatCount = 0;
    char ch = 0, lastCh = 0;
    bool BreakInRequested = false;
    for (;;)
    {
        if (m_Pipe.Receive(&ch, 1) != 1)
        {
            if (BreakInRequested)
            {
                pContext->BreakInRequested = TRUE;
                return KD_RECV_CODE_FAILED;
            }
            else
            {
                unsigned err = m_Pipe.GetLastOperationError();
                switch (err)
                {
                case ERROR_PIPE_LISTENING:
                case ERROR_PIPE_NOT_CONNECTED:
                case ERROR_NO_DATA:
                    Sleep(10);
                    break;
                }
                return KD_RECV_CODE_TIMEOUT;
            }
        }
        switch (ch)
        {
        case '0':
        case 'i':
            break;
        case 'b':
            BreakInRequested = true;
            //no break!
        default:
            g_pReporter->GetStatusPointer()->BytesDropped++;
            repeatCount = 0;
            continue;
        }

        if (!repeatCount)
        {
            lastCh = ch;
            repeatCount++;
            continue;
        }

        if (ch != lastCh)
            repeatCount = 1, lastCh = ch;
        else
        {
            if (++repeatCount >= 4)
            {
                if (BreakInRequested)
                    pContext->BreakInRequested = TRUE;
                *pSignature = (ch == '0') ? '0000' : 'iiii';
                KD_DEBUGGER_NOT_PRESENT = false;
                KdDebuggerEnabled |= 3;
                return KD_RECV_CODE_OK;
            }
        }
    }
}

void KdComDispatcher::KdpSendControlPacket(ULONG PacketType, ULONG NextPacketId)
{
    static char __unused[sizeof(KD_PACKET_HEADER) == 16];
    static unsigned LastSentID = 0;
    KD_PACKET_HEADER hdr;
    hdr.Signature = 'iiii';
    hdr.PacketType = (USHORT)PacketType;
    hdr.TotalDataLength = 0;
    if (NextPacketId)
        hdr.PacketID = LastSentID = NextPacketId;
    else
        hdr.PacketID = LastSentID;
    hdr.Checksum = 0;
    if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdCom))
    {
        TCHAR tsz[512];
        _sntprintf(tsz, __countof(tsz), _T("Sending control packet (type = %d, id = %08X)... "),
            hdr.PacketType,
            hdr.PacketID);
        g_pReporter->LogLineIfEnabled(tsz);
    }
    m_Pipe.Send(&hdr, sizeof(hdr));
    if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdCom))
    {
        g_pReporter->LogLineIfEnabled(_T("ok\r\n"));
    }
}

KD_RECV_CODE __stdcall KdComDispatcher::KdReceivePacket(ULONG PacketType,
    PKD_BUFFER FirstBuffer,
    PKD_BUFFER SecondBuffer,
    PULONG PayloadBytes,
    PKD_CONTEXT KdContext,
    unsigned *pPollIterationsToSkip)
{
    /*#ifdef _DEBUG
        if (PacketType == 0x1234)
        {
            memset(FirstBuffer->pData, '+', FirstBuffer->MaxLength);
            memset(SecondBuffer->pData, '*', SecondBuffer->MaxLength);
            FirstBuffer->Length = FirstBuffer->MaxLength;
            SecondBuffer->Length = SecondBuffer->MaxLength;
            *PayloadBytes = SecondBuffer->MaxLength;
            return KD_RECV_CODE_OK;
        }
    #endif*/
    /*#ifdef _DEBUG
        if (GetKeyState(VK_SCROLL) & 1)
        {
            INPUT inp = {0,};
            inp.type = INPUT_KEYBOARD;
            inp.ki.wVk = VK_SCROLL;
            SendInput(1, &inp, sizeof(inp));
            inp.ki.dwFlags |= KEYEVENTF_KEYUP;
            SendInput(1, &inp, sizeof(inp));
            Sleep(100);
            SimulateWindowsTermination();
        }
    #endif*/
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
    if (pPollIterationsToSkip)
    {
        static int skip = 0;
        *pPollIterationsToSkip = skip;
    }
#endif

    ASSERT(KdContext);
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
    g_pReporter->GetStatusPointer()->OSDetected = true;
#endif

    if (KdTraceEnabled)
    {
        TCHAR tsz[512];
        switch (PacketType)
        {
        case KdPacketAcknowledge:
            if (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdCom)
            {
                _sntprintf(tsz, __countof(tsz), _T("KdReceivePacket(%d)\r\n"),
                    PacketType);
                g_pReporter->LogLineIfEnabled(tsz);
            }
            break;
        case KdCheckForAnyPacket:
            if (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdComAll)
                g_pReporter->LogLineIfEnabled(_T("KdReceivePacket(): checking for new packets\r\n"));
            break;
        }
    }

    bool instantBreakIn = m_ServiceCommandServer.CheckAndClearInstantBreakRequestedFlag();
    if (instantBreakIn)
        KdContext->BreakInRequested = TRUE;

    if (PacketType == KdCheckForAnyPacket)
    {
        bool hasData = m_Pipe.HasDataInBuffer();
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
        g_pReporter->GetStatusPointer()->DebuggerConnected = m_Pipe.IsClientConnected();
        g_pReporter->GetStatusPointer()->PacketPollCallsServiced++;
#endif
        if (!hasData)
        {
            if (instantBreakIn)
                return KD_RECV_CODE_OK;
            return KD_RECV_CODE_TIMEOUT;
        }
        char ch = 0;
        if (!m_Pipe.Receive(&ch, 1))
            return KD_RECV_CODE_TIMEOUT;
        if (ch != 'b')
            return KD_RECV_CODE_TIMEOUT;
        return KD_RECV_CODE_OK;
    }

    if (FirstBuffer)
        FirstBuffer->Length = 0;
    if (SecondBuffer)
        SecondBuffer->Length = 0;

    for (;;)
    {
        KD_PACKET_HEADER header = { 0, };
        KD_RECV_CODE status = KdCompReceivePacketLeader(PacketType, &header.Signature, KdContext);
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
        g_pReporter->GetStatusPointer()->DebuggerConnected = m_Pipe.IsClientConnected();
#endif
        if (status != KD_RECV_CODE_TIMEOUT)
            KdCompNumberRetries = KdCompRetryCount;
        if (status != KD_RECV_CODE_OK)
        {
            if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdComAll))
            {
                TCHAR tsz[512];
                _sntprintf(tsz, __countof(tsz), _T("KdCompReceivePacketLeader(): %s\r\n"), (status == KD_RECV_CODE_TIMEOUT) ? _T("timeout") : _T("error"));
                g_pReporter->LogLineIfEnabled(tsz);
            }
            return status;
        }
        size_t size = sizeof(header) - sizeof(header.Signature);
        //SIMPLIFY: We do not have 'failed' condition any more. Only timeout, so receive the rest of the packet in a single call.
        if (m_Pipe.Receive(&header.PacketType, size, false) != size)
            return KD_RECV_CODE_TIMEOUT;

        if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdCom))
        {
            TCHAR tsz[512];
            _sntprintf(tsz, __countof(tsz), _T("Got packet (expected %d) (signature = %08X, id = %08X, type = %d, data = %d, checksum = %d)\r\n"),
                PacketType,
                header.Signature,
                header.PacketID,
                header.PacketType,
                header.TotalDataLength,
                header.Checksum);
            g_pReporter->LogLineIfEnabled(tsz);
        }

        //SIMPLIFY: Originally this check should be performed just after receiving the packet type
        if ((header.Signature == 'iiii') && (header.PacketType == KdPacketRetryRequest))
            return KD_RECV_CODE_FAILED;

        if (header.Signature == 'iiii')
        {
            switch (header.PacketType)
            {
            case KdPacketAcknowledge:
                if ((PacketType == 4) && (header.PacketID == (KdCompNextPacketIdToSend & 0xFFFFF7FF)))
                {
                    KdCompNextPacketIdToSend ^= 1;
                    return KD_RECV_CODE_OK;
                }
                break;
            case KdPacketResynchronize:
                if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdCom))
                {
                    TCHAR tsz[512];
                    _sntprintf(tsz, __countof(tsz), _T("Resync (got %d, sent %d, seq cnt %d)\r\n"),
                        m_PacketsGotFromLastReset,
                        m_PacketsSentFromLastReset,
                        m_SequentialResetCount);
                    g_pReporter->LogLineIfEnabled(tsz);
                }
                if ((m_PacketsGotFromLastReset <= KdMinPacketCountForSuccessfulLink) && (m_PacketsSentFromLastReset <= KdMinPacketCountForSuccessfulLink))
                    m_SequentialResetCount++;
                KdResetPacketNumbering(false);
                /*
                    The following line serves as a workaround for the 'resync bounce' bug.
                    The KDCOM protocol was originally designed for normal, hardware COM ports. It means, that
                    an operating system that gained control over the COM port did not receive the previously
                    sent data. In case of named pipes, the following scenario usually takes place:
                    1) KD opens the pipe and starts sending RESET packets every second
                    2) OS loads and starts receiving packets
                    3) OS receives N RESET packets and sends N RESET acknowledgments
                    4) KD receives the first RESET ack packet and starts normal operation (sends something)
                    5) OS receives normal packet
                    6) KD receives next RESET ack, performs resynchronization and... sends a RESET packet
                    7) OS responds with a RESET ack
                    8) KD reads another previous RESET ack and continues
                    9) ...
                    A) KD receives another RESET from OS and goes to step 6
                    The loop is never broken, the RESET packets continue being bounced from KD to OS and back

                    By discarding all buffered data from the named pipe when a RESET occurs, the problem is solved.
                */
                m_Pipe.DiscardBufferedData();
                KdpSendControlPacket(KdPacketResynchronize, 0);
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
                g_pReporter->GetStatusPointer()->ResyncCount++;
#endif
                return KD_RECV_CODE_FAILED;
            case KdPacketRetryRequest:
                return KD_RECV_CODE_FAILED;
            }
        }
        else	//header.Signature == '0000'
        {
            if (PacketType != KdPacketAcknowledge)
            {
                if (header.TotalDataLength <= KdMaxBufferSize)
                {
                    if (header.TotalDataLength >= FirstBuffer->MaxLength)
                    {
                        *PayloadBytes = header.TotalDataLength - FirstBuffer->MaxLength;
                        if (m_Pipe.Receive(FirstBuffer->pData, FirstBuffer->MaxLength, false) == FirstBuffer->MaxLength)
                        {
                            FirstBuffer->Length = FirstBuffer->MaxLength;
                            if (!(*PayloadBytes) || (m_Pipe.Receive(SecondBuffer->pData, *PayloadBytes, false) == *PayloadBytes))
                            {
                                SecondBuffer->Length = (USHORT)*PayloadBytes;
                                unsigned char ch = 0;
                                if (m_Pipe.Receive(&ch, 1) && (ch == 0xAA))
                                {
                                    if (PacketType != header.PacketType)
                                    {
                                        KdpSendControlPacket(KdPacketAcknowledge, header.PacketID);
                                        continue;
                                    }
                                    else if ((header.PacketID == 0x80800000) || (header.PacketID == 0x80800001))
                                    {
                                        if (header.PacketID != KdCompPacketIdExpected)
                                        {
                                            KdpSendControlPacket(KdPacketAcknowledge, header.PacketID);
                                            continue;
                                        }
                                    }
                                    int checksum = KdpComputeChecksum(FirstBuffer->pData, FirstBuffer->Length);
                                    checksum += KdpComputeChecksum(SecondBuffer->pData, SecondBuffer->Length);
                                    if (checksum == header.Checksum)
                                    {
                                        KdpSendControlPacket(KdPacketAcknowledge, header.PacketID);
                                        KdCompPacketIdExpected ^= 1;
                                        if (++m_PacketsGotFromLastReset >= KdMinPacketCountForSuccessfulLink)
                                            m_SequentialResetCount = 0;
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
                                        g_pReporter->GetStatusPointer()->BytesReceived += header.TotalDataLength + sizeof(KD_PACKET_HEADER) + 1;
                                        g_pReporter->GetStatusPointer()->PacketsReceived++;
#endif
                                        m_PacketLogger.OnSendReceivePacket(g_pReporter->GetStatusPointer()->LogAllPackets != 0,
                                            false,
                                            PacketType,
                                            FirstBuffer,
                                            SecondBuffer,
                                            KdContext);

                                        return KD_RECV_CODE_OK;
                                    }
                                }
                            }

                        }
                    }
                }
                KdpSendControlPacket(KdPacketRetryRequest, 0);
                continue;
            }
            if (header.PacketID == KdCompPacketIdExpected)
            {
                KdpSendControlPacket(KdPacketRetryRequest, 0);
                KdCompNextPacketIdToSend ^= 1;
                return KD_RECV_CODE_OK;
            }
            else
            {
                KdpSendControlPacket(KdPacketAcknowledge, header.PacketID);
                continue;
            }
        }
    }

    return KD_RECV_CODE_TIMEOUT;
}

static bool FixVerifierUnicodeReport(PKD_BUFFER pDataBuffer)
{
    char *pText = (char *)pDataBuffer->pData;
    char *pZero = NULL;
    char *pEnd = pText + pDataBuffer->Length;
    for (int i = 0; i < pDataBuffer->Length; i++)
        if (!pText[i])
        {
            pZero = &pText[i];
            break;
        }
    if (!pZero)
        return false;
    if (!strstr(pText, "Driver Verifier:"))
        return false;

    char *pSrc = pZero + 1;
    while (pSrc < pEnd)
    {
        if (!pSrc[0])
        {
            pSrc++;
            continue;
        }
        *pZero = *pSrc;

        pZero++;
        pSrc++;
    }
    //We cannot modify the length of the packet, as it seems to be specified somewhere else additionally.
    //Instead, we exploit the same effect of chopping of all string characters past first NULL char.
    if (pZero < pEnd)
        *pZero = 0;
    return true;
}

bool __stdcall KdComDispatcher::KdSendPacket(ULONG PacketType,
    PKD_BUFFER FirstBuffer,
    PKD_BUFFER SecondBuffer,
    PKD_CONTEXT KdContext)
{
    m_PacketLogger.OnSendReceivePacket(g_pReporter->GetStatusPointer()->LogAllPackets != 0,
        true,
        PacketType,
        FirstBuffer,
        SecondBuffer,
        KdContext);

    if (!m_RandomValidContextRecord.GetSize() && (PacketType == 2))
        if (FirstBuffer && FirstBuffer->pData)
            if (*((unsigned *)FirstBuffer->pData) == DbgKdReadControlSpaceApi)
                if (SecondBuffer && SecondBuffer->Length && SecondBuffer->pData)
                {
                    m_RandomValidContextRecord.EnsureSize(SecondBuffer->Length);
                    memcpy(m_RandomValidContextRecord.GetData(), SecondBuffer->pData, SecondBuffer->Length);
                    m_RandomValidContextRecord.SetSize(SecondBuffer->Length);
                }

#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
    g_pReporter->GetStatusPointer()->OSDetected = true;
#endif
#ifdef KDCLIENT_ENABLE_TRACE_ASSIST
    switch (PacketType)
    {
    case KdPacketType3:
        if (FirstBuffer && SecondBuffer && (*((ULONG *)FirstBuffer->pData) == 0x3230))
        {
            FixVerifierUnicodeReport(SecondBuffer);
            if (g_pReporter->GetStatusPointer()->TraceAssistUpdatePending)
            {
                m_TraceAssistant.ReloadParams();
                g_pReporter->GetStatusPointer()->TraceAssistUpdatePending = false;
            }
            if (m_TraceAssistant.TraceLine((char *)SecondBuffer->pData, SecondBuffer->Length))
            {
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
                g_pReporter->GetStatusPointer()->BytesSent += (FirstBuffer->Length + SecondBuffer->Length + sizeof(KD_PACKET_HEADER) + 1);
                g_pReporter->GetStatusPointer()->PacketsSent++;
#endif
                return true;
            }
        }
        break;
    }
#endif

    ASSERT(FirstBuffer);
    KD_PACKET_HEADER header;
    unsigned checksum = KdpComputeChecksum(FirstBuffer->pData, FirstBuffer->Length);
    unsigned totalLength = FirstBuffer->Length;
    if (SecondBuffer)
    {
        totalLength += SecondBuffer->Length;
        checksum += KdpComputeChecksum(SecondBuffer->pData, SecondBuffer->Length);
    }

    header.Signature = '0000';
    header.PacketType = (USHORT)PacketType;
    header.Checksum = checksum;
    header.TotalDataLength = totalLength;
    KdCompNumberRetries = KdCompRetryCount;

    if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdCom))
    {
        TCHAR tsz[512];
        _sntprintf(tsz, __countof(tsz), _T("Sending normal packet (type = %d, id = %08X, len = %d)...\r\n"),
            header.PacketType,
            KdCompNextPacketIdToSend,
            header.TotalDataLength);
        g_pReporter->LogLineIfEnabled(tsz);
    }

    unsigned RetryNumber = 0;

    for (;;)
    {
        //SIMPLIFY: The classical KdCompRetryCount-related scheme is replaced by infinite retrying with
        //fallback to VM. In case we get looped here (inside GuestRPC dispatcher), VMWare hangs.
        //if (!KdCompNumberRetries)
        if (RetryNumber)
        {
            /*bool bCanDrop = false;
            switch (PacketType)
            {
            case KdPacketType3:
                bCanDrop = *((ULONG *)FirstBuffer->pData) == 0x3230;
                break;
            case KdPacketType7:
                bCanDrop = *((ULONG *)FirstBuffer->pData) == 0x3031;
                break;
            case KdPacketType11:
                bCanDrop = *((ULONG *)FirstBuffer->pData) == 0x3430;
                break;
            }
            if (bCanDrop)*/
            {
                //As returning false will result in automatic packet retrying, we do not loop here forever any more, if a packet send failed.
                //Instead, we return false and wait for this call to be retried by KDVMWARE.DLL.
                KD_DEBUGGER_NOT_PRESENT = TRUE;
                KdResetPacketNumbering(true);
                if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdCom))
                    g_pReporter->LogLineIfEnabled(_T("Acknowledgment timeout\r\n"));
                return false;
            }
        }

        if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdComAll))
        {
            TCHAR tsz[512];
            _sntprintf(tsz, __countof(tsz), _T("KdSendPacket(): sending (%d)\r\n"),
                header.PacketType);
            g_pReporter->LogLineIfEnabled(tsz);
        }

        header.PacketID = KdCompNextPacketIdToSend;
        m_Pipe.Send(&header, sizeof(header));
        m_Pipe.Send(FirstBuffer->pData, FirstBuffer->Length);
        if (SecondBuffer && SecondBuffer->Length)
            m_Pipe.Send(SecondBuffer->pData, SecondBuffer->Length);
        unsigned char ch = 0xAA;
        m_Pipe.Send(&ch, 1);
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
        g_pReporter->GetStatusPointer()->DebuggerConnected = m_Pipe.IsClientConnected();
#endif
        switch (KdReceivePacket(KdPacketAcknowledge, NULL, NULL, NULL, KdContext, NULL))
        {
        case KD_RECV_CODE_TIMEOUT:
            KdCompNumberRetries--;
            RetryNumber++;
            break;
        case KD_RECV_CODE_OK:
            KdCompNextPacketIdToSend &= 0xFFFFFFF7;
            KdCompRetryCount = KdContext->RetryCount;
            if (++m_PacketsSentFromLastReset >= KdMinPacketCountForSuccessfulLink)
                m_SequentialResetCount = 0;
            if (KdTraceEnabled && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTraceKdCom))
                g_pReporter->LogLineIfEnabled(_T("Packet acknowledged\r\n"));
#ifdef KDCLIENT_REPORT_PERFORMANCE_INFORMATION
            g_pReporter->GetStatusPointer()->BytesSent += (header.TotalDataLength + sizeof(KD_PACKET_HEADER) + 1);
            g_pReporter->GetStatusPointer()->PacketsSent++;
#endif
            return true;
        }
    }
}

void KdComDispatcher::ReportProtocolVersionError(unsigned GuestVersion, unsigned HostVersion)
{
    g_pReporter->GetStatusPointer()->ProtocolMismatchStatus = { GuestVersion, HostVersion };
}

//-------------------------------------------------------------------------------------------------------------------------------------

typedef unsigned SystemTerminationPacket[60];

typedef struct _DBGKD_LOAD_SYMBOLS64
{
    ULONG PathNameLength;
    ULONG64 BaseOfDll;
    ULONG64 ProcessId;
    ULONG CheckSum;
    ULONG SizeOfImage;
    BOOLEAN UnloadSymbols;
} DBGKD_LOAD_SYMBOLS64, *PDBGKD_LOAD_SYMBOLS64;

static void InitializeSystemTerminationPacket(SystemTerminationPacket Packet)
{
    memset(Packet, -1, sizeof(SystemTerminationPacket));
    Packet[0] = 0x00003031;	//DbgKdLoadSymbolsStateChange
    Packet[1] = 6;			//ProcessorLevel
    Packet[2] = 1;			//NumberOfProcessors

    //Thread and ProgramCounter fields are left uninitialized, as this should not matter when signalling Windows termination.

    const int kDbgkdLoadSymbols64Offset = 8;

    PDBGKD_LOAD_SYMBOLS64 pSym = (PDBGKD_LOAD_SYMBOLS64)&Packet[kDbgkdLoadSymbols64Offset];

    pSym->PathNameLength = 0;
    pSym->BaseOfDll = -1;
    pSym->ProcessId = 0;
    pSym->CheckSum = 0;
    pSym->SizeOfImage = 0;
    pSym->UnloadSymbols = TRUE;
}


//WARNING! This code is experimental and serves as a workaround for MS debug engine hanging, when a remote machine stops responding.
//The main idea is to send a 'NTOSKRNL unloaded' message when a VM process is terminating, or DLL is unloading.
//Everything works perfectly, if the target is running at that time, however, if the target is stopped, the message will not be
//processed normally, as the debugger does not expect it. Simple resending does not help, as too many copies of that message
//may drive WinDbg into a permanent 'out of sync' state. This code tries to detect, how and when to send such message.
void KdComDispatcher::SimulateWindowsTermination()
{
    SystemTerminationPacket packet;
    InitializeSystemTerminationPacket(packet);

    m_Notifier.SignalSessionTerminationAndWaitForAck();
    m_PacketLogger.OnWindowsTerminationSimulated();

    KD_BUFFER first = { 0, }, second = { 0, };
    first.Length = sizeof(packet);
    first.pData = (PUCHAR)packet;
    KD_CONTEXT ctx = { 0, };

    bool RequireAdditionalTerminationPacket = false;

    while (!KdSendPacket(7, &first, &second, &ctx))
    {
        if (!m_Pipe.IsClientConnected())
        {
            m_PacketLogger.OnWindowsTerminationSimDone("debugger disconnected");
            return;
        }
        RequireAdditionalTerminationPacket = true;	//Target is not running and debugger is not ready to receive a packet. We'll repeat it later.
    }

    //Packet type 2 always has buffer 1 of size 0x38
    static char szBuf1[0x38], szBuf2[KdMaxBufferSize];

    ULONG unused = 0;

    BazisLib::DateTime dtStart = BazisLib::DateTime::Now();
    const int NoControlPacketTimeout = 1500;

    for (;;)
    {
        if (!m_Pipe.IsClientConnected())
        {
            m_PacketLogger.OnWindowsTerminationSimDone("debugger disconnected");
            return;
        }

        first.MaxLength = sizeof(szBuf1);
        first.pData = (PUCHAR)szBuf1;
        second.MaxLength = sizeof(szBuf2);
        second.pData = (PUCHAR)szBuf2;

        KD_RECV_CODE code = KdReceivePacket(2, &first, &second, &unused, &ctx, NULL);
        if (!m_Pipe.IsClientConnected())
        {
            m_PacketLogger.OnWindowsTerminationSimDone("debugger disconnected");
            return;
        }

        if (code == KD_RECV_CODE_TIMEOUT)
        {
            if ((BazisLib::DateTime::Now() - dtStart).GetTotalMilliseconds() < NoControlPacketTimeout)
                continue;

            m_PacketLogger.OnWindowsTerminationSimDone("timeout");
            return;
        }
        if (code != KD_RECV_CODE_OK)
            continue;

        if ((*((unsigned *)szBuf1) == DbgKdContinueApi2) || (*((unsigned *)szBuf1) == DbgKdContinueApi))
        {
            if (RequireAdditionalTerminationPacket)
            {
                first.Length = sizeof(packet);
                first.pData = (PUCHAR)packet;
                if (!KdSendPacket(7, &first, &second, &ctx))
                {
                    m_PacketLogger.OnWindowsTerminationSimDone("type 7 packet not acknowledged");
                    return;
                }
                RequireAdditionalTerminationPacket = false;
                continue;
            }
            m_PacketLogger.OnWindowsTerminationSimDone("continue command received");
            return;
        }

        ((unsigned *)szBuf1)[2] = 0;	//Set ReturnStatus in DBGKD_MANIPULATE_STATE64 to STATUS_SUCCESS

        if ((*((unsigned *)szBuf1) == DbgKdReadControlSpaceApi))
        {
            second.Length = (USHORT)m_RandomValidContextRecord.GetSize();
            second.pData = (PUCHAR)m_RandomValidContextRecord.GetData();
        }

        if ((*((unsigned *)szBuf1) == DbgKdSetContextApi))
        {
            second.Length = 0;
            second.pData = NULL;
        }


        //If we have received a type 2 packet, other than 'continue', simply echo the packet back.
        //Packet-level layer of WinDbg will return a successful status code, while higher level will
        //most likely treat it as an error. However, then WinDbg will receive our termination packet
        //and will disconnect from session.
        KdSendPacket(2, &first, &second, &ctx);
        RequireAdditionalTerminationPacket = true;
    }
    m_PacketLogger.OnWindowsTerminationSimDone("unexpected");
}