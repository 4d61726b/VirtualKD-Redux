/*! \file vmwrpc.h
    \author Ivan Shcherbakov (Bazis)
    $Id: vmwrpc.h,v 1.9 2009-06-07 10:00:24 Administrator Exp $
    \brief Contains inline classes for performing Guest-to-Host VMWare RPC calls (\ref guestrpc)
    Further information about GuestRPC can be found here: http://chitchat.at.infoseek.co.jp/vmware/backdoor.html
*/

#pragma once

//#ifdef _DEBUG
#define VMWRPC_PARAMETER_CHECKING
//#endif

#pragma pack(push, 4)
//! Contains state variables for a \ref guestrpc channel.
class VMWareRpcData
{
public:
    unsigned m_Cookie1;
    unsigned m_Cookie2;
    unsigned m_ChannelNumber;

    unsigned m_ReplyId;
    unsigned m_CachedReplySize;
};
#pragma pack(pop)

#ifdef _AMD64_
extern "C"
{
    //Channel manipulation functions are defined in vmwrpc64.asm
    int VMWareOpenChannel(VMWareRpcData *pChannel);
    bool VMWareCloseChannel(VMWareRpcData *pChannel);
    bool VMWareSendRequestLength(VMWareRpcData *pChannel, size_t sizeNotCountingNULL);
    bool VMWareSendPartialData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, const void *pData);
    bool VMWareSendData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, const void *pData);
    unsigned  VMWareGetReplySize(VMWareRpcData *pChannel);
    bool  VMWareRecvData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, void *pData);
    bool  VMWareEndReceive(VMWareRpcData *pChannel);
}
#endif

//! Contains low-level GuestRPC-related routines written in assembly language
/*
    \remarks See details on VMWare backdoor commands here: http://chitchat.at.infoseek.co.jp/vmware/backdoor.html
*/
namespace VMWareRPC
{
#ifndef _AMD64_
    //! Opens a GuestRPC channel.
    static unsigned __declspec(naked) __fastcall OpenChannel(VMWareRpcData *pChannel)
    {
        __asm
        {
            pushad
            mov eax, 0x564D5868
            mov ebx, 0xC9435052
            xchg ecx, ebp
            mov ecx, 0x0000001E
            mov edx, 0x5658
            out dx, eax
            test ecx, 0x00010000
            jz vmware_rpc_open_channel_error
            mov[ebp].m_Cookie1, esi
            mov[ebp].m_Cookie2, edi
            mov[ebp].m_ChannelNumber, edx
            popad
            xor eax, eax
            inc eax
            ret
            vmware_rpc_open_channel_error :
            mov edx, 0x5658
                in eax, dx
                cmp eax, 'XOBV'	//'VBOX' signature is returned, if this call was made under VirtualBox
                jz vmware_rpc_open_channel_vbox
                popad
                xor eax, eax
                ret
                vmware_rpc_open_channel_vbox :
            popad
                xor eax, eax
                add eax, 2
                ret
        }
    }

    //! Closes a GuestRPC channel
    static bool __declspec(naked) __fastcall CloseChannel(VMWareRpcData *pChannel)
    {
        __asm
        {
            pushad
            mov eax, 0x564D5868
            mov edx, [ecx].m_ChannelNumber
            or edx, 0x5658
            mov esi, [ecx].m_Cookie1
            mov edi, [ecx].m_Cookie2
            mov ecx, 0x0006001E
            out dx, eax
            cmp ecx, 0x00010000
            popad
            setz al
            and eax, 1
            ret
        }
    }

    //! Sends the length of a GuestRPC request
    static bool __declspec(naked) __fastcall SendRequestLength(VMWareRpcData *pChannel, size_t sizeNotCountingNULL)
    {
        __asm
        {
            pushad
            mov eax, 0x564D5868
            mov ebx, edx
            mov edx, [ecx].m_ChannelNumber
            or edx, 0x5658
            mov esi, [ecx].m_Cookie1
            mov edi, [ecx].m_Cookie2
            mov ecx, 0x0001001E
            out dx, eax
            cmp ecx, 0x00810000
            jnz vmware_rpc_send_size_error

            popad
            xor eax, eax
            inc eax
            ret

            vmware_rpc_send_size_error :
            popad
                xor eax, eax
                ret
        }
    }

