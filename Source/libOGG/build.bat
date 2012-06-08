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
msbuild /t:clean Source\win32\VS2010\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\libogg_static.sln /p:configuration=debug /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\Win32\Debug\libogg.lib Output\Windows\x86\Lib\Debug\
xcopy /r /y Source\include\ogg\ogg.h Output\Windows\x86\Include\ogg\
xcopy /r /y Source\include\ogg\os_types.h Output\Windows\x86\Include\ogg\

:Build32Release
msbuild /t:clean Source\win32\VS2010\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\libogg_static.sln /p:configuration=release /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\Win32\Release\libogg.lib Output\Windows\x86\Lib\Release\

:Build64Debug
msbuild /t:clean Source\win32\VS2010\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\libogg_static.sln /p:configuration=debug /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\x64\Debug\libogg.lib Output\Windows\x64\Lib\Debug\
xcopy /r /y Source\include\ogg\ogg.h Output\Windows\x64\Include\ogg\
xcopy /r /y Source\include\ogg\os_types.h Output\Windows\x64\Include\ogg\

:Build64Release
msbuild /t:clean Source\win32\VS2010\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\libogg_static.sln /p:configuration=release /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\x64\Release\libogg.lib Output\Windows\x64\Lib\Release\

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1
