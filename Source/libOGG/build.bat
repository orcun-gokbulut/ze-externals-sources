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
xcopy /r /y Source\win32\VS2010\Win32\Debug\libogg.lib Output\Lib\Win32\Debug\
xcopy /r /y Source\include\ogg\ogg.h Output\Include\Win32\ogg\
xcopy /r /y Source\include\ogg\os_types.h Output\Include\Win32\ogg\

:Build32Release
msbuild /t:clean Source\win32\VS2010\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\libogg_static.sln /p:configuration=release /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\Win32\Release\libogg.lib Output\Lib\Win32\Release\

:Build64Debug
msbuild /t:clean Source\win32\VS2010\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\libogg_static.sln /p:configuration=debug /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\x64\Debug\libogg.lib Output\Lib\Win64\Debug\
xcopy /r /y Source\include\ogg\ogg.h Output\Include\Win64\ogg\
xcopy /r /y Source\include\ogg\os_types.h Output\Include\Win64\ogg\

:Build64Release
msbuild /t:clean Source\win32\VS2010\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\libogg_static.sln /p:configuration=release /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\x64\Release\libogg.lib Output\Lib\Win64\Release\

goto End

:Error
cd ../../..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0