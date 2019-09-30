/*! \file PacketLog.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: PacketLog.cpp,v 1.2 2009-12-31 13:06:41 Bazis Exp $
    \brief Implements packet logging functionality
*/

#include "stdafx.h"
#include "PacketLog.h"
#include <BazisLib/bzscore/autofile.h>

static const char szHTMLPreambleFormat[] = "<html><head><title>KDVMWare Packet Log - %S</title>\r\n\
<style>table {font-family: Courier New} tr.recv {color: red; } tr.send {color: green; }</style>\r\n\
</head>\r\n<body><center><h1>KDVMWare Packet Log - %S</h1></center>\r\n";

static const char szLegend[] = "Legend: <b><font color = red>Debugger-to-kernel packets are in red</font>; <font color = green>Kernel-to-debugger packets are in green</font></b><br/>\r\n";
static const char szTableHeader[] = "<table border><tr><td>#</td><td>Time</td><td>Type</td><td>Data</td></tr>";

static const char szTerminationSimulationNotification[] = "</table><br/><center><font color = \"blue\"><b>----- Simulating session termination -----</b></font></center><br/>";
static const char szTerminationSimulationEndFmt[] = "</table><br/><center><font color = \"blue\"><b>----- Session termination simulated: %s -----</b></font></center><br/>";

static const char szHTMLFooter[] = "\r\n</table></body></html>";

PacketLogger::PacketLogger( const TCHAR *pszKdPipeName )
	: m_LogFilePath(_T(""))
	, m_SendPacketNumber(0)
	, m_RecvPacketNumber(0)
	, m_bTableHeaderPrinted(false)
{
	if (!pszKdPipeName)
		return;
	const TCHAR *p = _tcsrchr(pszKdPipeName, '\\');
	if (!p)
		return;
	
	TCHAR tsz[MAX_PATH];
#ifdef _USRDLL
	extern HMODULE g_hThisDll;
	GetModuleFileName(g_hThisDll, tsz, sizeof(tsz)/sizeof(tsz[0]));
#else
	GetModuleFileName(GetModuleHandle(NULL), tsz, sizeof(tsz)/sizeof(tsz[0]));
#endif

	TCHAR *p2 = _tcsrchr(tsz, '\\');
	if (p2)
		p2[0] = 0;

	m_LogFilePath = tsz;
	m_LogFilePath = BazisLib::Path::Combine(m_LogFilePath, p + 1);

	if (!memcmp(p + 1, L"kd_", 3 * sizeof(TCHAR)))
		p += 3;

#ifdef UNICODE
	m_SessionName = (p + 1);
#else
#error "Unsupported"
#endif
	m_LogFilePath += _T(".html");
}

#pragma region Helpers for displaying various constants from windbgkd.h
static const char *GetPacketTypeName(int PacketType)
{
	switch(PacketType)
	{
	case 1:
		return " KD_STATE_CHANGE32";
	case 2:
		return " KD_STATE_MANIPULATE";
	case 3:
		return " KD_DEBUG_IO";
	case 4:
		return " KD_ACKNOWLEDGE";
	case 5:
		return " KD_RESEND";
	case 6:
		return " KD_RESET";
	case 7:
		return " KD_STATE_CHANGE64";
	case 8:
		return " KD_POLL_BREAKIN";
	case 9:
		return " KD_TRACE_IO";
	case 10:
		return " KD_CONTROL_REQUEST";
	case 11:
		return " KD_FILE_IO";
	default:
		return "";
	}
}

