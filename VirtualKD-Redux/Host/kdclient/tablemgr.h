/*! \file tablemgr.h
    \author Ivan Shcherbakov (Bazis)
    $Id: tablemgr.h,v 1.11 2009-06-07 15:31:55 Administrator Exp $
    \brief Contains declaration for patcher engine and patch database
*/

#pragma once

#include <BazisLib/bzshlp/smvec.h>
#include <list>
#include <BazisLib/bzshlp/serializer.h>
#include "regconfig.h"

static const int s_EnableLogging = true;

using namespace BazisLib;

DECLARE_SERIALIZEABLE_STRUC5(PatchInfoRecord,
    unsigned, TimeStamp,
    unsigned, FileSize,
    unsigned, FirstSectionSize,
    unsigned, DispatcherTableOffset,
    unsigned, EntryCount);

//! Stores information about \ref guestrpc handler table location for different versions of VMWARE-VMX.EXE
class PatchInfoDatabase
{
private:
    BazisLib::String m_Directory;
    BazisLib::SingleMallocVector<PatchInfoRecord> m_Records;

private:
    static bool QueryLoadedImage(void *pPEHeader, PatchInfoRecord &rec);
    void AddBuiltInRecords();

public:
    PatchInfoDatabase(const BazisLib::String &Directory);
    ~PatchInfoDatabase();

    //! Finds a record in database
    /*!
        \param pPEHeader Points to the PE header of the VMWARE-VMX.EXE to search for.
        \param record Points to a record to be filled
        \return The function returns <b>true</b> if the record was found in the database.
    */
    bool FindRecord(void *pPEHeader, PatchInfoRecord &record);
    bool AddRecord(void *pPEHeader, LPVOID lpDispatcherTable, unsigned EntryCount);
};

//! Performs patching of \ref guestrpc handler table
/*!
    VMWare main process (vmware-vmx.exe) contains a table of RPC dispatchers. The RPCTableManager finds this table
    by building a list of all strings in the original EXE module and finding a structure containing links to the matching
    strings.
*/

#pragma region RPC handler definitions
typedef bool(*GRPCHANDLER)(void *pContext, int ChannelNumber, char *pCommandBody, unsigned CommandBodyLength, char **ppReply, unsigned *pReplyLen);
//! Represents a \ref guestrpc handler entry used by VMWARE-VMX.EXE (Pre 15.5)
struct RPCHandlerRecord_Legacy
{
    char *pCommandPrefix;
    unsigned CommandPrefixLength;
    GRPCHANDLER pHandler;
    void *pHandlerContext;

    bool Enabled;
};
//! Represents a \ref guestrpc handler entry used by VMWARE-VMX.EXE (15.5+)
struct RPCHandlerRecord_15_5
{
    GRPCHANDLER pHandler;
    void *pHandlerContext;
    void *pUnused;
    char *pCommandPrefix;
    unsigned CommandPrefixLength;
    bool Enabled;
};
#pragma endregion


class IRPCTableManager {
public:
    virtual bool InstallHandler(const char *pszPrefix, size_t prefixLen, GRPCHANDLER pHandler, void *pContext, bool ForceReinstall) = 0;
    virtual void RestoreOriginalHandler() = 0;
    virtual bool IsEntryModified() = 0;
};
IRPCTableManager* RPCTableManagerFactory(HINSTANCE hThisDLL, DWORD dwVersionMajor, DWORD dwVersionMinor, DWORD dwVersionBugFix);

template<class Record_T>
class RPCTableManager : public IRPCTableManager
{
private:
#pragma region Auxillary structures
    //! Represents an arbitrary address range
    struct AddressRange
    {
        char *lpStart;
        char *lpEnd;

        AddressRange(LPVOID start, LPVOID end)
            : lpStart((char *)start)
            , lpEnd((char *)end)
        {
            ASSERT(lpEnd > lpStart);
        }

        size_t GetSize()
        {
            return lpEnd - lpStart;
        }
    };

    //! Represents a pointer to an ASCIIZ string with a cached size
    struct StringPointer
    {
        char *pszData;
        size_t length;

        StringPointer(char *psz, size_t len)
            : pszData(psz)
            , length(len)
        {
        }
    };

    //! Represents a pointer to a string pointer. Used to locate \ref guestrpc handler table with pointers to command names
    struct StringReferenceDescriptor
    {
        void *pAddress;
        unsigned GroupID;

        StringReferenceDescriptor(void *addr)
            : pAddress(addr)
            , GroupID(0)
        {

        }
    };

