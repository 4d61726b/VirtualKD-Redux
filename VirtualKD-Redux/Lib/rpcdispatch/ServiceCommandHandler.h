#pragma once
#include "ServiceCommandProtocol.h"
#include "pipesrv.h"
#include <BazisLib/bzscore/thread.h>

HRESULT StartRevertingCurrentVMToLastSnapshot_VBox();
HRESULT StartRevertingCurrentVMToLastSnapshot_VMWare();

class ServiceCommandHandler
{
private:
    PipeServer<true, true, false> m_PipeServer;
    BazisLib::MemberThread m_Thread;
    BazisLib::Win32::Event m_ThreadDoneEvent;

    long m_bInstantBreakRequested;
    VMType m_VMType;

public:
    unsigned ThreadBody()
    {
        while (!m_PipeServer.IsShutDown())
        {
            ServiceProtocolPacket packet;
            if (m_PipeServer.Receive(&packet, sizeof(packet), false) != sizeof(packet))
                continue;

            ServiceProtocolReply reply = { 0, };
            switch (packet.Command)
            {
            case kInstantBreak:
                m_bInstantBreakRequested = true;
                reply.Status = S_OK;
                break;
            case kRevertToLastSnapshot:
                switch (m_VMType)
                {
                case kVMWare32:
                case kVMWare64:
                    reply.Status = StartRevertingCurrentVMToLastSnapshot_VMWare();
                    break;
                case kVBox32:
                case kVBox64:
                    reply.Status = StartRevertingCurrentVMToLastSnapshot_VBox();
                    break;
                default:
                    reply.Status = E_NOTIMPL;
                    break;
                }
                break;
            }

            m_PipeServer.Send(&reply, sizeof(reply));
        }
        m_ThreadDoneEvent.Set();
        //Returning/calling ExitThread() when DLL_PROCESS_DETACH is active would cause deadlock in WaitForSingleObject()
        m_Thread.Terminate();
        return 0;
    }

    /*!
        \param lpPipeName Specifies the base VirtualKD pipe name. The service pipe name will be formed by appending the suffix.
    */
    ServiceCommandHandler(LPCTSTR lpPipeName, VMType vmType)
        : m_PipeServer((BazisLib::String(lpPipeName) + VIRTUALKD_SERVICE_PROTOCOL_PIPE_SUFFIX).c_str())
        , m_Thread(this, &ServiceCommandHandler::ThreadBody)
        , m_bInstantBreakRequested(0)
        , m_VMType(vmType)
    {
        m_Thread.Start();
    }

    ~ServiceCommandHandler()
    {
        m_PipeServer.Shutdown();
        //If this destructor is called from DLL_PROCESS_DETACH, waiting for the thread won't work. 

        HANDLE waitObjects[2] = { m_ThreadDoneEvent.GetHandle(), m_Thread.GetHandle() };
        WaitForMultipleObjects(_countof(waitObjects), waitObjects, FALSE, INFINITE);

        m_Thread.Terminate();
    }

    bool CheckAndClearInstantBreakRequestedFlag()
    {
        return InterlockedExchange(&m_bInstantBreakRequested, 0) != 0;
    }

};