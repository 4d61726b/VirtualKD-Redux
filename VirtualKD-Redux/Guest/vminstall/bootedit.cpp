#include "stdafx.h"
#include "bootedit.h"
#include <BazisLib/bzshlp/Win32/bcd.h>
#include <BazisLib/bzscore/Win32/registry.h>
#include <BazisLib/bzshlp/textfile.h>
#include <BazisLib/bzscore/strfast.h>

#include <map>

bool IsWin8OrLater();

using namespace BootEditor;

namespace VistaBCD
{
    using namespace BazisLib::Win32::BCD;

    class BootEntry : public AIBootConfigurationEntry
    {
    private:
        BCDObject m_Object;

        friend class BootConfigEditor;

    public:
        virtual BazisLib::String GetDescription()
        {
            return m_Object.GetElement(BcdLibraryString_Description).ToString();
        }

        virtual ActionStatus SetDescription(LPCWSTR pDescription)
        {
            return m_Object.SetElement(BcdLibraryString_Description, pDescription);
        }

        virtual bool IsCurrentOS()
        {
            wchar_t wszWinDir[MAX_PATH] = { 0, }, wszRawPath[MAX_PATH] = { 0, };
            GetWindowsDirectoryW(wszWinDir, __countof(wszWinDir));

            wszWinDir[2] = 0;
            if (!QueryDosDevice(wszWinDir, wszRawPath, __countof(wszRawPath)))
                return false;

            BCDDeviceData devData = m_Object.GetElement(BcdOSLoaderDevice_OSDevice).ToDeviceData();
            if (!devData.Valid())
                return false;

            if (devData.GetPartitionPath().icompare(wszRawPath))
                return false;	//Partition mismatch

            wszWinDir[2] = '\\';

            if (m_Object.GetElement(BcdOSLoaderString_SystemRoot).ToString().icompare(&wszWinDir[2]))
                return false;

            return true;
        }

        virtual KernelDebuggerMode GetDebuggerMode()
        {
            if (!m_Object.Valid())
                return kdUnknown;
            if (!m_Object.GetElement(BcdOSLoaderBoolean_KernelDebuggerEnabled).ToBoolean())
                return kdNone;
            if (!m_Object.GetElement(BcdOSLoaderString_DbgTransportPath).ToString().empty())
                return kdCustom;
            return kdStandard;
        }

        virtual String GetCustomKDName()
        {
            if (!m_Object.GetElement(BcdOSLoaderBoolean_KernelDebuggerEnabled).ToBoolean())
                return String();
            return m_Object.GetElement(BcdOSLoaderString_DbgTransportPath).ToString();
        }

        virtual ActionStatus EnableCustomKD(LPCWSTR pCustomKDName)
        {
            if (!pCustomKDName)
                return MAKE_STATUS(InvalidParameter);
            ActionStatus st = m_Object.SetElement(BcdOSLoaderString_DbgTransportPath, pCustomKDName);
            if (!st.Successful())
                return st;
            st = m_Object.SetElement(BcdLibraryBoolean_AllowPrereleaseSignatures, true);
            ActionStatus st2 = m_Object.SetElement(BcdOSLoaderBoolean_AllowPrereleaseSignatures, true);
            if (!st.Successful() && !st2.Successful())
                return st;
            st = m_Object.SetElement(BcdOSLoaderBoolean_KernelDebuggerEnabled, true);
            if (!st.Successful())
                return st;

            if (IsWin8OrLater())
            {
                st = m_Object.SetElement((BCDElementType)0x250000c2, (ULONGLONG)0); //Enable old-style boot selection menu.
                if (!st.Successful())
                    return st;
                st = m_Object.SetElement(BcdLibraryBoolean_AutoRecoveryEnabled, false);
                if (!st.Successful())
                    return st;
                st = m_Object.SetElement(BcdLibraryInteger_DebuggerType, (ULONGLONG)0); //Set DebuggerType to DebuggerSerial
                if (!st.Successful())
                    return st;
            }

            return MAKE_STATUS(Success);
        }

        virtual ActionStatus ExplicitlyDisableDebugging()
        {
            return m_Object.SetElement(BcdOSLoaderBoolean_KernelDebuggerEnabled, false);
        }

        bool Valid()
        {
            return m_Object.Valid() && (m_Object.GetType() == BCDObject::WindowsLoader);
        }

