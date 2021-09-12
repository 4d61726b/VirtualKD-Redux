mkdir Bundles

mkdir Bundles\ReleaseBundle
copy Build\Win32\Release\kdclient32.dll Bundles\ReleaseBundle\
copy Build\x64\Release\kdclient64.dll Bundles\ReleaseBundle\
copy Build\Win32\Release\vmmon32.exe Bundles\ReleaseBundle\
copy Build\x64\Release\vmmon64.exe Bundles\ReleaseBundle\
copy Build\Win32\Release\vmxpatch32.exe Bundles\ReleaseBundle\
copy Build\x64\Release\vmxpatch64.exe Bundles\ReleaseBundle\
copy Build\AnyCPU\Release\VirtualBoxIntegration.exe Bundles\ReleaseBundle\
mkdir Bundles\ReleaseBundle\target32
mkdir Bundles\ReleaseBundle\target64
REM copy Build\Win32\Release\kdpatch.reg Bundles\ReleaseBundle\target32
REM copy Build\x64\Release\kdpatch.reg Bundles\ReleaseBundle\target64
copy Build\Win32\Release\vminstall.exe Bundles\ReleaseBundle\target32
copy Build\x64\Release\vminstall.exe Bundles\ReleaseBundle\target64
copy Build\Win32\Release\kdbazis.dll Bundles\ReleaseBundle\target32
copy Build\x64\Release\kdbazis.dll Bundles\ReleaseBundle\target64
REM copy Build\Win32\Release\kdpatch.sys Bundles\ReleaseBundle\target32
REM copy Build\x64\Release\kdpatch.sys Bundles\ReleaseBundle\target64
copy Certs\VKD-Redux_CA.cer Bundles\ReleaseBundle\

mkdir Bundles\DebugBundle
copy Build\Win32\Debug\kdclient32.dll Bundles\DebugBundle\
copy Build\x64\Debug\kdclient64.dll Bundles\DebugBundle\
copy Build\Win32\Debug\vmmon32.exe Bundles\DebugBundle\
copy Build\x64\Debug\vmmon64.exe Bundles\DebugBundle\
copy Build\Win32\Debug\vmxpatch32.exe Bundles\DebugBundle\
copy Build\x64\Debug\vmxpatch64.exe Bundles\DebugBundle\
copy Build\AnyCPU\Debug\VirtualBoxIntegration.exe Bundles\DebugBundle\
mkdir Bundles\DebugBundle\target32
mkdir Bundles\DebugBundle\target64
REM copy Build\Win32\Debug\kdpatch.reg Bundles\DebugBundle\target32
REM copy Build\x64\Debug\kdpatch.reg Bundles\DebugBundle\target64
copy Build\Win32\Debug\vminstall.exe Bundles\DebugBundle\target32
copy Build\x64\Debug\vminstall.exe Bundles\DebugBundle\target64
copy Build\Win32\Debug\kdbazis.dll Bundles\DebugBundle\target32
copy Build\x64\Debug\kdbazis.dll Bundles\DebugBundle\target64
REM copy Build\Win32\Debug\kdpatch.sys Bundles\DebugBundle\target32
REM copy Build\x64\Debug\kdpatch.sys Bundles\DebugBundle\target64
copy Certs\VKD-Redux_CA.cer Bundles\DebugBundle\
