/*! \file
    \author Ivan Shcherbakov (Bazis)
    $Id: 32to64.h,v 1.3 2009-12-31 13:06:41 Bazis Exp $
    \brief Contains declaration for \ref vm32to64
*/

#pragma once

enum KDClientCommand
{
	kGetVMSessionName = 1,
	kIsSessionPatched,
	kStartVMSessionPatching,
	kStartVMSessionUnpatching,
	kPatchAndWait,
	kUnpatchAndWait,
	kQueryVersion,
};

ULONGLONG Call64BitKDCLIENT(KDClientCommand cmd, unsigned PID, LPWSTR lpStringResult = NULL, size_t StringSize = 0);

//! Called by 64-bit RUNDLL32.EXE to perform 64-bit operation from 32-bit DLL
extern "C" __declspec(dllexport) void KdClient32To64Entry();