    public:
        BootEntry(const BCDObject &obj)
            : m_Object(obj)
        {
        }
    };

    class BootConfigEditor : public AIBootConfigurationEditor
    {
    private:
        BCDStore m_Store;

        unsigned m_NextBootObject;

    public:
        BootConfigEditor()
            : m_Store(BCDStore::OpenStore())
            , m_NextBootObject(0)
        {
        }

        virtual ActionStatus StartEnumeration()
        {
            m_NextBootObject = 0;
            return MAKE_STATUS(Success);
        }

        virtual ManagedPointer<AIBootConfigurationEntry> GetNextEntry()
        {
            for (;;)
            {
                if (m_NextBootObject >= m_Store.GetObjectCount())
                    return NULL;
                const BCDObject &obj = m_Store.GetObjects()[m_NextBootObject++];
                if (obj.GetType() != BCDObject::WindowsLoader)
                    continue;
                return new BootEntry(obj);
            }
        }

        virtual size_t GetEntryCount()
        {
            return m_Store.GetObjectCount();
        }

        virtual ManagedPointer<AIBootConfigurationEntry> CopyEntry(const ConstManagedPointer<AIBootConfigurationEntry> &pEntry, bool AddToSelectionList = true, ActionStatus *pStatus = NULL)
        {
            if (!pEntry)
            {
                ASSIGN_STATUS(pStatus, InvalidParameter);
                return NULL;
            }
            ManagedPointer<BootEntry> pBootEntry = pEntry;
            if (!pBootEntry->Valid())
            {
                ASSIGN_STATUS(pStatus, InvalidParameter);
                return NULL;
            }


            BCDObject newObj = m_Store.CopyObject(pBootEntry->m_Object, pStatus);
            if (!newObj.Valid())
            {
                ASSIGN_STATUS(pStatus, UnknownError);
                return NULL;
            }

            if (AddToSelectionList)
            {
                BCDObject globalSettings = m_Store.GetFirstObjectOfType(BCDObject::GlobalSettings);
                if (!globalSettings.Valid())
                {
                    ASSIGN_STATUS(pStatus, UnknownError);
                    return NULL;
                }
                BCDObjectList selectionList = globalSettings.GetElement(BcdBootMgrObjectList_DisplayOrder).GetObjectList();
                if (!selectionList.Valid())
                {
                    ASSIGN_STATUS(pStatus, UnknownError);
                    return NULL;
                }

                ActionStatus st = selectionList.InsertObject(newObj);
                if (!st.Successful())
                {
                    ASSIGN_STATUS(pStatus, UnknownError);
                    return NULL;
                }
            }

            ASSIGN_STATUS(pStatus, Success);
            return new BootEntry(newObj);
        }

        virtual ActionStatus FinishEditing()
        {
            return MAKE_STATUS(Success);
        }

        bool Valid()
        {
            return m_Store.Valid();
        }

        virtual ActionStatus SetDefaultEntry(const ConstManagedPointer<AIBootConfigurationEntry> &pEntry)
        {
            if (!pEntry)
                return MAKE_STATUS(InvalidParameter);
            ManagedPointer<BootEntry> pBootEntry = pEntry;
            if (!pBootEntry || !pBootEntry->Valid())
                return MAKE_STATUS(InvalidParameter);

            BCDObject globalSettings = m_Store.GetFirstObjectOfType(BCDObject::GlobalSettings);
            if (!globalSettings.Valid())
                return MAKE_STATUS(UnknownError);

            return globalSettings.SetElement(BcdBootMgrObject_DefaultObject, pBootEntry->m_Object);
        }

        virtual ActionStatus SetTimeout(unsigned TimeoutInSeconds)
        {
            BCDObject globalSettings = m_Store.GetFirstObjectOfType(BCDObject::GlobalSettings);
            if (!globalSettings.Valid())
                return MAKE_STATUS(UnknownError);

            return globalSettings.SetElement(BcdBootMgrInteger_Timeout, (ULONGLONG)TimeoutInSeconds);
        }

