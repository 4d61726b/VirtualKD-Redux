/*! \file
    \author Ivan Shcherbakov (Bazis)
    $Id: hook64.h,v 1.3 2009-06-07 15:31:55 Administrator Exp $
    \brief Contains declaration for GetRemoteModuleHandle64Aware()
*/

#pragma once

//! Gets module base for other process in a way, compatible with both x86 and x64.
ULONGLONG GetRemoteModuleHandle64Aware(unsigned PID, LPCTSTR lpModuleName, bool ShortName = true);