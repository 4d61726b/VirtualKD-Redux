mkdir Bundle
copy Build\Win32\Release\kdclient.dll Bundle\
copy Build\x64\Release\kdclient64.dll Bundle\
copy Build\Win32\Release\vmmon.exe Bundle\
copy Build\x64\Release\vmmon64.exe Bundle\
copy Build\Win32\Release\vmxpatch.exe Bundle\
copy Build\x64\Release\vmxpatch64.exe Bundle\
mkdir Bundle\target32
mkdir Bundle\target64
copy Build\Win32\Release\kdpatch.reg Bundle\target32
copy Build\x64\Release\kdpatch.reg Bundle\target64
copy Build\Win32\Release\vminstall.exe Bundle\target32
copy Build\x64\Release\vminstall.exe Bundle\target64
copy Build\Win32\Release\kdbazis.dll Bundle\target32
copy Build\x64\Release\kdbazis.dll Bundle\target64
copy Build\Win32\Release\kdpatch.sys Bundle\target32
copy Build\x64\Release\kdpatch.sys Bundle\target64


