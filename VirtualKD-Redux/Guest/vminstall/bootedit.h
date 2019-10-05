#pragma once
#include <BazisLib/bzscore/objmgr.h>

namespace BootEditor
{
	using namespace BazisLib;

	enum KernelDebuggerMode
	{
		kdNone,
		kdUnknown,
		kdStandard,
		kdCustom,
	};

	class AIBootConfigurationEntry : AUTO_INTERFACE
	{
	public:
		virtual BazisLib::String GetDescription()=0;
		virtual ActionStatus SetDescription(LPCWSTR pDescription)=0;

		virtual bool IsCurrentOS()=0;

		virtual KernelDebuggerMode GetDebuggerMode()=0;
		virtual String GetCustomKDName()=0;

		virtual ActionStatus EnableCustomKD(LPCWSTR pCustomKDName)=0;
		virtual ActionStatus ExplicitlyDisableDebugging() = 0;
	};

	class AIBootConfigurationEditor : AUTO_INTERFACE
	{
	public:
		virtual ActionStatus StartEnumeration()=0;
		virtual ManagedPointer<AIBootConfigurationEntry> GetNextEntry()=0;

		virtual ManagedPointer<AIBootConfigurationEntry> CopyEntry(const ConstManagedPointer<AIBootConfigurationEntry> &pEntry, bool AddToSelectionList = true, ActionStatus *pStatus = NULL)=0;
		virtual ActionStatus FinishEditing()=0;

		virtual ActionStatus SetDefaultEntry(const ConstManagedPointer<AIBootConfigurationEntry> &pEntry)=0;

		virtual unsigned GetTimeout(ActionStatus *pStatus = NULL)=0;
		virtual ActionStatus SetTimeout(unsigned TimeoutInSeconds)=0;

		virtual size_t GetEntryCount()=0;
	};

	ManagedPointer<AIBootConfigurationEditor> CreateConfigurationEditor();
}