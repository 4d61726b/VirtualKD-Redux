/*! \file tablemgr.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: tablemgr.cpp,v 1.12 2009-09-07 13:43:30 Administrator Exp $
    \brief Implements the RPC dispatcher hooking engine
*/

#include "stdafx.h"
#include "tablemgr.h"
#include <list>
#include <vector>
#include <BazisLib/bzshlp/textfile.h>
#include <BazisLib/bzshlp/textser.h>
#include <BazisLib/bzscore/Win32/registry.h>
#include <BazisLib/bzscore/autofile.h>
#include "rpcdispatch/reporter.h"
#include <BazisLib/bzshlp/win32/RegistrySerializer.h>

using namespace BazisLib;

PatchInfoDatabase::PatchInfoDatabase(const BazisLib::String &directory)
    : m_Directory(directory)
{
    Directory dir(m_Directory);
    for (Directory::enumerator it = dir.FindFirstFile(_T("*.vmpatch")); it.Valid(); it.Next())
    {
        PatchInfoRecord rec;
        memset(&rec, 0, sizeof(rec));
        ManagedPointer<TextUnicodeFileReader> pReader = new TextUnicodeFileReader(new ACFile(it.GetFullPath(), FileModes::OpenReadOnly));
        if (TextFileSerializer::Deserialize(pReader, rec).Successful())
            m_Records.Add(rec);
    }

    AddBuiltInRecords();
}

PatchInfoDatabase::~PatchInfoDatabase()
{

}

void PatchInfoDatabase::AddBuiltInRecords()
{
    static const std::vector<PatchInfoRecord> vecRecords = {
        // vmware-vmx.exe 15.5.0 build-14665864
        // {1568685315,16754096,8153517,14297256,110}
    };

    for (const PatchInfoRecord& rec : vecRecords)
    {
        bool found = false;

        for (size_t i = 0; i < m_Records.Count(); i++)
        {
            if (m_Records[i].FileSize == rec.FileSize &&
                m_Records[i].TimeStamp == rec.TimeStamp &&
                m_Records[i].FirstSectionSize == rec.FirstSectionSize)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            m_Records.Add(rec);
        }
    }
}

bool PatchInfoDatabase::QueryLoadedImage(void *pPEHeader, PatchInfoRecord &rec)
{
    if (!pPEHeader)
        return false;
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((char *)pPEHeader + ((PIMAGE_DOS_HEADER)pPEHeader)->e_lfanew);
    PIMAGE_SECTION_HEADER pSections = (PIMAGE_SECTION_HEADER)&pNtHdr[1];
    rec.TimeStamp = pNtHdr->FileHeader.TimeDateStamp;
    rec.FirstSectionSize = pSections[0].Misc.VirtualSize;
    rec.FileSize = 0;
    TCHAR tsz[MAX_PATH];
    ULONGLONG fsize = 0;
    GetModuleFileName((HMODULE)pPEHeader, tsz, __countof(tsz));
    {
        File f(tsz, FileModes::OpenReadOnly);
        fsize = f.GetSize();
    }
    if (!fsize || (fsize == -1LL))
        return false;
    rec.FileSize = (unsigned)fsize;
    return true;
}

bool PatchInfoDatabase::FindRecord(void *pPEHeader, PatchInfoRecord &record)
{
    PatchInfoRecord curmod;
    if (!QueryLoadedImage(pPEHeader, curmod))
        return false;
    for (size_t i = 0; i < m_Records.Count(); i++)
    {
        if ((m_Records[i].FileSize == curmod.FileSize) &&
            (m_Records[i].TimeStamp == curmod.TimeStamp) &&
            (m_Records[i].FirstSectionSize == curmod.FirstSectionSize))
        {
            record = m_Records[i];
            return true;
        }
    }
    return false;
}

