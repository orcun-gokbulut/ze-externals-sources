:Initialization
@echo off
set ProjectName=libogg
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
del /s /q /f Output
rmdir /s /q Output
del /q /f Source\zconf.h

:Build32Debug
@echo [ZEBuild Externals] Info : Building %ProjectName%.
vcbuild /Clean Source\win32\VS2008\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\win32\VS2008\libogg_static.sln "Debug|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2008\Win32\Debug\libogg_static.lib Output\Lib\Win32\Debug\
move  Output\Lib\Win32\Debug\libogg_static.lib Output\Lib\Win32\Debug\libogg.lib
xcopy /r /y Source\include\ogg\ogg.h Output\Include\Win32\ogg\
xcopy /r /y Source\include\ogg\os_types.h Output\Include\Win32\ogg\

:Build32Release
vcbuild /Clean Source\win32\VS2008\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\win32\VS2008\libogg_static.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2008\Win32\Release\libogg_static.lib Output\Lib\Win32\Release\
move  Output\Lib\Win32\Release\libogg_static.lib Output\Lib\Win32\Release\libogg.lib

:Build64Debug
vcbuild /Clean Source\win32\VS2008\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\win32\VS2008\libogg_static.sln "Debug|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2008\x64\Debug\libogg_static.lib Output\Lib\Win64\Debug\
move  Output\Lib\Win64\Debug\libogg_static.lib Output\Lib\Win64\Debug\libogg.lib
xcopy /r /y Source\include\ogg\ogg.h Output\Include\Win64\ogg\
xcopy /r /y Source\include\ogg\os_types.h Output\Include\Win64\ogg\

:Build64Release
vcbuild /Clean Source\win32\VS2008\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\win32\VS2008\libogg_static.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2008\x64\Release\libogg_static.lib Output\Lib\Win64\Release\
move  Output\Lib\Win64\Release\libogg_static.lib Output\Lib\Win64\Release\libogg.lib

goto End

:Error
cd ../../..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0