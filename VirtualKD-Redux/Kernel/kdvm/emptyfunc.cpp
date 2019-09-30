/*! \file emptyfunc.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: emptyfunc.cpp,v 1.5 2009-06-07 10:08:59 Administrator Exp $
    \brief Contains implementation of unused API exported by KDVM.dll
*/

#include "stdafx.h"

#define DEFINE_EMPTYFUNC0(name) NTSTATUS name() {return STATUS_SUCCESS;}
#define DEFINE_EMPTYFUNC1(name) NTSTATUS name(void *) {return STATUS_SUCCESS;}

extern "C"
{
    DEFINE_EMPTYFUNC0(KdD0Transition)
    DEFINE_EMPTYFUNC0(KdD3Transition)
    DEFINE_EMPTYFUNC1(KdDebuggerInitialize1)

	DEFINE_EMPTYFUNC1(KdRestore)
	DEFINE_EMPTYFUNC1(KdSave)

	NTSTATUS KdSetHiberRange(void *)
	{
		return STATUS_NOT_SUPPORTED;	//As in KDCOM.DLL
	}
}
