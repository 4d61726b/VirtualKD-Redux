/*! \file trassist.h
    \author Ivan Shcherbakov (Bazis)
    $Id: trassist.h,v 1.7 2009-12-31 13:06:41 Bazis Exp $
    \brief Contains inline TraceAssist implementation
*/

#pragma once
#include <BazisLib/bzshlp/Win32/RegistrySerializer.h>
#include <BazisLib/bzscore/Win32/registry.h>
#include <BazisLib/bzscore/file.h>
#include <vkdversion.h>

//! Allows saving DbgPrint() messages directly to files on host machine bypassing WinDBG.
class TraceAssistant
{
private:
    TraceAssistParams m_Params;
    BazisLib::File *m_pLogFile;
    BazisLib::String m_LogFileName;
    BazisLib::DynamicStringA m_Prefix;

public:
    //! Reloads TraceAssist parameters from registry
    void ReloadParams()
    {
        BazisLib::RegistryKey key(HKEY_LOCAL_MACHINE, VKD_REGISTRY_TRACE_ASSIST_PATH, 0, false);
        BazisLib::Win32::RegistrySerializer::Deserialize(key, m_Params);
        m_Prefix = BazisLib::StringToANSIString(m_Params.MessagePrefix);
    }

    TraceAssistant(LPCTSTR ptszFullPipeName)
        : m_LogFileName(_T(""))
        , m_pLogFile(NULL)
    {
        ASSERT(ptszFullPipeName);
        LPCTSTR pT = _tcsrchr(ptszFullPipeName, '\\');
        if (pT)
            m_LogFileName = (pT + 1 + 3);
        else
            m_LogFileName = ptszFullPipeName + 3;
        m_LogFileName += _T(".log");
        ReloadParams();
    }

    ~TraceAssistant()
    {
        delete m_pLogFile;
    }

    //! Logs a block of text
    /*!
        \return If the text was successfully logged and it should not be passed to WinDBG, the function returns <b>true</b>.
                If an error occured, or the settings prevent TraceAssist from logging the line, the function returns
                <b>false</b>, and the text is passed to WinDBG.
    */
    bool TraceLine(const char *pszLine, size_t LineLength)
    {
        if (!pszLine || !LineLength)
            return false;
        if (!m_Params.TraceAssistEnabled || m_Params.LogFileDirectory.empty())
            return false;
        if (!m_Prefix.empty())
            if (memcmp(m_Prefix.c_str(), pszLine, m_Prefix.length()))
                return false;
        if (!m_pLogFile)
        {
            m_pLogFile = new BazisLib::File(BazisLib::Path::Combine(m_Params.LogFileDirectory, m_LogFileName),
                BazisLib::FileModes::CreateOrOpenRW);
            if (!m_pLogFile->Valid())
            {
                delete m_pLogFile;
                m_pLogFile = NULL;
                return false;
            }
            if (m_Params.OverwriteFileOnStart)
                m_pLogFile->Crop();
            else
                m_pLogFile->Seek(0, BazisLib::FileFlags::FileEnd);
        }
        return (m_pLogFile->Write(pszLine, LineLength) == LineLength);
    }
};