    //! Sends a portion of GuestRPC request data. Works similar to send() function in sockets.
    static bool __declspec(naked) __fastcall SendPartialData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, const void *pData)
    {
        __asm
        {
            pushad
            mov eax, 0x564D5868
            //ecx <- byte count to send (minus null)
            xchg ecx, edx
            mov ebx, 0x00010000
            mov esi, [esp + 0x24]	//pData
            mov edi, [edx].m_Cookie2
            mov ebp, [edx].m_Cookie1
            mov edx, [edx].m_ChannelNumber
            or edx, 0x5659
            rep outsb

            cmp ebx, 0x00010000
            jnz vmware_rpc_send_partial_error

            popad
            xor eax, eax
            inc eax
            ret 4

            vmware_rpc_send_partial_error:
            popad
                xor eax, eax
                ret 4
        }
    }

    //! Sends all request data within a single call
    static bool __declspec(naked) __fastcall SendData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, const void *pData)
    {
        __asm
        {
            pushad
            mov eax, 0x564D5868
            mov ebx, edx
            mov edx, [ecx].m_ChannelNumber
            or edx, 0x5658
            mov esi, [ecx].m_Cookie1
            mov edi, [ecx].m_Cookie2
            // ebp <- class pointer
            xchg ebp, ecx
            mov ecx, 0x0001001E
            out dx, eax
            cmp ecx, 0x00810000
            jnz vmware_rpc_send_data_error

            //ecx <- byte count to send (minus null)
            xchg ecx, ebx
            mov ebx, 0x00010000
            mov esi, [esp + 0x24]	//pData
            mov edx, [ebp].m_ChannelNumber
            or edx, 0x5659
            mov edi, [ebp].m_Cookie2
            mov ebp, [ebp].m_Cookie1
            rep outsb

            cmp ebx, 0x00010000
            jnz vmware_rpc_send_data_error

            popad
            xor eax, eax
            inc eax
            ret 4

            vmware_rpc_send_data_error:
            popad
                xor eax, eax
                ret
        }
    }

    //! Returns reply size of a GuestRPC command
    static unsigned __declspec(naked) __fastcall GetReplySize(VMWareRpcData *pChannel)
    {
        __asm
        {
            push ebx
            push esi
            push edi
            push ebp
            mov eax, 0x564D5868
            mov edx, [ecx].m_ChannelNumber
            or edx, 0x5658
            mov esi, [ecx].m_Cookie1
            mov edi, [ecx].m_Cookie2
            xchg ebp, ecx
            mov ecx, 0x0003001E
            out dx, eax
            mov[ebp].m_ReplyId, edx
            cmp ecx, 0x00830000
            jz return_from_get_reply_size
            or ebx, -1
            return_from_get_reply_size:
            xchg eax, ebx
                pop ebp
                pop edi
                pop esi
                pop ebx
                ret
        }
    }

    //! Reads a part of GuestRPC reply data. Works similar to recv() function in sockets.
    static bool __declspec(naked) __fastcall RecvData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, void *pData)
    {
        __asm
        {
            pushad
            mov eax, 0x564D5868
            mov ebx, 0x00010000
            xchg ecx, edx
            mov edi, [esp + 0x24]	//pData
            mov esi, [edx].m_Cookie1
            mov ebp, [edx].m_Cookie2
            mov edx, [edx].m_ChannelNumber
            or edx, 0x5659
            rep insb

            cmp ebx, 0x00010000
            jnz vmware_rpc_recv_data_error

            popad
            xor eax, eax
            inc eax
            ret 4

            vmware_rpc_recv_data_error:
            popad
                xor eax, eax
                ret 4
        }
    }

    //! Called when all reply data has been received
    static bool __declspec(naked) __fastcall EndReceive(VMWareRpcData *pChannel)
    {
        __asm
        {
            pushad
            mov eax, 0x564D5868
            mov ebx, [ecx].m_ReplyId
            mov edx, [ecx].m_ChannelNumber
            or edx, 0x5658
            mov esi, [ecx].m_Cookie1
            mov edi, [ecx].m_Cookie2
            mov ecx, 0x0005001E
            out dx, eax
            cmp ecx, 0x00010000
            popad
            setz al
            and eax, 1
            ret
        }
    }
