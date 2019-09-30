/*! \file reporter.h
    \author Ivan Shcherbakov (Bazis)
    $Id: reporter.h,v 1.6 2008-12-31 13:11:45 Bazis Exp $
    \brief Contains declarations for status reporting class
*/

#pragma once
#include "status.h"

#define KDCLIENT_REPORT_PERFORMANCE_INFORMATION
#define KDCLIENT_ENABLE_TRACE_ASSIST

//! Reports various statistics to VMMON.EXE application using a shared file mapping.
class StatusReporter
{
private:
	HANDLE m_hMapping;
	HANDLE m_hLogPipe;
	KdClientStatus *m_pStatus;
	
	KdClientStatus m_UnusedStatus;

private:
	void DoLogLine(const TCHAR *pszText);

public:
	//! Returns a pointer to the KdClientStatus structure.
	/*!
		\remarks This function never returns NULL. If the connection with VMMON.EXE was not established,
		the function returns a pointer to an unused structure stored inside the StatusReporter.
	*/
	KdClientStatus *GetStatusPointer()
	{
		return m_pStatus;
	}

public:
	StatusReporter();
	~StatusReporter();

	void LogLineIfEnabled(const TCHAR *pszText)
	{
		if (m_hLogPipe != INVALID_HANDLE_VALUE)
			DoLogLine(pszText);
	}
};

extern StatusReporter *g_pReporter;