bool PatchInfoDatabase::AddRecord(void *pPEHeader, LPVOID lpDispatcherTable, unsigned EntryCount)
{
    PatchInfoRecord rec;
    bool Exists = false;
    if (FindRecord(pPEHeader, rec))
        Exists = true;
    QueryLoadedImage(pPEHeader, rec);
    rec.DispatcherTableOffset = (unsigned)((char *)lpDispatcherTable - (char *)pPEHeader);
    rec.EntryCount = EntryCount;
    if (Exists)
    {
        for (size_t i = 0; i < m_Records.Count(); i++)
        {
            if (((m_Records[i].FileSize) == rec.FileSize) &&
                ((m_Records[i].FirstSectionSize) == rec.FirstSectionSize) &&
                ((m_Records[i].TimeStamp) == rec.TimeStamp))
            {
                m_Records[i] = rec;
                break;
            }
        }
    }
    else
        m_Records.Add(rec);
    TCHAR tsz[MAX_PATH];
    _sntprintf(tsz, __countof(tsz), _T("%08X.vmpatch"), rec.TimeStamp);
    String fp = Path::Combine(m_Directory, tsz);
    unsigned n = 0;
    while (File::Exists(fp) && !Exists)
    {
        n++;
        _sntprintf(tsz, __countof(tsz), _T("%08X (%d).vmpatch"), rec.TimeStamp, n);
        fp = Path::Combine(m_Directory, tsz);
    }
    ManagedPointer<AIFile> pFile = new ACFile(fp, FileModes::CreateOrTruncateRW);
    if (!pFile->Valid())
    {
        if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
            g_pReporter->LogLineIfEnabled(_T("!!!!!!!!!!!! Failed to save patching parameters to database !!!!!!!!!!!!"));
    }
    pFile->Write(L"\xfeff", 2);
    return TextFileSerializer::Serialize(pFile, rec).Successful();
}

//-----------------------------------------------------------------------------------

IRPCTableManager* RPCTableManagerFactory(HINSTANCE hThisDLL, DWORD dwVersionMajor, DWORD dwVersionMinor, DWORD dwVersionBugFix)
{
    if (dwVersionMajor < 15 || (dwVersionMajor == 15 && dwVersionMinor < 5))
    {
        return new RPCTableManager<RPCHandlerRecord_Legacy>(hThisDLL);
    }
    return new RPCTableManager<RPCHandlerRecord_15_5>(hThisDLL);
}

enum
{
    MinStringLength = 10,
    MaxStringLength = 100,
    MaxRpcHandlers = 1024,
};

static inline bool IsValidStringChar(unsigned char ch)
{
    if ((ch >= 'a') && (ch <= 'z'))
        return true;
    if ((ch >= 'A') && (ch <= 'Z'))
        return true;
    if ((ch >= '0') && (ch <= '9'))
        return true;
    if ((ch == '_') || (ch == '.'))
        return true;
    return false;
}

template<class Record_T>
void RPCTableManager<Record_T>::FindSections(char *lpMainExe, std::list<AddressRange> &dataRanges, std::list<AddressRange> &codeRanges)
{
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(lpMainExe + ((PIMAGE_DOS_HEADER)lpMainExe)->e_lfanew);
    PIMAGE_SECTION_HEADER pSections = (PIMAGE_SECTION_HEADER)&pNtHdr[1];

    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
        g_pReporter->LogLineIfEnabled(_T("Building list of EXE sections... "));

    size_t totalSize = 0;
    for (unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++)
    {
        if (!(pSections[i].Characteristics & IMAGE_SCN_CNT_CODE))
        {
            AddressRange range(lpMainExe + pSections[i].VirtualAddress, lpMainExe + pSections[i].VirtualAddress + pSections[i].Misc.VirtualSize);
            totalSize += range.GetSize();
            dataRanges.push_back(range);
        }
        else
        {
            AddressRange range(lpMainExe + pSections[i].VirtualAddress, lpMainExe + pSections[i].VirtualAddress + pSections[i].Misc.VirtualSize);
            totalSize += range.GetSize();
            codeRanges.push_back(range);
        }
    }

    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
    {
        TCHAR tsz[512];
        _sntprintf(tsz, __countof(tsz), _T("%uK of data found.\r\nScanning for RPC command name strings...\r\n"), (unsigned int)(totalSize / 1024));
        g_pReporter->LogLineIfEnabled(tsz);
    }
}