static const char *GetApiTypeName(int ApiType)
{
	switch (ApiType)
	{
	case 0x00003030:
		return "DbgKdExceptionStateChange";
	case 0x00003031:
		return "DbgKdLoadSymbolsStateChange";
	case 0x00003032:
		return "DbgKdCommandStringStateChange";
	case 0x00003033:
		return "DbgKdMaximumStateChange";
	case 0x00003130:
		return "DbgKdReadVirtualMemoryApi";
	case 0x00003131:
		return "DbgKdWriteVirtualMemoryApi";
	case 0x00003132:
		return "DbgKdGetContextApi";
	case 0x00003133:
		return "DbgKdSetContextApi";
	case 0x00003134:
		return "DbgKdWriteBreakPointApi";
	case 0x00003135:
		return "DbgKdRestoreBreakPointApi";
	case 0x00003136:
		return "DbgKdContinueApi";
	case 0x00003137:
		return "DbgKdReadControlSpaceApi";
	case 0x00003138:
		return "DbgKdWriteControlSpaceApi";
	case 0x00003139:
		return "DbgKdReadIoSpaceApi";
	case 0x0000313A:
		return "DbgKdWriteIoSpaceApi";
	case 0x0000313B:
		return "DbgKdRebootApi";
	case 0x0000313C:
		return "DbgKdContinueApi2";
	case 0x0000313D:
		return "DbgKdReadPhysicalMemoryApi";
	case 0x0000313E:
		return "DbgKdWritePhysicalMemoryApi";
	case 0x0000313F:
		return "DbgKdQuerySpecialCallsApi";
	case 0x00003140:
		return "DbgKdSetSpecialCallApi";
	case 0x00003141:
		return "DbgKdClearSpecialCallsApi";
	case 0x00003142:
		return "DbgKdSetInternalBreakPointApi";
	case 0x00003143:
		return "DbgKdGetInternalBreakPointApi";
	case 0x00003144:
		return "DbgKdReadIoSpaceExtendedApi";
	case 0x00003145:
		return "DbgKdWriteIoSpaceExtendedApi";
	case 0x00003146:
		return "DbgKdGetVersionApi";
	case 0x00003147:
		return "DbgKdWriteBreakPointExApi";
	case 0x00003148:
		return "DbgKdRestoreBreakPointExApi";
	case 0x00003149:
		return "DbgKdCauseBugCheckApi";
	case 0x00003150:
		return "DbgKdSwitchProcessor";
	case 0x00003151:
		return "DbgKdPageInApi";
	case 0x00003152:
		return "DbgKdReadMachineSpecificRegister";
	case 0x00003153:
		return "DbgKdWriteMachineSpecificRegister";
	case 0x00003154:
		return "OldVlm1";
	case 0x00003155:
		return "OldVlm2";
	case 0x00003156:
		return "DbgKdSearchMemoryApi";
	case 0x00003157:
		return "DbgKdGetBusDataApi";
	case 0x00003158:
		return "DbgKdSetBusDataApi";
	case 0x00003159:
		return "DbgKdCheckLowMemoryApi";
	case 0x0000315A:
		return "DbgKdClearAllInternalBreakpointsApi";
	case 0x0000315B:
		return "DbgKdFillMemoryApi";
	case 0x0000315C:
		return "DbgKdQueryMemoryApi";
	case 0x0000315D:
		return "DbgKdSwitchPartition";
	case 0x0000315E:
		return "DbgKdMaximumManipulate";
	default:
		return NULL;
	}
}
#pragma endregion

static void DumpHexA(const void *pBuffer, size_t length, BazisLib::ConstManagedPointer<BazisLib::AIFile> &pFile, size_t BytesPerLine = 16, const char *pLineBreak = "<br/>\r\n")
{
	PBYTE pBuf = (PBYTE)pBuffer;
	std::string strBuf;
	strBuf.reserve(1024);

	char sz[32];
	for (size_t i = 0; i < length;)
	{
		size_t todo = length - i;
		if (todo > BytesPerLine)
			todo = BytesPerLine;

		strBuf.erase();
		
		for (size_t j = 0; j < BytesPerLine; j++)
		{
			if (j < todo)
				_snprintf(sz, sizeof(sz), "%02X&nbsp;", pBuf[i + j]);
			else
				strncpy(sz, "&nbsp;&nbsp;&nbsp;", sizeof(sz));
			strBuf += sz;
		}

		strBuf += "&nbsp;&nbsp;&nbsp;&nbsp;";

		sz[1] = 0;
		for (size_t j = 0; j < todo; j++)
		{
			BYTE byte = pBuf[i + j];
			if ((byte >= 0x20) && (byte <= 0x7F))
				sz[0] = byte;
			else
				sz[0] = '.';
			strBuf += sz;
		}

		i += todo;
		pFile->Write(strBuf.c_str(), strBuf.length());
		
		if (i < length)
			pFile->Write(pLineBreak, strlen(pLineBreak));

	}
}

