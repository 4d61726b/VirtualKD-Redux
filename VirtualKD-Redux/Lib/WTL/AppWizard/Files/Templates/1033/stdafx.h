// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

// Change these values to use different versions
[!if WTL_SUPPORT_WINXP]
#define WINVER		0x0501
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0600
#define _RICHEDIT_VER	0x0300
[!else]
#define WINVER		0x0601
#define _WIN32_WINNT	0x0601
#define _WIN32_IE	0x0700
#define _RICHEDIT_VER	0x0500
[!endif]

[!if WTL_COM_SERVER]
#define _ATL_APARTMENT_THREADED

[!endif]
[!if WTL_NO_AUTO_NS]
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _WTL_NO_AUTOMATIC_NAMESPACE

[!endif]
#include <atlbase.h>
#include <atlapp.h>

[!if WTL_COM_SERVER]
extern [!output WTL_NS]CServerAppModule _Module;

[!else]
extern [!output WTL_NS]CAppModule _Module;

[!endif]
[!if WTL_ENABLE_AX || WTL_COM_SERVER]
#include <atlcom.h>
[!endif]
[!if WTL_ENABLE_AX]
#include <atlhost.h>
[!endif]
#include <atlwin.h>
[!if WTL_ENABLE_AX]
#include <atlctl.h>
[!endif]
[!if WTL_USE_CPP_FILES]

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
[!if WTL_USE_CMDBAR]
#include <atlctrlw.h>
[!endif]
[!if WTL_APPTYPE_TABVIEW]
#include <atlctrlx.h>
[!endif]
[!if WTL_APPTYPE_EXPLORER]
#include <atlctrlx.h>
#include <atlsplit.h>
[!endif]
[!if WTL_USE_VIEW]
[!if WTL_VIEWTYPE_SCROLL]
#include <atlscrl.h>
[!endif]
[!endif]
[!if WTL_USE_RIBBON]
#include <atlribbon.h>
[!endif]
[!endif]

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
