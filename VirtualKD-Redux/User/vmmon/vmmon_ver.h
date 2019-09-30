#pragma once
#include "../vkdversion.h"

/////////////////////////////////////////////////////////////////////////////
//
// Version
//

VS_VERSION_INFO VERSIONINFO
 FILEVERSION VMMON_EXE_VER
 PRODUCTVERSION VIRTUALKD_VERSION
 FILEFLAGSMASK 0x3fL
#ifdef _DEBUG
 FILEFLAGS 0x1L
#else
 FILEFLAGS 0x0L
#endif
 FILEOS 0x4L
 FILETYPE 0x2L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904b0"
        BEGIN
            VALUE "Comments", COMMENTS_STR
            VALUE "CompanyName", COMPANY_STR
            VALUE "FileDescription", "Virtual Machine Monitor"
            VALUE "FileVersion", VMMON_EXE_VER_STR
            VALUE "InternalName", "vmmon"
            VALUE "LegalCopyright", COPYRIGHT_STR
            VALUE "LegalTrademarks", TRADEMARKS_STR
            VALUE "OriginalFilename", "vmmon.exe"
            VALUE "ProductName", PRODUCTNAME_STR
            VALUE "ProductVersion", VIRTUALKD_VERSION_STR
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x409, 1200
    END
END
