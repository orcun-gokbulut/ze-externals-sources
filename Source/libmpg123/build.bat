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
del /s /q /f Source\ports\MSVC++\2008\Debug\
msbuild /t:clean Source\ports\MSVC++\2008\mpg123.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\ports\MSVC++\2008\mpg123.sln /p:configuration=debug /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\ports\MSVC++\2008\Debug\libmpg123.lib Output\Windows\x86\Lib\Debug\
xcopy /r /y Source\ports\MSVC++\2008\Debug\libmpg123.dll Output\Windows\x86\Dll\Debug\
xcopy /r /y Source\src\libmpg123\mpg123.h.in Output\Windows\x86\Include\
xcopy /r /y Source\ports\MSVC++\mpg123.h Output\Windows\x86\Include\

:Build32Release
del /s /q /f Source\ports\MSVC++\2008\Release\
msbuild /t:clean Source\ports\MSVC++\2008\mpg123.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\ports\MSVC++\2008\mpg123.sln /p:configuration=release /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\ports\MSVC++\2008\Release\libmpg123.lib Output\Windows\x86\Lib\Release\
xcopy /r /y Source\ports\MSVC++\2008\Release\libmpg123.dll Output\Windows\x86\Dll\Release\

:Build64Debug
del /s /q /f Source\ports\MSVC++\2008\Debug\
msbuild /t:clean Source\ports\MSVC++\2008\mpg123.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\ports\MSVC++\2008\mpg123.sln /p:configuration=debug /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\ports\MSVC++\2008\x64\Debug_Generic_Dll\libmpg123.lib Output\Windows\x64\Lib\Debug\
xcopy /r /y Source\ports\MSVC++\2008\Debug\libmpg123.dll Output\Windows\x64\Dll\Debug\
xcopy /r /y Source\src\libmpg123\mpg123.h.in Output\Windows\x64\Include\
xcopy /r /y Source\ports\MSVC++\mpg123.h Output\Windows\x64\Include\

:Build64Release
del /s /q /f Source\ports\MSVC++\2008\Release\
msbuild /t:clean Source\ports\MSVC++\2008\mpg123.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\ports\MSVC++\2008\mpg123.sln /p:configuration=release /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\ports\MSVC++\2008\x64\Release_Generic_Dll\libmpg123.lib Output\Windows\x64\Lib\Release\
xcopy /r /y Source\ports\MSVC++\2008\Release\libmpg123.dll Output\Windows\x64\Dll\Release\

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
cd ../../..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1