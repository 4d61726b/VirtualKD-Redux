/*! \file pipesrv.h
    \author Ivan Shcherbakov (Bazis)
    $Id: pipesrv.h,v 1.10 2009-09-15 11:06:50 Administrator Exp $
    \brief Contains a simple inline named pipe server
*/

#pragma once
#include "rpcdispatch/rpcdisp.h"
#include "permdesc.h"
#include <BazisLib/bzscore/string.h>

//! Implements a simple Named Pipe Server with automatic reconnection support
template <bool _MessageOriented, bool _AutoReconnect, bool _FailTransferOnListening = false> class PipeServer
{
private:
    HANDLE m_hPipe;
    HANDLE m_hNextInstance;
    BazisLib::String m_PipeName;
    bool m_bReconnectPending;
    bool m_bClientConnected;
    unsigned m_Timeout;
    unsigned m_LastError;

    OVERLAPPED m_Overlapped;
    HANDLE m_hEvt;

    bool m_bShutDown;

private:
    HANDLE CreatePipeInstance()
    {
        PermissiveSecurityDescriptor desc;
        return CreateNamedPipe(m_PipeName.c_str(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            _MessageOriented ? (PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE) : (PIPE_TYPE_BYTE | PIPE_READMODE_BYTE),
            2,
            131072,
            131072,
            m_Timeout,
            desc);
    }

private:
    template<bool out> size_t TransferWithTimeout(void *pData, size_t size)
    {
        DWORD dwOk = 0;
        if (out)
            WriteFile(m_hPipe, pData, (DWORD)size, &dwOk, &m_Overlapped);
        else
            ReadFile(m_hPipe, pData, (DWORD)size, &dwOk, &m_Overlapped);
        m_LastError = GetLastError();
        if (dwOk)
            return dwOk;
        switch (m_LastError)
        {
        case ERROR_PIPE_LISTENING:
            m_bClientConnected = false;
            return 0;
        case ERROR_IO_PENDING:
            m_bClientConnected = true;
            break;
        default:
            return 0;
        }
        if (WaitForSingleObject(m_hEvt, m_Timeout) == WAIT_OBJECT_0)
        {
            GetOverlappedResult(m_hPipe, &m_Overlapped, &dwOk, TRUE);
            return dwOk;
        }
        CancelIo(m_hPipe);
        GetOverlappedResult(m_hPipe, &m_Overlapped, &dwOk, TRUE);
        return dwOk;
    }

public:

    unsigned GetLastOperationError()
    {
        return m_LastError;
    }

    bool ReconnectPipe()
    {
        if (m_hPipe != INVALID_HANDLE_VALUE)
            CloseHandle(m_hPipe);
        if (m_hNextInstance == INVALID_HANDLE_VALUE)
            m_hNextInstance = CreatePipeInstance();

        m_hPipe = m_hNextInstance;
        m_hNextInstance = CreatePipeInstance();
        m_bClientConnected = false;

        m_bReconnectPending = false;
        if ((m_hPipe != INVALID_HANDLE_VALUE) && (m_hNextInstance != INVALID_HANDLE_VALUE))
            return true;
        else
            return false;
    }

    ~PipeServer()
    {
        if (m_hPipe != INVALID_HANDLE_VALUE)
            CloseHandle(m_hPipe);
        if (m_hNextInstance != INVALID_HANDLE_VALUE)
            CloseHandle(m_hNextInstance);
        if (m_hEvt != INVALID_HANDLE_VALUE)
            CloseHandle(m_hEvt);
    }

    PipeServer(const TCHAR *pszPipeName, unsigned Timeout = 1000)
        : m_PipeName(pszPipeName)
        , m_hPipe(INVALID_HANDLE_VALUE)
        , m_hNextInstance(INVALID_HANDLE_VALUE)
        , m_bReconnectPending(false)
        , m_Timeout(Timeout)
        , m_bClientConnected(false)
        , m_LastError(0)
        , m_bShutDown(false)
    {
        m_hEvt = CreateEvent(NULL, TRUE, FALSE, NULL);
        memset(&m_Overlapped, 0, sizeof(m_Overlapped));
        m_Overlapped.hEvent = m_hEvt;
        ReconnectPipe();
    }

    size_t Send(const void *pData, size_t Size)
    {
        if (m_bShutDown)
            return 0;
        SIZE_T dwOk;
        while (!(dwOk = TransferWithTimeout<true>((void *)pData, Size)))
        {
            switch (m_LastError)
            {
            case ERROR_PIPE_LISTENING:
                if (_FailTransferOnListening)
                    return 0;
                if (!ConnectNamedPipe(m_hPipe, NULL))
                    return 0;
                continue;
                break;
            case ERROR_BROKEN_PIPE:
                m_bReconnectPending = true;
                break;
            }
            return 0;
        }
        return dwOk;
    }

    size_t Receive(void *pData, size_t Size, bool PartialRecvAllowed = true)
    {
        if (m_bShutDown)
            return 0;
        size_t offset = 0;
        for (;;)
        {
            SIZE_T dwOk = 0;
            dwOk = TransferWithTimeout<false>(((char *)pData) + offset, Size - offset);
            if (dwOk)
            {
                if (PartialRecvAllowed)
                    return dwOk;
                else
                {
                    offset += dwOk;
                    if (offset == Size)
                        return Size;
                    else
                        continue;
                }
            }
            else
            {
                switch (m_LastError)
                {
                case ERROR_PIPE_LISTENING:
                    if (_FailTransferOnListening)
                        return 0;
                    if (!ConnectNamedPipe(m_hPipe, NULL))
                        return 0;
                    continue;
                    break;
                case ERROR_BROKEN_PIPE:
                    m_bReconnectPending = true;
                    if (_AutoReconnect)
                    {
                        ReconnectPipe();
                        continue;
                    }
                    break;
                }
                return 0;
            }
        }
    }

    bool HasDataInBuffer()
    {
        DWORD dataAvailable = 0;
        for (;;)
        {
            if (!PeekNamedPipe(m_hPipe, NULL, 0, NULL, &dataAvailable, NULL))
            {
                m_LastError = GetLastError();
                switch (m_LastError)
                {
                case ERROR_BROKEN_PIPE:
                    m_bClientConnected = false;
                    m_bReconnectPending = true;
                    if (_AutoReconnect)
                    {
                        ReconnectPipe();
                        continue;
                    }
                    break;
                case ERROR_PIPE_LISTENING:
                    m_bClientConnected = false;
                    break;
                }
                return false;
            }
            m_bClientConnected = true;
            return (dataAvailable != 0);
        }
    }

    bool IsClientConnected()
    {
        return m_bClientConnected;
    }

    void DiscardBufferedData()
    {
        DWORD dataAvailable = 0;
        for (;;)
        {
            if (!PeekNamedPipe(m_hPipe, NULL, 0, NULL, &dataAvailable, NULL))
            {
                m_LastError = GetLastError();
                if (m_LastError == ERROR_BROKEN_PIPE)
                {
                    m_bReconnectPending = true;
                    if (_AutoReconnect)
                    {
                        ReconnectPipe();
                        continue;
                    }
                }
                return;
            }
            char *pTemp = new char[dataAvailable];
            Receive(pTemp, dataAvailable, false);
            delete pTemp;
            return;
        }
    }

    bool ReconnectPending()
    {
        return m_bReconnectPending;
    }

    void Shutdown()
    {
        m_bShutDown = true;
        CloseHandle(m_hPipe);
        CloseHandle(m_hNextInstance);
        m_hPipe = m_hNextInstance = INVALID_HANDLE_VALUE;
    }

    bool IsShutDown()
    {
        return m_bShutDown;
    }
};

