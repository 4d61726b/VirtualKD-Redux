SET VS160COMNTOOLS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools
call "%VS160COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsall.bat" x64
echo on

msbuild /m VirtualKD-Redux.sln /property:Platform=Win32 /property:Configuration="Debug" || goto error
msbuild /m VirtualKD-Redux.sln /property:Platform=x64 /property:Configuration="Debug" || goto error
msbuild /m VirtualKD-Redux.sln /property:Platform=Win32 /property:Configuration="Release" || goto error
msbuild /m VirtualKD-Redux.sln /property:Platform=x64 /property:Configuration="Release" || goto error

goto success

:error
echo Build failed!
goto end

:success
call "createbundle.bat"

:end
pause