#else

    static inline int OpenChannel(VMWareRpcData *pChannel)
    {
        return VMWareOpenChannel(pChannel);
    }

    static inline bool CloseChannel(VMWareRpcData *pChannel)
    {
        return VMWareCloseChannel(pChannel);
    }

    static inline bool SendRequestLength(VMWareRpcData *pChannel, size_t sizeNotCountingNULL)
    {
        return VMWareSendRequestLength(pChannel, sizeNotCountingNULL);
    }

    static inline bool  SendPartialData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, const void *pData)
    {
        return VMWareSendPartialData(pChannel, sizeNotCountingNULL, pData);
    }

    static inline bool  SendData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, const void *pData)
    {
        return VMWareSendData(pChannel, sizeNotCountingNULL, pData);
    }

    static inline unsigned  GetReplySize(VMWareRpcData *pChannel)
    {
        return VMWareGetReplySize(pChannel);
    }

    static inline bool  RecvData(VMWareRpcData *pChannel, size_t sizeNotCountingNULL, void *pData)
    {
        return VMWareRecvData(pChannel, sizeNotCountingNULL, pData);
    }

    static inline bool  EndReceive(VMWareRpcData *pChannel)
    {
        return VMWareEndReceive(pChannel);
    }
#endif
}

//! Specifies a simple object-oriented wrapper for GuestRPC commands
/*! This class provides a convenient way of executing GuestRPC commands. It automatically
    manages the internal state variables of a GuestRPC channel, opening it from a constructor
    and closing from a destructor. All channel interaction is performed using VMWareRPCChannel::ExecuteCommand() method.
    A channel is reusable, i.e. multiple commands can be executed sequentially without reopening a channel.
    \remarks The "one call per request" model is not very convenient to use, so this class is not used directly.
    Instead, the <b>AdvancedRPCChannel</b> was used in KDVMWARE version 1.0. However, as the GuestRPC call processing time
    is almost fixed for a single backdoor operation, and is mostly not dependant on data size, the BufferedRPCChannel
    class reducing the number of backdoor requests should be used now to improve performance.
*/
class VMWareRPCChannel : protected VMWareRpcData
{
private:
    bool m_bVBoxDetected;

protected:
    bool Send(const char *pData, unsigned Size = 0)
    {
        if (!Size)
            Size = (unsigned int)strlen(pData);
        m_CachedReplySize = -1;
        return VMWareRPC::SendData(this, Size, pData);
    }

    bool Receive(void *pData, unsigned Size)
    {
        if (m_CachedReplySize == -1)
            return false;
        if (Size > m_CachedReplySize)
            Size = m_CachedReplySize;
        if (!Size)
            return true;
        if (!VMWareRPC::RecvData(this, Size, pData))
        {
            m_CachedReplySize = m_ChannelNumber = -1;
            return false;
        }
        m_CachedReplySize -= Size;
        return true;
    }

    bool EndReceive()
    {
        if (!VMWareRPC::EndReceive(this))
            return false;
        m_CachedReplySize = -1;
        return true;
    }

    unsigned GetReplySize()
    {
        if (m_CachedReplySize != -1)
            return m_CachedReplySize;
        m_CachedReplySize = VMWareRPC::GetReplySize(this);
        return m_CachedReplySize;
    }

public:

    VMWareRPCChannel()
        : m_bVBoxDetected(false)
    {
        switch (VMWareRPC::OpenChannel(this))
        {
        case 1:
            break;
        case 2:
            m_bVBoxDetected = true;
            // no break here;
        default:
            m_ChannelNumber = -1;

        }
        m_CachedReplySize = -1;
    }

    ~VMWareRPCChannel()
    {
        VMWareRPC::CloseChannel(this);
    }

    //! Executes a single GuestRPC request
    unsigned ExecuteCommand(const char *pszRequest, char *pszBuffer, unsigned MaxBufferSize)
    {
        ASSERT(pszRequest && pszBuffer && (MaxBufferSize > 1));
        if (!Send(pszRequest))
            return -1;
        unsigned size = GetReplySize();
        if ((size == -1) || (size >= MaxBufferSize))
            return -1;
        if (!Receive(pszBuffer, size))
            return -1;
        pszBuffer[size] = 0;
        if (!EndReceive())
            return -1;
        return size;
    }

