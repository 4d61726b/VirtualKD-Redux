/*! \file reporter.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: reporter.cpp,v 1.5 2009-06-06 12:33:45 Administrator Exp $
    \brief Contains implementation for status reporting class
*/

#include "stdafx.h"
#include "reporter.h"
#include <BazisLib/bzscore/assert.h>
#include "permdesc.h"
#include "vkdversion.h"
#include <stdio.h>

StatusReporter::StatusReporter()
    : m_pStatus(NULL)
    , m_hLogPipe(INVALID_HANDLE_VALUE)
{
    TCHAR tszMappingName[MAX_PATH];
    _sntprintf_s(tszMappingName, __countof(tszMappingName), _TRUNCATE, tszMappingNameFormat, GetCurrentProcessId());
    PermissiveSecurityDescriptor desc;
    m_hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, desc, PAGE_READWRITE, 0, sizeof(KdClientStatus), tszMappingName);
    bool exists = (GetLastError() == ERROR_ALREADY_EXISTS);
    m_pStatus = (KdClientStatus *)MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if ((m_hMapping == INVALID_HANDLE_VALUE) || !m_pStatus)
    {
        if (m_hMapping != INVALID_HANDLE_VALUE)
            CloseHandle(m_hMapping);
        m_hMapping = INVALID_HANDLE_VALUE;
        memset(&m_UnusedStatus, 0, sizeof(m_UnusedStatus));
        m_pStatus = &m_UnusedStatus;
        return;
    }
    if (!exists)
        memset(m_pStatus, 0, sizeof(KdClientStatus));

    m_pStatus->HostSideDLLVersion = VIRTUALKD_VER_INT;

    _sntprintf_s(tszMappingName, __countof(tszMappingName), _TRUNCATE, tszLogPipeNameFormat, GetCurrentProcessId());
    m_hLogPipe = CreateFile(tszMappingName, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
}

StatusReporter::~StatusReporter()
{
    if (m_hMapping != INVALID_HANDLE_VALUE)
    {
        UnmapViewOfFile(m_pStatus);
        CloseHandle(m_hMapping);
    }
    if (m_hLogPipe != INVALID_HANDLE_VALUE)
        CloseHandle(m_hLogPipe);
}

void StatusReporter::DoLogLine(const TCHAR *pszText)
{
    DWORD dwOk;
    if (!WriteFile(m_hLogPipe, pszText, (DWORD)_tcslen(pszText) * sizeof(TCHAR), &dwOk, NULL))
    {
        if (m_hLogPipe != INVALID_HANDLE_VALUE)
            CloseHandle(m_hLogPipe);
        TCHAR tszMappingName[MAX_PATH];
        _sntprintf_s(tszMappingName, __countof(tszMappingName), _TRUNCATE, tszLogPipeNameFormat, GetCurrentProcessId());
        m_hLogPipe = CreateFile(tszMappingName, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        WriteFile(m_hLogPipe, pszText, (DWORD)_tcslen(pszText) * sizeof(TCHAR), &dwOk, NULL);
    }
}