    //! Contains information about a \ref guestrpc handler table
    struct RPCTableInfo
    {
        void *pAddr;
        unsigned EntryCount;
        unsigned UsedEntries;
        unsigned FreeEntries;
    };

    //! Contains information about a set of pointers to strings that can potentially be a \ref guestrpc handler table
    struct RefGroupDescriptor
    {
        void *pAddr;
        unsigned RefCount;
        RPCTableInfo info;

        RefGroupDescriptor(void *addr)
            : pAddr(addr)
            , RefCount(0)
        {
            memset(&info, 0, sizeof(info));
        }
    };

#pragma endregion

private:
    PatchInfoDatabase m_Database;
    PatchingParams m_Params;

    Record_T m_OriginalHandler, m_PatchedEntryCopy;
    Record_T *m_pPatchedEntry;

private:

    enum
    {
        VerifMaxStringLength = 100,
    };

    static inline bool PtrInRange(const std::list<AddressRange> &list, void *ptr)
    {
        for (std::list<AddressRange>::const_iterator it = list.begin(); it != list.end(); it++)
            if ((ptr >= it->lpStart) && (ptr <= it->lpEnd))
                return true;
        return false;
    }

    //! Returns whether pStr points to an ASCIIZ string with length of exact <b>expected</b> bytes.
    static inline bool islen(const char *pStr, size_t expected)
    {
        for (size_t i = 0; i < expected; i++)
            if (!pStr[i])
                return false;
        return !pStr[expected];
    }

    //! Returns whether a potential \ref guestrpc handler entry is correct and contains no inconsisancies.
    static inline bool VerifyEntry(Record_T *pRec, std::list<AddressRange> &dataRanges, std::list<AddressRange> &codeRanges)
    {
        switch (*((char *)&pRec->Enabled))
        {
        case 0:
            //			if (pRec->CommandPrefixLength || pRec->pCommandPrefix || pRec->pHandler)
            //				return false;
            break;
        case 1:
            if (pRec->CommandPrefixLength > VerifMaxStringLength)
                return false;
            if (!PtrInRange(dataRanges, pRec->pCommandPrefix))
                return false;
            if (!islen(pRec->pCommandPrefix, pRec->CommandPrefixLength))
                return false;
            if (!PtrInRange(codeRanges, pRec->pHandler))
                return false;
            return true;
        default:
            return false;
        }
        return true;
    }

    static void FindSections(char *lpMainExe, std::list<AddressRange> &dataRanges, std::list<AddressRange> &codeRanges);
    static void MakeListOfStrings(const std::list<AddressRange> &dataRanges, BazisLib::SingleMallocVector<StringPointer> &strings, bool FullMode);
    static void FindStringRefs(const std::list<AddressRange> &dataRanges,
        const BazisLib::SingleMallocVector<StringPointer> &strings,
        BazisLib::SingleMallocVector<StringReferenceDescriptor> &stringRefs);
    static void GroupStringRefs(BazisLib::SingleMallocVector<StringReferenceDescriptor> &stringRefs,
        BazisLib::SingleMallocVector<RefGroupDescriptor> &groups);

    static bool ScanPotentialRPCTable(void *pAddr,
        std::list<AddressRange> &dataRanges,
        std::list<AddressRange> &codeRanges,
        RPCTableInfo &info);

private:
    //! Tries to load handler table information from PatchInfoDatabase, and, if not found, scans for it.
    bool FindHandlerTable(bool FullMode = false);
    void DoPatch(Record_T *pEntry, const char *pszPrefix, size_t prefixLen, GRPCHANDLER pHandler, void *pContext)
    {
        m_OriginalHandler = *pEntry;
        pEntry->pCommandPrefix = (char *)pszPrefix;
        pEntry->CommandPrefixLength = (unsigned)prefixLen;
        pEntry->pHandler = pHandler;
        pEntry->pHandlerContext = pContext;
        pEntry->Enabled = true;
        m_pPatchedEntry = pEntry;
        m_PatchedEntryCopy = *pEntry;
    }

public:
    RPCTableManager(HINSTANCE hThisDLL);
    bool InstallHandler(const char *pszPrefix, size_t prefixLen, GRPCHANDLER pHandler, void *pContext, bool ForceReinstall);
    void RestoreOriginalHandler();

    bool IsEntryModified()
    {
        if (!m_pPatchedEntry)
            return false;
        //TODO: insert invertion here and test repeated patching bug
        return (memcmp(m_pPatchedEntry, &m_PatchedEntryCopy, sizeof(Record_T)) != 0);
    }

};