template<class Record_T>
void RPCTableManager<Record_T>::MakeListOfStrings(const std::list<AddressRange> &dataRanges, BazisLib::SingleMallocVector<StringPointer> &strings, bool FullMode)
{
    for (std::list<AddressRange>::const_iterator it = dataRanges.begin(); it != dataRanges.end(); it++)
    {
        const AddressRange &range = *it;
        char *pPossibleStart = NULL;

        for (char *p = range.lpStart; p < range.lpEnd; p++)
        {
            if (pPossibleStart)
            {
                if (p[0])
                {
                    if (!IsValidStringChar(*p))
                        pPossibleStart = NULL;
                }
                else
                {
                    size_t len = p - pPossibleStart;
                    if ((len >= MinStringLength) && (len <= MaxStringLength))
                    {
                        if ((FullMode && strchr(pPossibleStart, '.')) || (!FullMode && (!memcmp(pPossibleStart, "tools.", 6))))
                            strings.Add(StringPointer(pPossibleStart, len));
                    }
                    pPossibleStart = NULL;
                }
            }
            else
            {
                if (IsValidStringChar(*p))
                    pPossibleStart = p;
            }
        }
    }

    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
    {
        TCHAR tsz[512];
        _sntprintf(tsz, __countof(tsz), _T("Finished scanning. Found %u strings.\r\nSearching for string references...\r\n"), (unsigned int)strings.Count());
        g_pReporter->LogLineIfEnabled(tsz);
    }
}

template<class Record_T>
void RPCTableManager<Record_T>::FindStringRefs(const std::list<AddressRange> &dataRanges,
    const BazisLib::SingleMallocVector<StringPointer> &strings,
    BazisLib::SingleMallocVector<StringReferenceDescriptor> &stringRefs)
{
    char *pMinPtr = 0, *pMaxPtr = 0;
    for (unsigned i = 0; i < strings.Count(); i++)
    {
        if ((strings[i].pszData < pMinPtr) || !pMinPtr)
            pMinPtr = strings[i].pszData;
        if ((strings[i].pszData > pMaxPtr) || !pMaxPtr)
            pMaxPtr = strings[i].pszData;
    }

    for (std::list<AddressRange>::const_iterator it = dataRanges.begin(); it != dataRanges.end(); it++)
    {
        const AddressRange &range = *it;

        for (char **p = (char **)range.lpStart; p < (char **)range.lpEnd; p++)
        {
            if ((*p < pMinPtr) || (*p > pMaxPtr))
                continue;
            for (unsigned i = 0; i < strings.Count(); i++)
                if (*p == strings[i].pszData)
                {
                    stringRefs.Add(StringReferenceDescriptor(p));
                    break;
                }
        }
    }
    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
    {
        TCHAR tsz[512];
        _sntprintf(tsz, __countof(tsz), _T("Found %u string references.\r\n"), (unsigned int)stringRefs.Count());
        g_pReporter->LogLineIfEnabled(tsz);
    }
}

template<class Record_T>
void RPCTableManager<Record_T>::GroupStringRefs(BazisLib::SingleMallocVector<StringReferenceDescriptor> &stringRefs,
    BazisLib::SingleMallocVector<RefGroupDescriptor> &groups)
{
    unsigned GroupCount = 0;

    for (unsigned i = 0; i < stringRefs.Count(); i++)
    {
        if (!stringRefs[i].GroupID)
            stringRefs[i].GroupID = ++GroupCount;
        for (unsigned j = i + 1; j < stringRefs.Count(); j++)
        {
            size_t dist = 0;
            if (stringRefs[i].pAddress > stringRefs[j].pAddress)
                dist = (size_t)stringRefs[i].pAddress - (size_t)stringRefs[j].pAddress;
            else
                dist = (size_t)stringRefs[j].pAddress - (size_t)stringRefs[i].pAddress;
            if ((dist < (sizeof(Record_T) * MaxRpcHandlers)) && !(dist % sizeof(Record_T)))
            {
                stringRefs[j].GroupID = stringRefs[i].GroupID;
            }
        }
    }

    groups.EnsureCapacity(GroupCount);
    groups.SetElementCount(GroupCount);
    memset(groups.GetRawPointer(), 0, sizeof(RefGroupDescriptor) * GroupCount);

    for (unsigned j = 0; j < stringRefs.Count(); j++)
    {
        if (stringRefs[j].GroupID)
        {
            groups[stringRefs[j].GroupID - 1].pAddr = (BYTE*)stringRefs[j].pAddress - offsetof(Record_T, pCommandPrefix);
            groups[stringRefs[j].GroupID - 1].RefCount++;
        }
    }

    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
    {
        TCHAR tsz[512];
        _sntprintf(tsz, __countof(tsz), _T("Found %d structures resemblant to RPC dispatcher table.\r\n"), GroupCount);
        g_pReporter->LogLineIfEnabled(tsz);
        if (GroupCount < 30)
        {
            for (unsigned i = 0; i < GroupCount; i++)
            {
                _sntprintf(tsz, __countof(tsz), _T("(address %p, matched pointers: %d)\r\n"), groups[i].pAddr, groups[i].RefCount);
                g_pReporter->LogLineIfEnabled(tsz);
            }
        }
        g_pReporter->LogLineIfEnabled(_T("Analyzing potential RPC dispatcher tables...\r\n"));
    }
}