void PacketLogger::OnSendReceivePacket( bool bLoggingEnabled, bool bSendPacket, ULONG PacketType, PKD_BUFFER FirstBuffer, PKD_BUFFER SecondBuffer, PKD_CONTEXT KdContext )
{
	if (!bLoggingEnabled)
	{
		if (m_pFile && m_pFile->Valid())
			m_pFile->Write(szHTMLFooter, sizeof(szHTMLFooter) - 1);
		if (m_pFile)
			m_pFile = NULL;
		return;
	}
	if (!m_pFile)
	{
		m_pFile = new BazisLib::ACFile(m_LogFilePath, BazisLib::FileModes::CreateOrTruncateRW);
		if (!m_pFile || !m_pFile->Valid())
			return;
		m_StartTime = BazisLib::DateTime::Now();
		m_RecvPacketNumber = m_SendPacketNumber = 1;
		char szTmp[sizeof(szHTMLPreambleFormat) + MAX_PATH * 2];
		_snprintf(szTmp, sizeof(szTmp), szHTMLPreambleFormat, m_SessionName.c_str(), m_SessionName.c_str());
		m_pFile->Write(szTmp, strlen(szTmp));
		m_pFile->Write(szLegend, sizeof(szLegend) - 1);
		m_bTableHeaderPrinted = false;
	}
	if (!m_pFile || !m_pFile->Valid())
		return;

	if (!m_bTableHeaderPrinted)
	{
		m_pFile->Write(szTableHeader, sizeof(szTableHeader) - 1);
		m_bTableHeaderPrinted = true;
	}

	char sz[512];
	ULONGLONG t = (BazisLib::DateTime::Now() - m_StartTime).GetTotalMilliseconds();

	unsigned msec = (unsigned)(t % 1000);
	unsigned tsec = (unsigned)(t / 1000);

	unsigned sec = tsec % 60;
	unsigned min = tsec / 60;
	unsigned hr = min / 60;
	min %= 60;

	unsigned &num = bSendPacket ? m_SendPacketNumber : m_RecvPacketNumber;
	_snprintf(sz, sizeof(sz), "<tr class=\"%s\"><td>%d</td><td>+%d:%02d:%02d.%04d</td><td>%d%s</td><td>\r\n", bSendPacket ? "send" : "recv", num++, hr, min, sec, msec, PacketType, GetPacketTypeName(PacketType));
	m_pFile->Write(sz, strlen(sz));

	if (FirstBuffer && FirstBuffer->Length)
	{
		_snprintf(sz, sizeof(sz), "Buf1: Size = %d (0x%X)", FirstBuffer->Length, FirstBuffer->Length);
		m_pFile->Write(sz, strlen(sz));

		const char *pApi = GetApiTypeName(*((unsigned *)FirstBuffer->pData));
		if (pApi)
			_snprintf(sz, sizeof(sz), "; Api# = %s<br/>\r\n", pApi);
		else
			_snprintf(sz, sizeof(sz), "<br/>\r\n");
		m_pFile->Write(sz, strlen(sz));

		DumpHexA(FirstBuffer->pData, FirstBuffer->Length, m_pFile);
	}

	if (SecondBuffer && SecondBuffer->Length)
	{
		_snprintf(sz, sizeof(sz), "<br/><br/>Buf2: Size = %d (0x%X)<br/>", SecondBuffer->Length, SecondBuffer->Length);
		m_pFile->Write(sz, strlen(sz));

		DumpHexA(SecondBuffer->pData, SecondBuffer->Length, m_pFile);
	}
	_snprintf(sz, sizeof(sz), "</td></tr>\r\n");
	m_pFile->Write(sz, strlen(sz));
}

PacketLogger::~PacketLogger()
{
	if (m_pFile && m_pFile->Valid())
		m_pFile->Write(szHTMLFooter, sizeof(szHTMLFooter) - 1);
}

void PacketLogger::OnWindowsTerminationSimulated()
{
	if (!m_pFile)
		return;
	m_pFile->Write(szTerminationSimulationNotification, sizeof(szTerminationSimulationNotification) - 1);
	m_bTableHeaderPrinted = false;
}

void PacketLogger::OnWindowsTerminationSimDone( const char *pDebugMsg )
{
	if (!m_pFile)
		return;
	char szMsg[1024];
	_snprintf(szMsg, sizeof(szMsg), szTerminationSimulationEndFmt, pDebugMsg ? pDebugMsg : "(no additional info)");
	m_pFile->Write(szMsg, strlen(szMsg));
	m_bTableHeaderPrinted = false;
}