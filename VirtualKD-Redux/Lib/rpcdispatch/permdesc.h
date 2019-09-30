/*! \file
    \author Ivan Shcherbakov (Bazis)
    $Id: permdesc.h,v 1.2 2009-06-06 12:48:44 Administrator Exp $
    \brief Contains implementation for PermissiveSecurityDescriptor
*/

#pragma once

//! Represents a securty descriptor granting all access to everyone
class PermissiveSecurityDescriptor
{
private:
	SECURITY_DESCRIPTOR m_SD;
	SECURITY_ATTRIBUTES m_SA;
	bool m_Used;
public:
	PermissiveSecurityDescriptor()
		: m_Used(false)
	{
		InitializeSecurityDescriptor (&m_SD, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl (&m_SD, TRUE, NULL, FALSE);
		m_SA.nLength = sizeof(m_SA);
		m_SA.lpSecurityDescriptor = &m_SD;
		m_SA.bInheritHandle = FALSE;
	}

	operator SECURITY_ATTRIBUTES *()
	{
		m_Used = true;
		return &m_SA;
	}

	~PermissiveSecurityDescriptor()
	{
		ASSERT(m_Used);
	}
};