        virtual unsigned GetTimeout(ActionStatus *pStatus = NULL)
        {
            BCDObject globalSettings = m_Store.GetFirstObjectOfType(BCDObject::GlobalSettings);
            if (!globalSettings.Valid())
            {
                ASSIGN_STATUS(pStatus, UnknownError);
                return -1;
            }

            BCDElement el = globalSettings.GetElement(BcdBootMgrInteger_Timeout);
            if (!el.Valid())
            {
                ASSIGN_STATUS(pStatus, UnknownError);
                return -1;
            }

            return (unsigned)el.ToInteger();
        }

    };
}


namespace BootIniEditor
{
    class BootEntry : public AIBootConfigurationEntry
    {
    private:
        DECLARE_REFERENCE(class BootConfigEditor, m_pEditor);
        unsigned m_EntryIndex;

    public:
        virtual BazisLib::String GetDescription();

        virtual ActionStatus SetDescription(LPCWSTR pDescription);
        virtual bool IsCurrentOS();
        virtual KernelDebuggerMode GetDebuggerMode();
        virtual String GetCustomKDName();
        virtual ActionStatus EnableCustomKD(LPCWSTR pCustomKDName);

        bool Valid()
        {
            return (m_EntryIndex != -1);
        }

    public:
        BootEntry(const ConstManagedPointer<BootConfigEditor> &pEditor, int EntryIndex)
            : INIT_REFERENCE(m_pEditor, pEditor)
            , m_EntryIndex(EntryIndex)
        {
        }

        unsigned GetEntryIndex()
        {
            return m_EntryIndex;
        }

        virtual ActionStatus ExplicitlyDisableDebugging()
        {
            return MAKE_STATUS(Success);	//Not needed on XP
        }
    };

    class BootConfigEditor : public AIBootConfigurationEditor
    {
    private:
        class EntryBase
        {
        protected:
            bool m_Modified;
            DynamicStringA m_OriginalString;

            EntryBase()
                : m_Modified(false)
            {
            }

        public:

            bool IsModified()
            {
                return m_Modified;
            }

            const DynamicStringA &GetOriginalString()
            {
                return m_OriginalString;
            }

            virtual DynamicStringA BuildNewString() const = 0;

        };

        class SimpleEntry : public EntryBase
        {
        private:
            DynamicStringA m_Value;

        public:

            static DynamicStringA GetKey(const TempStringA &Line)
            {
                size_t off = Line.find('=');
                if (off == -1)
                    return DynamicStringA();
                DynamicStringA res = Line.substr(0, off);
                off = res.find_last_not_of(" \t");
                if (off == -1)
                    off = res.length() - 1;
                res.erase(off + 1);
                return res;
            }

            SimpleEntry(const TempStringA &Line = ConstStringA(""))
            {
                size_t off = Line.find('=');
                if (off == -1)
                    return;
                off++;
                off = Line.find_first_not_of(" \t", off);
                if (off == -1)
                    return;
                m_Value = Line.substr(off);
                m_OriginalString = Line;
            }

        public:
            const DynamicStringA &GetValue()
            {
                return m_Value;
            }

            void SetValue(const TempStringA &NewValue)
            {
                m_Value = NewValue;
                m_Modified = true;
            }

            void SetValue(unsigned NewValue)
            {
                m_Value.Format("%d", NewValue);
                m_Modified = true;
            }

            unsigned ToInteger()
            {
                return atoi(m_Value.c_str());
            }

        public:
            virtual DynamicStringA BuildNewString() const
            {
                DynamicStringA ret = GetKey(m_OriginalString);
                ret += '=';
                ret += m_Value;
                return ret;
            }

        };

        class _CaseInsensitiveLess : public std::binary_function<DynamicStringA, DynamicStringA, bool>
        {
        public:
            bool operator()(const DynamicStringA &_Left, const DynamicStringA &_Right) const
            {
                return _stricmp(_Left.c_str(), _Right.c_str()) < 0;
            }
        };

        class OSEntry : public EntryBase
        {
        private:
            DynamicStringA m_Path;
            DynamicStringA m_Description;
            std::map<DynamicStringA, DynamicStringA, _CaseInsensitiveLess> m_Properties;

            bool m_Valid;

