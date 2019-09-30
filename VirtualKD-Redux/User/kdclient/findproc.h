/*! \file findproc.h
    \author Ivan Shcherbakov (Bazis)
    $Id: findproc.h,v 1.2 2009-06-07 15:31:55 Administrator Exp $
    \brief Contains declaration for process enumeration functions
	The API provided by this file is deprecated. Use the <b>ToolhelpSnapshot</b> class from BazisLib instead.
*/

#pragma once

int FindProcessByName(HANDLE hSnapshot, TCHAR *pszName, bool RestartSearch);
int FindProcessByNames(HANDLE hSnapshot, TCHAR **pszName, unsigned NameCount, bool RestartSearch, unsigned *pMatchedIndex);
int FindProcessByName(TCHAR *pszName);
LPVOID GetModuleFromProcess(int PID, TCHAR *pszModuleName);
int GetFirstThreadID(int PID);
