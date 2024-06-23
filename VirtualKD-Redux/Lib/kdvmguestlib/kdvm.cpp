/*! \file kdvm.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: kdvm.cpp,v 1.11 2009-06-07 10:51:22 Administrator Exp $
    \brief Contains guest side of KdXXX() marshalling functions
*/

#include "stdafx.h"
#include "kdxxx.h"
#include "vmwrpc.h"
#include "kdvm.h"
#include "kdrpc.h"
#include "vboxrpc.h"

// There's an old bug dating back to around XP SP2 where calling MmGetSystemRoutineAddress on a non-existant routine name
// can result in a BSOD. It's been fixed since Vista but because VirtualKD-Redux will ALWAYS support XP RTM and beyond, we can never use that function.
#include "moduleapi.h"

#define SGD_KD_DEBUGGER_ENABLED  (10)

namespace
{
    char g_PacketBuffer[131072 + 1024];
}

//#define VIRTUALKD_BENCHMARK_AND_EXIT
//Driver v1.1 does 100K test requests in ~7.21 sec (~13K requests/sec) under VMWare
//Driver v2.0 does 1M test requests in ~16 sec (~62K requests/sec) under VirtualBox

typedef BufferedRPCChannel<g_PacketBuffer, sizeof(g_PacketBuffer)> VMWareChannel;
typedef BufferedVBoxChannel<g_PacketBuffer, sizeof(g_PacketBuffer)> VBoxChannel;

extern "C" void HalInitSystem(void *, void *);

#ifndef _AMD64_
extern "C" void __declspec(naked) __stdcall DllEntryPoint()
{
    __asm
    {
        jmp HalInitSystem
    }
}
#endif

#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
static int s_PacketPollRequestsToSkip = 0;
#endif

static KD_BUFFER s_EmptyBuffer;
static KD_CONTEXT s_EmptyKdContext;

volatile LONG KdVmActiveCallCount = 0;
static bool s_bVBoxDetected = false;

ULONG KdVMGetActiveCallCount()
{
    return KdVmActiveCallCount;
}

static PIMAGE_NT_HEADERS ImageNtHeader(PVOID pModuleAddress)
{
    PIMAGE_NT_HEADERS pNtHeaders = NULL;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pModuleAddress;

    __try
    {
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            __leave;
        }

        PIMAGE_NT_HEADERS pNtHeadersTmp = (PIMAGE_NT_HEADERS)((PUCHAR)pModuleAddress + pDosHeader->e_lfanew);
        if (pNtHeadersTmp->Signature != IMAGE_NT_SIGNATURE ||
            pNtHeadersTmp->OptionalHeader.Magic !=
#ifdef _WIN64
            IMAGE_NT_OPTIONAL_HDR64_MAGIC
#else
            IMAGE_NT_OPTIONAL_HDR32_MAGIC
#endif
            ||
            pNtHeadersTmp->FileHeader.Machine !=
#ifdef _WIN64
            IMAGE_FILE_MACHINE_AMD64
#else
            IMAGE_FILE_MACHINE_I386
#endif
            )
        {
            __leave;
        }

        pNtHeaders = pNtHeadersTmp;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return pNtHeaders;
}