        public:
            OSEntry(const TempStringA &Line)
                : m_Valid(false)
            {
                SplitStrByFirstOfA keyAndValue(Line, ConstStringA("= \t"));
                if (!keyAndValue)
                    return;

                m_Path = keyAndValue.left;

                for (size_t context = 0;;)
                {
                    TempStringA token = FastStringRoutines::GetNextToken<false>(keyAndValue.right, &context);
                    if (token.empty())
                        break;
                    if (m_Description.empty())
                        m_Description = token;
                    else
                    {
                        if (token[0] != '/')
                            return;
                        SplitStrByFirstOfA keyAndVal(token.substr(1), ConstStringA("="));
                        if (keyAndVal)
                            m_Properties[keyAndVal.left] = keyAndVal.right;
                        else
                            m_Properties[token.substr(1)] = DynamicStringA();
                    }
                }
                if (m_Description.empty())
                    return;

                m_OriginalString = Line;
                m_Valid = true;
            }


            bool Valid()
            {
                return m_Valid;
            }

            const DynamicStringA &GetDescription() const { return m_Description; }
            const DynamicStringA &GetPath() const { return m_Path; }

            const DynamicStringA GetProperty(const DynamicStringA &Key)
            {
                std::map<DynamicStringA, DynamicStringA, _CaseInsensitiveLess>::iterator it = m_Properties.find(Key);
                if (it == m_Properties.end())
                    return DynamicStringA();
                return it->second;
            }

            const bool IsPropertySet(const DynamicStringA &Key)
            {
                return m_Properties.find(Key) != m_Properties.end();
            }

            void SetProperty(const DynamicStringA &key, const DynamicStringA &Value = DynamicStringA())
            {
                m_Properties[key] = Value;
                m_Modified = true;
            }

            void SetDescription(const TempStringA &Description)
            {
                m_Description = Description;
                m_Modified = true;
            }

            void FlagAsCopy()
            {
                m_OriginalString.clear();
                m_Modified = true;
            }

            bool IsNewEntry() const
            {
                return m_Modified && m_OriginalString.empty();
            }

        public:
            virtual DynamicStringA BuildNewString() const
            {
                DynamicStringA ret = GetPath();
                ret.AppendFormat("=\"%s\"", GetDescription().c_str());
                for each(const std::pair<DynamicStringA, DynamicStringA> &kv in m_Properties)
                {
                    if (!kv.second.empty())
                        ret.AppendFormat(" /%s=%s", kv.first.c_str(), kv.second.c_str());
                    else
                        ret.AppendFormat(" /%s", kv.first.c_str());
                }
                return ret;
            }

        };


        SimpleEntry m_Timeout, m_Default;
        unsigned m_PendingEntry, m_DefaultEntryIndex;

        std::vector<OSEntry> m_OSEntries;

        bool m_bValid;

#define BOOT_INI_FILE_NAME	_T("c:\\boot.ini")
#define BOOT_BAK_FILE_NAME	_T("c:\\boot.vkd")

    public:
        BootConfigEditor()
            : m_PendingEntry(0)
            , m_bValid(false)
            , m_DefaultEntryIndex(-1)
        {
            ManagedPointer<TextANSIFileReader> pBootIni = new TextANSIFileReader(new ACFile(BOOT_INI_FILE_NAME, FileModes::OpenReadOnly));
            if (!pBootIni->Valid())
                return;

            enum CurrentSection
            {
                csUnknown,
                csLoader,
                csOpSystems,
            } curSection = csUnknown;

            while (!pBootIni->IsEOF())
            {
                DynamicStringA line = pBootIni->ReadLine();
                if (line.empty())
                    continue;
                if (line[0] == '[')
                {
                    if (!line.icompare("[boot loader]"))
                        curSection = csLoader;
                    else if (!line.icompare("[operating systems]"))
                        curSection = csOpSystems;
                    else
                        curSection = csUnknown;
                    continue;
                }
                if (curSection == csLoader)
                {
                    DynamicStringA key = SimpleEntry::GetKey(line);
                    if (!key.icompare("timeout"))
                        m_Timeout = SimpleEntry(line);
                    else if (!key.icompare("default"))
                        m_Default = SimpleEntry(line);
                }
                else if (curSection == csOpSystems)
                {
                    OSEntry entry(line);
                    if (entry.Valid())
                        m_OSEntries.push_back(entry);
                }
            }
            if (!m_OSEntries.empty() && !m_Default.GetValue().empty() && !m_Timeout.GetValue().empty())
                m_bValid = true;
        }

        virtual ActionStatus StartEnumeration()
        {
            m_PendingEntry = 0;
            return MAKE_STATUS(Success);
        }