template<class Record_T>
bool RPCTableManager<Record_T>::ScanPotentialRPCTable(void *pAddr,
    std::list<AddressRange> &dataRanges,
    std::list<AddressRange> &codeRanges,
    RPCTableInfo &info)
{
    Record_T *pRec = (Record_T *)pAddr;
    Record_T *pFirst = pRec, *pLast = pRec;
    for (unsigned i = 0; i < MaxRpcHandlers; i++)
    {
        if (!VerifyEntry(pFirst, dataRanges, codeRanges))
            break;
        pFirst--;
    }
    do { pFirst++; } while (!pFirst->Enabled && (pFirst < pRec));
    for (unsigned i = 0; i < MaxRpcHandlers; i++)
    {
        if (!VerifyEntry(pLast, dataRanges, codeRanges))
            break;
        pLast++;
    }
    do { pLast--; } while (!pLast->Enabled && (pLast > pRec));
    if (pFirst >= pLast)
        return false;
    info.pAddr = pFirst;
    info.EntryCount = (unsigned)(pLast - pFirst);
    info.UsedEntries = info.FreeEntries = 0;
    for (pRec = pFirst; pRec <= pLast; pRec++)
    {
        if (pRec->Enabled)
            info.UsedEntries++;
        else
            info.FreeEntries++;
    }
    return true;
}

template<class Record_T>
bool RPCTableManager<Record_T>::FindHandlerTable(bool FullMode)
{
    char *lpMainExe = (char *)GetModuleHandle(NULL);
    if (!lpMainExe)
        return false;

    std::list<AddressRange> dataRanges, codeRanges;
    BazisLib::SingleMallocVector<StringPointer> strings;
    BazisLib::SingleMallocVector<StringReferenceDescriptor> stringRefs;
    BazisLib::SingleMallocVector<RefGroupDescriptor> groups;

    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
        g_pReporter->LogLineIfEnabled(_T("Analyzing VMWARE-VMX executable...\r\n"));

    FindSections(lpMainExe, dataRanges, codeRanges);
    MakeListOfStrings(dataRanges, strings, FullMode);
    FindStringRefs(dataRanges, strings, stringRefs);
    GroupStringRefs(stringRefs, groups);

    unsigned matchCount = 0;
    for (unsigned i = 0; i < groups.Count(); i++)
    {
        if (!ScanPotentialRPCTable(groups[i].pAddr, dataRanges, codeRanges, groups[i].info))
            groups[i].info.pAddr = NULL;
        else
            matchCount++;
    }

    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
    {
        TCHAR tsz[512];
        _sntprintf(tsz, __countof(tsz), _T("Potential RPC table analysis complete. Found %d candidates.\r\n"), matchCount);
        g_pReporter->LogLineIfEnabled(tsz);
        if (matchCount < 30)
        {
            for (unsigned i = 0; i < groups.Count(); i++)
            {
                if (!groups[i].info.pAddr)
                    continue;
                _sntprintf(tsz, __countof(tsz), _T("(address %p, entries: %d, free entries: %d)\r\n"), groups[i].info.pAddr, groups[i].info.EntryCount, groups[i].info.FreeEntries);
                g_pReporter->LogLineIfEnabled(tsz);
            }
        }
    }

    unsigned maxMatchID = -1;
    for (unsigned i = 0; i < groups.Count(); i++)
    {
        if (!groups[i].info.pAddr)
            continue;
        if ((maxMatchID == -1) || (groups[i].info.EntryCount > groups[maxMatchID].info.EntryCount))
            maxMatchID = i;
    }

    if (maxMatchID == -1)
        return false;

    m_Database.AddRecord(lpMainExe, groups[maxMatchID].info.pAddr, groups[maxMatchID].info.EntryCount);
    return true;
}

