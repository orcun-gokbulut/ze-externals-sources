:Initialization
@echo off
set ProjectName=libmpg123
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
vcbuild /Clean Source\ports\MSVC++\2008\mpg123.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\ports\MSVC++\2008\mpg123.sln "Debug|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\ports\MSVC++\2008\Debug\libmpg123.lib Output\Lib\Win32\Debug\
xcopy /r /y Source\ports\MSVC++\2008\Debug\libmpg123.dll Output\Dll\Win32\Debug\
xcopy /r /y Source\src\libmpg123\mpg123.h.in Output\Include\Win32\
xcopy /r /y Source\ports\MSVC++\mpg123.h Output\Include\Win32\

:Build32Release
vcbuild /Clean Source\ports\MSVC++\2008\mpg123.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\ports\MSVC++\2008\mpg123.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\ports\MSVC++\2008\Release\libmpg123.lib Output\Lib\Win32\Release\
xcopy /r /y Source\ports\MSVC++\2008\Release\libmpg123.dll Output\Dll\Win32\Release\

:Build64Debug
vcbuild /Clean Source\ports\MSVC++\2008\mpg123.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\ports\MSVC++\2008\mpg123.sln "Debug|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\ports\MSVC++\2008\Debug\libmpg123.lib Output\Lib\Win64\Debug\
xcopy /r /y Source\ports\MSVC++\2008\Debug\libmpg123.dll Output\Dll\Win64\Debug\
xcopy /r /y Source\src\libmpg123\mpg123.h.in Output\Include\Win64\
xcopy /r /y Source\ports\MSVC++\mpg123.h Output\Include\Win64\

:Build64Release
vcbuild /Clean Source\ports\MSVC++\2008\mpg123.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\ports\MSVC++\2008\mpg123.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\ports\MSVC++\2008\Release\libmpg123.lib Output\Lib\Win64\Release\
xcopy /r /y Source\ports\MSVC++\2008\Release\libmpg123.dll Output\Dll\Win64\Release\

goto End

:Error
cd ../../..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0