        virtual ManagedPointer<AIBootConfigurationEntry> GetNextEntry()
        {
            if (m_PendingEntry >= m_OSEntries.size())
                return NULL;
            return new BootEntry(ManagedPointer<BootConfigEditor>(AUTO_THIS), m_PendingEntry++);
        }

        virtual size_t GetEntryCount()
        {
            return m_OSEntries.size();
        }

        virtual ManagedPointer<AIBootConfigurationEntry> CopyEntry(const ConstManagedPointer<AIBootConfigurationEntry> &pEntry, bool AddToSelectionList = true, ActionStatus *pStatus = NULL)
        {
            ASSERT(pEntry);
            ManagedPointer<BootEntry> pRawEntry = pEntry;
            if (!pRawEntry)
            {
                ASSIGN_STATUS(pStatus, InvalidParameter);
                return NULL;
            }

            m_OSEntries.push_back(m_OSEntries[pRawEntry->GetEntryIndex()]);

            m_OSEntries[m_OSEntries.size() - 1].FlagAsCopy();

            ASSIGN_STATUS(pStatus, Success);
            return new BootEntry(ManagedPointer<BootConfigEditor>(AUTO_THIS), (int)(m_OSEntries.size() - 1));
        }

        virtual ActionStatus FinishEditing()
        {
            DynamicString tempFileName = BOOT_INI_FILE_NAME;
            tempFileName.AppendFormat(L".tmp%04x", GetCurrentProcessId());

            ActionStatus st;
            ManagedPointer<TextANSIFileReader> pBootIni = new TextANSIFileReader(new ACFile(BOOT_INI_FILE_NAME, FileModes::OpenReadOnly, &st));
            if (!pBootIni->Valid())
            {
                if (!st.Successful())
                    return st;
                return MAKE_STATUS(UnknownError);
            }

            File newBootIni(tempFileName, FileModes::CreateOrTruncateRW, &st);
            if (!st.Successful())
                return st;
            if (!newBootIni.Valid())
                return MAKE_STATUS(UnknownError);

            size_t entryCount = 2 + m_OSEntries.size();
            //All changes made to BOOT.INI are represented as a list of (old line -> new line) pairs.
            //This basically replaces all matching 'old lines' with corresponding 'new lines', leaving
            //other parts of BOOT.INI untouched.
            EntryBase **ppEntriesToChange = new EntryBase*[entryCount];

            ppEntriesToChange[0] = &m_Timeout;
            ppEntriesToChange[1] = &m_Default;
            for (size_t i = 0; i < m_OSEntries.size(); i++)
            {
                if (m_OSEntries[i].IsModified() && !m_OSEntries[i].GetOriginalString().empty() && (m_DefaultEntryIndex != i))
                    ppEntriesToChange[2 + i] = &m_OSEntries[i];
                else
                    ppEntriesToChange[2 + i] = NULL;
            }

            OSEntry *pNewDefaultEntry = NULL;
            if (m_DefaultEntryIndex != -1)
                pNewDefaultEntry = &m_OSEntries[m_DefaultEntryIndex];

            bool bOSSectionStarted = false, bNewEntriesAdded = false;

            //Each line of BOOT.INI is processed separately
            while (!pBootIni->IsEOF())
            {
                DynamicStringA line = pBootIni->ReadLine();

                bool lineReplaced = false;
                ActionStatus status;

                //Replace the line, if it matches one of the changed entries
                if (!line.empty() && (line[0] != '['))
                {
                    if (pNewDefaultEntry && (pNewDefaultEntry->GetOriginalString() == line))
                        continue;	//Default entry is always placed first and is removed from its old position

                    for (size_t i = 0; i < entryCount; i++)
                    {
                        if (!ppEntriesToChange[i] || !ppEntriesToChange[i]->IsModified())
                            continue;
                        if (!line.compare(ppEntriesToChange[i]->GetOriginalString()))
                        {
                            newBootIni.WriteLine(ppEntriesToChange[i]->BuildNewString(), &status);
                            lineReplaced = true;
                            break;
                        }
                    }
                }

                //Insert all new entries BEFORE the line, if it is an empty line in the [operating systems] section (or a 'C:\xxx' line)
                if (bOSSectionStarted && !bNewEntriesAdded)
                {
                    if (line.empty() || ((line.length() >= 2) && line[1] == ':'))
                    {
                        for each (const OSEntry &entry in m_OSEntries)
                            if (entry.IsNewEntry() && (pNewDefaultEntry != &entry))
                                newBootIni.WriteLine(entry.BuildNewString());
                        bNewEntriesAdded = true;
                    }
                }

                //Write the line, if it was not changed
                if (!lineReplaced)
                    newBootIni.WriteLine(line, &status);

                //Insert default entry right after the [operating systems] line
                if (pNewDefaultEntry && (line == "[operating systems]"))
                {
                    bOSSectionStarted = true;
                    newBootIni.WriteLine(pNewDefaultEntry->IsModified() ? pNewDefaultEntry->BuildNewString() : pNewDefaultEntry->GetOriginalString());
                }

                if (!status.Successful())
                {
                    delete ppEntriesToChange;
                    break;
                }

            }

            if (!bNewEntriesAdded)
            {
                for each (const OSEntry &entry in m_OSEntries)
                    if (entry.IsNewEntry() && (pNewDefaultEntry != &entry))
                        newBootIni.WriteLine(entry.BuildNewString());
            }

            newBootIni.Close();
            delete ppEntriesToChange;

            pBootIni = NULL;

            if (File::Exists(BOOT_BAK_FILE_NAME))
                File::Delete(BOOT_BAK_FILE_NAME, true);

            if (!MoveFile(BOOT_INI_FILE_NAME, BOOT_BAK_FILE_NAME))
            {
                DWORD err = GetLastError();
                DeleteFile(tempFileName.c_str());
                return MAKE_STATUS(ActionStatus::FromWin32Error(err));
            }

            if (!MoveFile(tempFileName.c_str(), BOOT_INI_FILE_NAME))
            {
                DWORD err = GetLastError();
                MoveFile(BOOT_BAK_FILE_NAME, BOOT_INI_FILE_NAME);
                DeleteFile(tempFileName.c_str());
                return MAKE_STATUS(ActionStatus::FromWin32Error(err));
            }

            DWORD attr = ::GetFileAttributes(BOOT_BAK_FILE_NAME);
            if (attr != INVALID_FILE_ATTRIBUTES)
                ::SetFileAttributes(BOOT_INI_FILE_NAME, attr);

            return MAKE_STATUS(Success);
        }