static inline String GetDLLDirectory(HINSTANCE hDLL)
{
    TCHAR tsz[MAX_PATH];
    GetModuleFileName(hDLL, tsz, __countof(tsz));
    String fp(tsz);
    return Path::GetDirectoryName(fp);
}

#include <ShlObj.h>

static String CreateAndGetDatabaseDirectory()
{
    String fpDir = Path::GetSpecialDirectoryLocation(BazisLib::Win32::SpecialDirFromCSIDL(CSIDL_APPDATA)) + String(_T("\\VirtualKD"));
    if (!Directory::Exists(fpDir))
        Directory::Create(fpDir);
    return fpDir;
}

template<class Record_T>
RPCTableManager<Record_T>::RPCTableManager(HINSTANCE hThisDLL)
    : m_Database(CreateAndGetDatabaseDirectory())
    , m_pPatchedEntry(NULL)
{
    const TCHAR tszRegistryPath[] = _T("SOFTWARE\\BazisSoft\\KDVMWare\\Patcher");
    RegistryKey key(HKEY_LOCAL_MACHINE, tszRegistryPath);
    BazisLib::Win32::RegistrySerializer::Deserialize(key, m_Params);
    BazisLib::Win32::RegistrySerializer::Serialize(key, m_Params);
}

template<class Record_T>
bool RPCTableManager<Record_T>::InstallHandler(const char *pszPrefix, size_t prefixLen, GRPCHANDLER pHandler, void *pContext, bool ForceReinstall)
{
    if (m_pPatchedEntry && !ForceReinstall)
        return false;
    PatchInfoRecord rec;

    char *lpMainExe = (char *)GetModuleHandle(NULL);
    if (!lpMainExe)
        return false;

    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
    {
        g_pReporter->LogLineIfEnabled(_T("*************************************************************************************\r\n"));
        g_pReporter->LogLineIfEnabled(_T("*VirtualKD-Redux patcher DLL successfully loaded. Patching the GuestRPC mechanism...*\r\n"));
        g_pReporter->LogLineIfEnabled(_T("*************************************************************************************\r\n"));
        g_pReporter->LogLineIfEnabled(_T("Searching patch database for information about current executable...\r\n"));
    }
    if (m_Params.DisablePatchDatabase || !m_Database.FindRecord(lpMainExe, rec) || !rec.DispatcherTableOffset)
    {
        if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
            g_pReporter->LogLineIfEnabled(_T("No information found.\r\n"));
        FILETIME ftStart, ftUnused;
        if (GetProcessTimes(GetCurrentProcess(), &ftStart, &ftUnused, &ftUnused, &ftUnused))
        {
            TimeSpan elapsed = DateTime::Now() - ftStart;
            unsigned msPassed = (unsigned)elapsed.GetTotalMilliseconds();
            if (msPassed < m_Params.MinRunTimeBeforePatch)
            {
                int toWait = m_Params.MinRunTimeBeforePatch - msPassed;
                if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
                {
                    TCHAR tsz[512];
                    _sntprintf_s(tsz, __countof(tsz), _TRUNCATE, _T("Waiting for VMWare to initialize (%d ms more to wait)\r\n"), toWait);
                    g_pReporter->LogLineIfEnabled(tsz);
                }
                if (toWait > 0)
                    Sleep(toWait);
            }
        }
        if (!FindHandlerTable(false))
        {
            if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
                g_pReporter->LogLineIfEnabled(_T("Cannot determine RPC dispatcher table location.\r\nPerforming additional analysis.\r\n"));
            if (!FindHandlerTable(true))
            {
                if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
                {
                    g_pReporter->LogLineIfEnabled(_T("Still cannot find RPC dispatcher table.\r\n"));
                    g_pReporter->LogLineIfEnabled(_T("Please report this issue at https://github.com/4d61726b/VirtualKD-Redux/issues\r\n"));
                }
                return false;
            }
        }
    }

    if (!m_Database.FindRecord(lpMainExe, rec) || !rec.DispatcherTableOffset)
        return false;

    PVOID pDispatcherTable = (char *)lpMainExe + rec.DispatcherTableOffset;

    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
    {
        TCHAR tsz[512];
        _sntprintf(tsz, __countof(tsz), _T("Using RPC dispatcher table at 0x%08I64X (%d entries)\r\n"), (ULONGLONG)pDispatcherTable, rec.EntryCount);
        g_pReporter->LogLineIfEnabled(tsz);
    }

    Record_T *pFirst = (Record_T *)pDispatcherTable;
    Record_T *pLast = pFirst + rec.EntryCount;

    if (m_Params.WaitForNonZeroFirstCommand)
    {
        if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
        {
            TCHAR tsz[512];
            _sntprintf(tsz, __countof(tsz), _T("Waiting for RPC table to be initialized by VMWare..."));
            g_pReporter->LogLineIfEnabled(tsz);
        }
        while (!pFirst->Enabled || !pFirst->CommandPrefixLength || !pFirst->pCommandPrefix || !pFirst->pHandler)
        {
            if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
                g_pReporter->LogLineIfEnabled(_T("."));
            Sleep(200);
        }
        Sleep(200);
        if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
            g_pReporter->LogLineIfEnabled(_T("\r\nRPC table initialized. Patching it...\r\n"));
    }

    memset(&m_OriginalHandler, 0, sizeof(Record_T));

    if (!m_Params.DefaultPatchingAtTableStart)
    {
        for (Record_T *pRec = pFirst; pRec < pLast; pRec++)
        {
            if (!pRec->Enabled)
            {
                DoPatch(pRec, pszPrefix, prefixLen, pHandler, pContext);
                if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
                {
                    TCHAR tsz[512];
                    _sntprintf(tsz, __countof(tsz), _T("Successfully patched entry #%u\r\n"), (unsigned int)(pRec - pFirst));
                    g_pReporter->LogLineIfEnabled(tsz);
                }
                return true;
            }
        }
    }

    if (m_Params.DefaultPatchingAtTableStart || m_Params.AllowPatchingAtTableStart)
    {
        if (!memcmp(&m_OriginalHandler, &pFirst[-1], sizeof(Record_T)))
        {
            DoPatch(--pFirst, pszPrefix, prefixLen, pHandler, pContext);
            if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
            {
                TCHAR tsz[512];
                _sntprintf(tsz, __countof(tsz), _T("Successfully patched entry before the first used one (%p)\r\n"), &pFirst[-1]);
                g_pReporter->LogLineIfEnabled(tsz);
            }
            return true;
        }
    }

    if (m_Params.AllowReplacingFirstCommand)
    {
        if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
        {
            TCHAR tsz[512];
            _sntprintf(tsz, __countof(tsz), _T("No suitable free entries found. Replaced handler for %s\r\n"), ANSIStringToString(TempStrPointerWrapperA(pFirst->pCommandPrefix, pFirst->CommandPrefixLength)).c_str());
            g_pReporter->LogLineIfEnabled(tsz);
        }
        DoPatch(pFirst, pszPrefix, prefixLen, pHandler, pContext);
        return true;
    }
    if (s_EnableLogging && (g_pReporter->GetStatusPointer()->DebugLevel >= DebugLevelTracePatching))
    {
        g_pReporter->LogLineIfEnabled(_T("Cannot find a usable entry to patch! Exiting.\r\n"));
        g_pReporter->LogLineIfEnabled(_T("Please report this issue at https://github.com/4d61726b/VirtualKD-Redux/issues\r\n"));
    }
    return false;
}

template<class Record_T>
void RPCTableManager<Record_T>::RestoreOriginalHandler()
{
    if (m_pPatchedEntry)
    {
        *m_pPatchedEntry = m_OriginalHandler;
        m_pPatchedEntry = NULL;
    }
}