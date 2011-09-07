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

:Build
@echo [ZEBuild Externals] Info : Building %ProjectName%.
vcbuild /Clean Source\win32\VS2008\libogg_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\win32\VS2008\libogg_static.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\win32\VS2008\libogg_static.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error

:CopyOutput
@echo [ZEBuild Externals] Info : Copying output of %ProjectName%.
xcopy /r /y Source\win32\VS2008\Win32\Release\libogg_static.lib Output\Lib\Win32\
move  Output\Lib\Win32\libogg_static.lib Output\Lib\Win32\libogg.lib
xcopy /r /y Source\win32\VS2008\x64\Release\libogg_static.lib Output\Lib\Win64\
move  Output\Lib\Win64\libogg_static.lib Output\Lib\Win64\libogg.lib
xcopy /r /y Source\include\ogg\ogg.h Output\Include\ogg\
xcopy /r /y Source\include\ogg\os_types.h Output\Include\ogg\
goto End

:Error
cd ../../..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0