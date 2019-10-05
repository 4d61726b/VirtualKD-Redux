#pragma once

class SessionTerminationNotifier
{
private:
    BazisLib::String m_PipeName;

public:
    SessionTerminationNotifier(const TCHAR *pOriginalPipeName)
    {
        m_PipeName = pOriginalPipeName;
        m_PipeName += _T("_notify0");
    }

    //! Notifies VisualDDK, that the session is being forcibly terminated, and waits for acknowledgment
    bool SignalSessionTerminationAndWaitForAck()
    {
        unsigned msg = 0;
        DWORD dwDone = 0;
        return (CallNamedPipe(m_PipeName.c_str(), &msg, sizeof(msg), &msg, sizeof(msg), &dwDone, NMPWAIT_NOWAIT) != FALSE);
    }
};