    bool Valid()
    {
        return m_ChannelNumber != -1;
    }

    bool IsVBoxDetected()
    {
        return m_bVBoxDetected;
    }
};

/*class AdvancedRPCChannel : private VMWareRPCChannel
{
public:
    bool PrepareSend(unsigned SendSize)
    {
        return VMWareRPC::SendRequestLength(this, SendSize);
    }

    bool SendPartial(const void *pData, unsigned Size)
    {
        return VMWareRPC::SendPartialData(this, Size, pData);
    }

    unsigned GetReplySize()
    {
        return __super::GetReplySize();
    }

    bool Receive(void *pData, unsigned Size)
    {
        return __super::Receive(pData, Size);
    }

    bool Valid()
    {
        return __super::Valid();
    }

    bool EndReceive()
    {
        return __super::EndReceive();
    }
};*/

//! Provides a more convenient way of using GuestRPC
template <char *t_pBuffer, unsigned t_BufferSize> class BufferedRPCChannel : private VMWareRPCChannel
{
private:
    unsigned m_BufferPos;
    unsigned m_TotalTransferSize;

    enum
    {
        Idle,
        Sending,
        Sent,
        Receiving,
        Received,
    } m_Phase;

private:
    bool SendBufferedData()
    {
#ifdef VMWRPC_PARAMETER_CHECKING
        if (m_Phase != Sending)
            return false;
        if (m_BufferPos != m_TotalTransferSize)
            return false;
#endif
        if (!__super::Send(t_pBuffer, m_TotalTransferSize))
            return false;
        m_Phase = Sent;
        return true;
    }

public:
    BufferedRPCChannel()
        : m_BufferPos(0)
        , m_TotalTransferSize(0)
        , m_Phase(Idle)
    {
    }

    ~BufferedRPCChannel()
    {
    }

    bool PrepareSend(unsigned SendSize)
    {
        if ((m_Phase != Idle) || (SendSize > t_BufferSize))
            return false;
        m_TotalTransferSize = SendSize;
        m_Phase = Sending;
        m_BufferPos = 0;
        return true;
    }

    bool SendPartial(const void *pData, unsigned Size)
    {
        if (!Size)
            return true;
#ifdef VMWRPC_PARAMETER_CHECKING
        if (m_Phase != Sending)
            return false;
        if (Size > (m_TotalTransferSize - m_BufferPos))
            return false;
#endif
        memcpy(t_pBuffer + m_BufferPos, pData, Size);
        m_BufferPos += Size;
        return true;
    }

    unsigned GetReplySize()
    {
        if (m_Phase == Receiving)
            return m_TotalTransferSize;
        if (!SendBufferedData())
            return -1;
        if (m_Phase != Sent)
            return -1;
        m_TotalTransferSize = __super::GetReplySize();
        if (m_TotalTransferSize && (m_TotalTransferSize != -1) && (m_TotalTransferSize <= t_BufferSize))
        {
            if (!__super::Receive(t_pBuffer, t_BufferSize))
                return -1;
            m_Phase = Receiving;
            m_BufferPos = 0;
            return m_TotalTransferSize;
        }
        else
            return -1;
    }

    bool Receive(void *pData, unsigned Size)
    {
        if (!Size)
            return true;
#ifdef VMWRPC_PARAMETER_CHECKING
        if (m_Phase != Receiving)
            return false;
        if (Size > (m_TotalTransferSize - m_BufferPos))
            return false;
#endif
        memcpy(pData, t_pBuffer + m_BufferPos, Size);
        m_BufferPos += Size;
        if (m_BufferPos == m_TotalTransferSize)
            m_Phase = Received;
        return true;
    }

    bool Valid()
    {
        return __super::Valid();
    }

    bool EndReceive()
    {
        if (m_Phase != Received)
            return false;
        if (!__super::EndReceive())
            return false;
        m_Phase = Idle;
        return true;
    }

    bool NeedRestartInVBoxMode()
    {
        return __super::IsVBoxDetected();
    }

};