        bool Valid()
        {
            return m_bValid;
        }

        virtual ActionStatus SetDefaultEntry(const ConstManagedPointer<AIBootConfigurationEntry> &pEntry)
        {
            ASSERT(pEntry);
            ManagedPointer<BootEntry> pRawEntry = pEntry;
            if (!pRawEntry)
                return MAKE_STATUS(InvalidParameter);

            DynamicStringA defaultPath = m_OSEntries[pRawEntry->GetEntryIndex()].GetPath();
            if (defaultPath.empty())
                return MAKE_STATUS(InvalidParameter);

            m_DefaultEntryIndex = pRawEntry->GetEntryIndex();

            m_Default.SetValue(defaultPath);
            return MAKE_STATUS(Success);
        }

        virtual ActionStatus SetTimeout(unsigned TimeoutInSeconds)
        {
            m_Timeout.SetValue(TimeoutInSeconds);
            return MAKE_STATUS(Success);
        }

        virtual unsigned GetTimeout(ActionStatus *pStatus = NULL)
        {
            ASSIGN_STATUS(pStatus, Success);
            return m_Timeout.ToInteger();
        }

        friend class BootEntry;

    };


    BazisLib::String BootEntry::GetDescription()
    {
        ASSERT(m_pEditor);
        ASSERT(m_EntryIndex < m_pEditor->m_OSEntries.size());
        return ANSIStringToString(m_pEditor->m_OSEntries[m_EntryIndex].GetDescription());
    }

    BazisLib::ActionStatus BootEntry::SetDescription(LPCWSTR pDescription)
    {
        ASSERT(m_pEditor);
        ASSERT(m_EntryIndex < m_pEditor->m_OSEntries.size());
        m_pEditor->m_OSEntries[m_EntryIndex].SetDescription(StringToANSIString(TempStrPointerWrapper(pDescription)));
        return MAKE_STATUS(Success);
    }

