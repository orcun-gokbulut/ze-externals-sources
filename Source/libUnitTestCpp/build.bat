:Initialization
@echo off
set ProjectName=libUnitTestCpp
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
del /s /q /f Output
rmdir /s /q Output

:Build
@echo [ZEBuild Externals] Info : Building %ProjectName%.

vcbuild /Upgrade Source\UnitTest++.vsnet2005.sln
if %ERRORLEVEL% NEQ 0 GOTO Error

vcbuild /Clean Source\UnitTest++.vsnet2005.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist

vcbuild Source\UnitTest++.vsnet2005.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Release\UnitTest++.vsnet2005.lib Output\Lib\Win32\
move  Output\Lib\Win32\UnitTest++.vsnet2005.lib Output\Lib\Win32\libUnitTestCpp.lib

vcbuild /Clean Source\UnitTest++.vsnet2005.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist

vcbuild Source\UnitTest++.vsnet2005.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\x64\Release\UnitTest++.vsnet2005.lib Output\Lib\Win64\
move  Output\Lib\Win64\UnitTest++.vsnet2005.lib Output\Lib\Win64\libUnitTestCpp.lib


:CopyOutput
@echo [ZEBuild Externals] Info : Copying output of %ProjectName%.
xcopy /r /y Source\src\*.h Output\Include\UnitTestCpp\
goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0