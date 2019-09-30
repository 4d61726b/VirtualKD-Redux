/*! \file
\author Ivan Shcherbakov (Bazis)
$$
*/

#pragma once

namespace VBoxRPC
{
#ifndef _AMD64_
	static void __declspec(naked) __fastcall ExchangeData(ULONG pPhysicalAddr, int)	//First dword specifies in size, second - max out size. On exit
	{
		__asm
		{
			xchg eax, ecx
			mov edx, 0x5659
			out dx, eax
			ret
		}
	}
#else
	extern "C" void VBoxExchangeData(ULONG pPhysicalAddr, int);

	static inline void ExchangeData(ULONG pPhysicalAddr, int a)
	{
		VBoxExchangeData(pPhysicalAddr, a);
	}

#endif
}

//! Provides a fast way of using VBOX exchange interface
template <char *t_pBuffer, unsigned t_BufferSize> class BufferedVBoxChannel
{
private:
	unsigned m_BufferPos;
	unsigned m_TotalTransferSize;
	PHYSICAL_ADDRESS m_BufPhysical;

	enum {SendHdrSize = 2 * sizeof(unsigned), RecvHdrSize = sizeof(unsigned)};

	enum
	{
		Idle,
		Sending,
		Receiving,
		Received,
	} m_Phase;

private:
	bool PerformDataExchange()
	{
#ifdef VMWRPC_PARAMETER_CHECKING
		if (m_Phase != Sending)
			return false;
		if (m_BufferPos != m_TotalTransferSize)
			return false;
#endif
		VBoxRPC::ExchangeData(m_BufPhysical.LowPart, 0);
		m_TotalTransferSize = ((unsigned *)t_pBuffer)[0] + RecvHdrSize;
		m_BufferPos = RecvHdrSize;
		m_Phase = Receiving;
		return true;
	}

public:
	BufferedVBoxChannel()
		: m_BufferPos(0)
		, m_TotalTransferSize(0)
		, m_Phase(Idle)
	{
		m_BufPhysical = MmGetPhysicalAddress(t_pBuffer);
	}

	~BufferedVBoxChannel()
	{
	}

	bool PrepareSend(unsigned SendSize)
	{
#ifdef VMWRPC_PARAMETER_CHECKING
		if ((m_Phase != Idle) || (SendSize > (t_BufferSize - SendHdrSize)))
			return false;
#endif
		m_TotalTransferSize = SendSize + SendHdrSize;
		m_Phase = Sending;
		((unsigned *)t_pBuffer)[0] = SendSize;
		((unsigned *)t_pBuffer)[1] = t_BufferSize;
		m_BufferPos = SendHdrSize;
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
			return m_TotalTransferSize - RecvHdrSize;
		if (!PerformDataExchange())
			return -1;
		if (m_Phase != Receiving)
			return -1;
		return m_TotalTransferSize - RecvHdrSize;
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
		return true;
	}

	bool EndReceive()
	{
		if (m_Phase != Received)
			return false;
		m_Phase = Idle;
		return true;
	}

	bool NeedRestartInVBoxMode()
	{
		return false;
	}
};