static PVOID GetModuleBaseAddress(PVOID pAddr)
{
    CHAR* pBase = (char*)(((ULONG_PTR)(void*)pAddr / PAGE_SIZE) * PAGE_SIZE);
    for (int i = 0; i < (1024 * 1024 * 50) / PAGE_SIZE; i++)
    {
        PVOID pCurAddr = pBase - PAGE_SIZE * i;

        __try
        {
            if (!MmIsAddressValid(pCurAddr))
            {
                continue;
            }

            PIMAGE_NT_HEADERS pHeaders = ImageNtHeader(pCurAddr);
            if (pHeaders)
            {
                return pCurAddr;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }

    return NULL;
}

static BOOLEAN GetKdDebuggerEnabled()
{
    typedef void(*fnRtlGetSystemGlobalData)(ULONGLONG, PVOID, ULONGLONG);
    static BOOLEAN bCheckedForFunc = FALSE;
    static fnRtlGetSystemGlobalData RtlGetSystemGlobalData = nullptr;

    if (!bCheckedForFunc)
    {
        bCheckedForFunc = TRUE;
        RtlGetSystemGlobalData = (fnRtlGetSystemGlobalData)KernelGetProcAddress(GetModuleBaseAddress(ExAllocatePool), "RtlGetSystemGlobalData");
    }

    if (RtlGetSystemGlobalData)
    {
        BOOLEAN bVal = 0;
        RtlGetSystemGlobalData(SGD_KD_DEBUGGER_ENABLED, &bVal, sizeof(bVal));
        return bVal;
    }
    else
    {
        return SharedUserData->KdDebuggerEnabled;
    }
}

static void SetKdDebuggerEnabled(BOOLEAN bVal)
{
    typedef void(*fnRtlSetSystemGlobalData)(ULONGLONG, PVOID, ULONGLONG);
    static BOOLEAN bCheckedForFunc = FALSE;
    static fnRtlSetSystemGlobalData RtlSetSystemGlobalData = nullptr;

    if (!bCheckedForFunc)
    {
        bCheckedForFunc = TRUE;
        RtlSetSystemGlobalData = (fnRtlSetSystemGlobalData)KernelGetProcAddress(GetModuleBaseAddress(ExAllocatePool), "RtlSetSystemGlobalData");
    }

    if (RtlSetSystemGlobalData)
    {
        RtlSetSystemGlobalData(SGD_KD_DEBUGGER_ENABLED, &bVal, sizeof(bVal));
    }
    else
    {
        SharedUserData->KdDebuggerEnabled = bVal;
    }
}


class InterlockedIncrementer
{
private:
    volatile LONG *m_pPtr;

public:
    inline InterlockedIncrementer(volatile LONG *pPtr)
        : m_pPtr(pPtr)
    {
        InterlockedIncrement(m_pPtr);
    }

    ~InterlockedIncrementer()
    {
        InterlockedDecrement(m_pPtr);
    }
};

template <class DefaultRPCChannel> class ChannelHelper
{
public:
    static bool TestConnectionOnChannel(DefaultRPCChannel &chn)
    {
        unsigned char t = TestConnection;
        unsigned char arr[KDRPC_TEST_BUFFER_SIZE];
        for (int i = 0; i < sizeof(arr); i++)
            arr[i] = (unsigned char)i;
        if (!chn.Valid())
            return false;
        if (!chn.PrepareSend(sizeof(arr) + sizeof(g_szRPCCommandHeader)))
            return false;
        if (!chn.SendPartial(g_szRPCCommandHeader, sizeof(g_szRPCCommandHeader) - 1))
            return false;
        if (!chn.SendPartial(&t, sizeof(t)))
            return false;
        if (!chn.SendPartial(arr, sizeof(arr)))
            return false;
        unsigned rsize = chn.GetReplySize();
        int refsize = ((sizeof(g_szRPCReplySignature) + sizeof(arr) - 1) + 2);
        if (rsize != refsize)
            return false;
        if (!chn.Receive(arr, (sizeof(g_szRPCReplySignature) - 1) + 2))
            return false;
        if (arr[0] != '1')
            return false;
        if (memcmp(arr + 2, g_szRPCReplySignature, sizeof(g_szRPCReplySignature) - 1))
            return false;
        if (!chn.Receive(arr, sizeof(arr)))
            return false;
        if (!chn.EndReceive())
            return false;
        for (int i = 0; i < KDRPC_TEST_BUFFER_SIZE; i++)
            if (arr[i] != (unsigned char)(i ^ 0x55))
                return false;
        return true;
    }

#ifdef VIRTUALKD_BENCHMARK_AND_EXIT
#define CONNECTION_TEST_ROUNDS	100000
#else
#define CONNECTION_TEST_ROUNDS	100
#endif

    static bool TestConnectionWithHost()
    {
        //SendableKdBuffer should be KD_BUFFER minus pData field (used in serialization). If not, an error will popup here:
#ifndef _AMD64_
        char _unused0[(int)(&((KD_BUFFER *)0)->pData) == sizeof(SendableKdBuffer)];
        UNREFERENCED_PARAMETER(_unused0);
#endif

#ifdef VIRTUALKD_BENCHMARK_AND_EXIT
        DbgPrint("VirtualKD: Starting connection self-test (%d rounds)...\n", CONNECTION_TEST_ROUNDS);
#endif
        for (int j = 0; j < 2; j++)
        {
            DefaultRPCChannel chn;
            for (int i = 0; i < (CONNECTION_TEST_ROUNDS / 2); i++)
            {
                if (!TestConnectionOnChannel(chn))
                {
#ifdef VIRTUALKD_BENCHMARK_AND_EXIT
                    DbgPrint("VirtualKD: Connection self-test failed on round %d!\n", i);
#endif
                    return false;
                }
            }
        }
#ifdef VIRTUALKD_BENCHMARK_AND_EXIT
        DbgPrint("VirtualKD: Connection self-test successful!\n");
        return false;
#endif
        return true;
    }

    static NTSTATUS NegotiateProtocolVersions()
    {
        DefaultRPCChannel channel;
        if (channel.NeedRestartInVBoxMode())
        {
            s_bVBoxDetected = true;
            return STATUS_RETRY;
        }
        channel.PrepareSend(5 + sizeof(g_szRPCCommandHeader) - 1);
        char commandType = VersionReport;
        unsigned int version = VIRTUALKD_REDUX_VER_INT;
        if (!channel.SendPartial(&g_szRPCCommandHeader, sizeof(g_szRPCCommandHeader) - 1))
            return STATUS_CONNECTION_REFUSED;
        if (!channel.SendPartial(&commandType, 1))
            return STATUS_CONNECTION_REFUSED;
        channel.SendPartial(&version, 4);
        if (channel.GetReplySize() != (4 + 2 + sizeof(g_szRPCReplySignature) - 1))
            return STATUS_CONNECTION_REFUSED;
        char pfx[sizeof(g_szRPCReplySignature)];
        if (!channel.Receive(pfx, 2))
            return STATUS_CONNECTION_REFUSED;
        if ((pfx[0] != '1') || (pfx[1] != ' '))
            return STATUS_CONNECTION_REFUSED;
        if (!channel.Receive(pfx, sizeof(g_szRPCReplySignature) - 1))
            return STATUS_CONNECTION_REFUSED;
        if (memcmp(pfx, g_szRPCReplySignature, sizeof(g_szRPCReplySignature) - 1))
            return STATUS_CONNECTION_REFUSED;
        channel.Receive(&version, 4);
#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
        if (version != VIRTUALKD_REDUX_VER_INT)
            return STATUS_PROTOCOL_NOT_SUPPORTED;
#else
        if (version < VIRTUALKD_REDUX_VER_INT)
            return STATUS_PROTOCOL_NOT_SUPPORTED;
#endif
        return STATUS_SUCCESS;
    }

    static NTSTATUS KdDebuggerInitialize0(PVOID lpLoaderParameterBlock)
    {
#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
        s_PacketPollRequestsToSkip = 0;
#endif
        memset(&s_EmptyBuffer, 0, sizeof(s_EmptyBuffer));
        memset(&s_EmptyKdContext, 0, sizeof(s_EmptyKdContext));
        KdVmActiveCallCount = 0;
        NTSTATUS status = NegotiateProtocolVersions();
        if (!NT_SUCCESS(status))
            return status;
        if (!TestConnectionWithHost())
            return STATUS_CONNECTION_REFUSED;
        return STATUS_SUCCESS;
    }

    static KD_RECV_CODE KdReceivePacket(
        __in ULONG PacketType,
        __inout_opt PKD_BUFFER FirstBuffer,
        __inout_opt PKD_BUFFER SecondBuffer,
        __out_opt PULONG PayloadBytes,
        __inout_opt PKD_CONTEXT KdContext)
    {
#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
        if ((PacketType == KdCheckForAnyPacket) && (s_PacketPollRequestsToSkip > 0))
        {
            s_PacketPollRequestsToSkip--;
            return KD_RECV_CODE_TIMEOUT;
        }
#endif

        InterlockedIncrementer ctr(&KdVmActiveCallCount);
        DefaultRPCChannel channel;
        char commandType = ReceivePacket;
        ULONG globals = kKDDebuggerEnabledValueAvailable;
        if (KD_DEBUGGER_NOT_PRESENT)
            globals |= kKDDebuggerNotPresentSet;
        globals |= (GetKdDebuggerEnabled() << 8);

        if (!channel.PrepareSend(sizeof(g_szRPCCommandHeader) + 2 * sizeof(ULONG) + 2 * sizeof(SendableKdBuffer) + sizeof(KD_CONTEXT)))
            return KD_RECV_CODE_FAILED;
        if (!channel.SendPartial(&g_szRPCCommandHeader, sizeof(g_szRPCCommandHeader) - 1))
            return KD_RECV_CODE_FAILED;
        if (!channel.SendPartial(&commandType, 1))
            return KD_RECV_CODE_FAILED;
        if (!channel.SendPartial(&PacketType, sizeof(ULONG)))
            return KD_RECV_CODE_FAILED;
        if (!channel.SendPartial(&globals, sizeof(ULONG)))
            return KD_RECV_CODE_FAILED;
        if (!channel.SendPartial(FirstBuffer ? FirstBuffer : &s_EmptyBuffer, sizeof(SendableKdBuffer)))
            return KD_RECV_CODE_FAILED;
        if (!channel.SendPartial(SecondBuffer ? SecondBuffer : &s_EmptyBuffer, sizeof(SendableKdBuffer)))
            return KD_RECV_CODE_FAILED;
        if (!channel.SendPartial(KdContext ? KdContext : &s_EmptyKdContext, sizeof(KD_CONTEXT)))
            return KD_RECV_CODE_FAILED;
        unsigned recvSize = channel.GetReplySize();
        unsigned minReplySize = (sizeof(g_szRPCReplySignature) + sizeof(SendableKdBuffer) * 2 + sizeof(KD_CONTEXT) + KDRPC_RECV_RETURNED_ULONGS * sizeof(ULONG) + 2);
        if ((recvSize == -1) || (recvSize < minReplySize))
            return KD_RECV_CODE_FAILED;
        KD_BUFFER tempBuffer;
        KD_CONTEXT tempContext;
        char pfx[sizeof(g_szRPCReplySignature)];
        if (!channel.Receive(pfx, 2))
            return KD_RECV_CODE_FAILED;
        if ((pfx[0] != '1') || (pfx[1] != ' '))
            return KD_RECV_CODE_FAILED;
        if (!channel.Receive(pfx, sizeof(g_szRPCReplySignature) - 1))
            return KD_RECV_CODE_FAILED;
        if (!channel.Receive(&commandType, 1))
            return KD_RECV_CODE_FAILED;
        if ((memcmp(pfx, g_szRPCReplySignature, sizeof(g_szRPCReplySignature) - 1) || (commandType != ReceivePacket)))
            return KD_RECV_CODE_FAILED;
        if (!channel.Receive(FirstBuffer ? FirstBuffer : &tempBuffer, sizeof(SendableKdBuffer)))
            return KD_RECV_CODE_FAILED;
        if (!channel.Receive(SecondBuffer ? SecondBuffer : &tempBuffer, sizeof(SendableKdBuffer)))
            return KD_RECV_CODE_FAILED;
        if (!channel.Receive(KdContext ? KdContext : &tempContext, sizeof(KD_CONTEXT)))
            return KD_RECV_CODE_FAILED;
        ULONG params[KDRPC_RECV_RETURNED_ULONGS];
        if (!channel.Receive(params, sizeof(ULONG) * KDRPC_RECV_RETURNED_ULONGS))
            return KD_RECV_CODE_FAILED;
        if (recvSize != (sizeof(g_szRPCReplySignature) + params[2] + params[3] + sizeof(SendableKdBuffer) * 2 + sizeof(KD_CONTEXT) + KDRPC_RECV_RETURNED_ULONGS * sizeof(ULONG) + 2))
            return KD_RECV_CODE_FAILED;

        if (FirstBuffer && FirstBuffer->pData && (FirstBuffer->MaxLength >= params[2]))
        {
            if (!channel.Receive(FirstBuffer->pData, params[2]))
                return KD_RECV_CODE_FAILED;
        }
        if (SecondBuffer && SecondBuffer->pData && (SecondBuffer->MaxLength >= params[3]))
        {
            if (!channel.Receive(SecondBuffer->pData, params[3]))
                return KD_RECV_CODE_FAILED;
        }
        if (PayloadBytes)
            *PayloadBytes = params[1];
        if (!channel.EndReceive())
            return KD_RECV_CODE_FAILED;

        globals = params[4];
        KD_DEBUGGER_NOT_PRESENT = ((globals & kKDDebuggerNotPresentSet) != 0);
        if (globals & kKDDebuggerEnabledValueAvailable)
            SetKdDebuggerEnabled((globals >> 8) & 0xFF);

#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
        s_PacketPollRequestsToSkip = params[5];
#endif

        return (KD_RECV_CODE)params[0];
        }

    static VOID KdSendPacket(
        __in ULONG PacketType,
        __in PKD_BUFFER FirstBuffer,
        __in_opt PKD_BUFFER SecondBuffer,
        __inout PKD_CONTEXT KdContext)
    {
        for (;;)
        {
            InterlockedIncrementer ctr(&KdVmActiveCallCount);
            DefaultRPCChannel channel;
            char commandType = SendPacket;
            unsigned Size1 = 0, Size2 = 0;
            ULONG globals = kKDDebuggerEnabledValueAvailable;
            if (KD_DEBUGGER_NOT_PRESENT)
                globals |= kKDDebuggerNotPresentSet;
            globals |= (GetKdDebuggerEnabled() << 8);
            if (FirstBuffer)
                Size1 = FirstBuffer->Length;
            if (SecondBuffer)
                Size2 = SecondBuffer->Length;
            if (!channel.PrepareSend(sizeof(g_szRPCCommandHeader) + sizeof(ULONG) * KDRPC_SEND_PASSED_ULONGS + 2 * sizeof(SendableKdBuffer) + sizeof(KD_CONTEXT) + Size1 + Size2))
                return;
            if (!channel.SendPartial(&g_szRPCCommandHeader, sizeof(g_szRPCCommandHeader) - 1))
                return;
            if (!channel.SendPartial(&commandType, 1))
                return;
            if (!channel.SendPartial(FirstBuffer ? FirstBuffer : &s_EmptyBuffer, sizeof(SendableKdBuffer)))
                return;
            if (!channel.SendPartial(SecondBuffer ? SecondBuffer : &s_EmptyBuffer, sizeof(SendableKdBuffer)))
                return;
            if (!channel.SendPartial(KdContext ? KdContext : &s_EmptyKdContext, sizeof(KD_CONTEXT)))
                return;
            ULONG params[KDRPC_SEND_PASSED_ULONGS] = { PacketType, Size1, Size2, globals };
            if (!channel.SendPartial(params, sizeof(params)))
                return;
            if (Size1)
                if (!channel.SendPartial(FirstBuffer->pData, Size1))
                    return;
            if (Size2)
                if (!channel.SendPartial(SecondBuffer->pData, Size2))
                    return;
            unsigned recvSize = channel.GetReplySize();
            if (recvSize != (sizeof(g_szRPCReplySignature) + 2 + sizeof(KD_CONTEXT) + sizeof(ULONG)))
                return;
            char pfx[sizeof(g_szRPCReplySignature)];
            if (!channel.Receive(pfx, 2))
                return;
            if ((pfx[0] != '1') || (pfx[1] != ' '))
                return;
            if (!channel.Receive(pfx, sizeof(g_szRPCReplySignature) - 1))
                return;
            if (!channel.Receive(&commandType, 1))
                return;
            if ((memcmp(pfx, g_szRPCReplySignature, sizeof(g_szRPCReplySignature) - 1) || (commandType != SendPacket)))
                return;
            KD_CONTEXT tempContext;
            if (!channel.Receive(KdContext ? KdContext : &tempContext, sizeof(KD_CONTEXT)))
                return;
            if (!channel.Receive(&globals, sizeof(ULONG)))
                return;
            if (!channel.EndReceive())
                return;

            KD_DEBUGGER_NOT_PRESENT = ((globals & kKDDebuggerNotPresentSet) != 0);
            if (globals & kKDDebuggerEnabledValueAvailable)
                SetKdDebuggerEnabled((globals >> 8) & 0xFF);

            //Retry request if a droppable packet was not delivered
            if (globals & kRetryKdSendPacket)
                continue;
            break;
        }
    }
    };

NTSTATUS __stdcall KdInitialize(int mode, PVOID lpLoaderParameterBlock, void *)
{
    if (mode == 0)
        return KdDebuggerInitialize0(lpLoaderParameterBlock);
    else
        return STATUS_SUCCESS;
}

NTSTATUS __stdcall KdPower(void *, void *)
{
    return STATUS_NOT_SUPPORTED;
}

NTSTATUS __stdcall KdDebuggerInitialize0(PVOID lpLoaderParameterBlock)
{
    NTSTATUS st;

    PVOID pAddr = GetModuleBaseAddress(KdDebuggerInitialize0);
    PIMAGE_NT_HEADERS pHeaders = ImageNtHeader(pAddr);
    if (pHeaders)
    {
        //Prevent the current module from being relocated to a different address and breaking the physical/virtual address mapping
        //for the KD buffer.
        pHeaders->FileHeader.Characteristics |= IMAGE_FILE_RELOCS_STRIPPED;
    }

    s_bVBoxDetected = false;
    st = ChannelHelper<VMWareChannel>::KdDebuggerInitialize0(lpLoaderParameterBlock);
    if ((st == STATUS_RETRY) && s_bVBoxDetected)
        st = ChannelHelper<VBoxChannel>::KdDebuggerInitialize0(lpLoaderParameterBlock);
    return STATUS_SUCCESS;
}

VOID __stdcall KdSendPacket(__in ULONG PacketType,
    __in PKD_BUFFER FirstBuffer,
    __in_opt PKD_BUFFER SecondBuffer,
    __inout PKD_CONTEXT KdContext)
{
    if (s_bVBoxDetected)
        return ChannelHelper<VBoxChannel>::KdSendPacket(PacketType, FirstBuffer, SecondBuffer, KdContext);
    else
        return ChannelHelper<VMWareChannel>::KdSendPacket(PacketType, FirstBuffer, SecondBuffer, KdContext);
}

KD_RECV_CODE __stdcall KdReceivePacket(__in ULONG PacketType,
    __inout_opt PKD_BUFFER FirstBuffer,
    __inout_opt PKD_BUFFER SecondBuffer,
    __out_opt PULONG PayloadBytes,
    __inout_opt PKD_CONTEXT KdContext)
{
    if (s_bVBoxDetected)
        return ChannelHelper<VBoxChannel>::KdReceivePacket(PacketType, FirstBuffer, SecondBuffer, PayloadBytes, KdContext);
    else
        return ChannelHelper<VMWareChannel>::KdReceivePacket(PacketType, FirstBuffer, SecondBuffer, PayloadBytes, KdContext);
}