    bool BootEntry::IsCurrentOS()
    {
        ASSERT(m_pEditor);
        ASSERT(m_EntryIndex < m_pEditor->m_OSEntries.size());
        DynamicStringA path = m_pEditor->m_OSEntries[m_EntryIndex].GetPath();
        SplitStrByFirstOfA deviceAndDir(path, ConstStringA("\\"));
        if (!deviceAndDir)
            return false;

        RegistryKey key(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control"));
        DynamicStringA systemBootDevice = StringToANSIString(key[_T("SystemBootDevice")]);

        if (systemBootDevice.empty() && m_pEditor->GetEntryCount() == 1)
        {
            // Old version of Windows that predates SystemBootDevice. Since there is only one entry, its generally safe
            // to say that this is the correct entry. This should be safe to set since it wasn't used.
            key[L"SystemBootDevice"].SetValueEx(ANSIStringToString(deviceAndDir.left).c_str(), (DWORD)(deviceAndDir.left.length() + 1) * sizeof(WCHAR), REG_SZ);
        }
        else
        {
            if (systemBootDevice.icompare(deviceAndDir.left))
                return false;
        }

        char szWinDir[MAX_PATH] = { 0, };
        GetWindowsDirectoryA(szWinDir, __countof(szWinDir));

        if (deviceAndDir.right.icompare(&szWinDir[3]))
            return false;

        return true;
    }

    BootEditor::KernelDebuggerMode BootEntry::GetDebuggerMode()
    {
        ASSERT(m_pEditor);
        ASSERT(m_EntryIndex < m_pEditor->m_OSEntries.size());

        if (!m_pEditor->m_OSEntries[m_EntryIndex].IsPropertySet("DEBUG"))
            return kdNone;

        DynamicStringA port = m_pEditor->m_OSEntries[m_EntryIndex].GetProperty("DEBUGPORT");
        if (port.empty())
            return kdUnknown;

        if (!port.icompare("1394") || !port.icompare(0, 3, "COM"))
            return kdStandard;

        return kdCustom;
    }

    BazisLib::String BootEntry::GetCustomKDName()
    {
        ASSERT(m_pEditor);
        ASSERT(m_EntryIndex < m_pEditor->m_OSEntries.size());

        String kdBase = ANSIStringToString(m_pEditor->m_OSEntries[m_EntryIndex].GetProperty("DEBUGPORT"));
        if (kdBase.empty())
            return kdBase;

        kdBase.insert(0, _T("kd"));
        kdBase.append(_T(".dll"));

        return kdBase;
    }

    BazisLib::ActionStatus BootEntry::EnableCustomKD(LPCWSTR pCustomKDName)
    {
        if (!pCustomKDName || (towupper(pCustomKDName[0]) != 'K') || (towupper(pCustomKDName[1]) != 'D'))
            return MAKE_STATUS(InvalidParameter);

        DynamicStringA customKDName = StringToANSIString(TempStrPointerWrapperW(pCustomKDName + 2));
        if ((customKDName.size() < 4) || (customKDName.substr(customKDName.size() - 4).icompare(".dll")))
            return MAKE_STATUS(InvalidParameter);

        ASSERT(m_pEditor);
        ASSERT(m_EntryIndex < m_pEditor->m_OSEntries.size());

        m_pEditor->m_OSEntries[m_EntryIndex].SetProperty("DEBUG");
        m_pEditor->m_OSEntries[m_EntryIndex].SetProperty("DEBUGPORT", customKDName.substr(0, customKDName.size() - 4));
        return MAKE_STATUS(Success);
    }
}

ManagedPointer<AIBootConfigurationEditor> BootEditor::CreateConfigurationEditor()
{
    OSVERSIONINFO version = { sizeof(version) };
    GetVersionEx(&version);
    if (version.dwMajorVersion >= 6) //We are running Vista or higher
    {
        ManagedPointer<VistaBCD::BootConfigEditor> pEditor = new VistaBCD::BootConfigEditor();
        if (!pEditor->Valid())
            return NULL;
        return pEditor;
    }
    else	//We are running on XP or similar and are modifying BOOT.INI
    {
        ManagedPointer<BootIniEditor::BootConfigEditor> pEditor = new BootIniEditor::BootConfigEditor();
        if (!pEditor->Valid())
            return NULL;
        return pEditor;
    }
}