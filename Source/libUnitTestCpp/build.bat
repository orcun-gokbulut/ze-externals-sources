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

:Build32Debug
@echo [ZEBuild Externals] Info : Building %ProjectName%.

vcbuild /Upgrade Source\UnitTest++.vsnet2005.sln
if %ERRORLEVEL% NEQ 0 GOTO Error

vcbuild /Clean Source\UnitTest++.vsnet2005.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\UnitTest++.vsnet2005.sln "Debug|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Debug\UnitTest++.vsnet2005.lib Output\Lib\Win32\Debug\
move  Output\Lib\Win32\Debug\UnitTest++.vsnet2005.lib Output\Lib\Win32\Debug\libUnitTestCpp.lib
xcopy /r /y Source\src\*.h Output\Include\Win32\UnitTestCpp\

:Build32Release
vcbuild /Clean Source\UnitTest++.vsnet2005.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\UnitTest++.vsnet2005.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Release\UnitTest++.vsnet2005.lib Output\Lib\Win32\Release\
move  Output\Lib\Win32\Release\UnitTest++.vsnet2005.lib Output\Lib\Win32\Release\libUnitTestCpp.lib

:Build64Debug
vcbuild /Clean Source\UnitTest++.vsnet2005.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\UnitTest++.vsnet2005.sln "Debug|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\x64\Debug\UnitTest++.vsnet2005.lib Output\Lib\Win64\Debug\
move  Output\Lib\Win64\Debug\UnitTest++.vsnet2005.lib Output\Lib\Win64\Debug\libUnitTestCpp.lib
xcopy /r /y Source\src\*.h Output\Include\Win32\UnitTestCpp\
xcopy /r /y Source\src\*.h Output\Include\Win64\UnitTestCpp\

:Build64Release
vcbuild /Clean Source\UnitTest++.vsnet2005.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\UnitTest++.vsnet2005.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\x64\Release\UnitTest++.vsnet2005.lib Output\Lib\Win64\Release\
move  Output\Lib\Win64\Release\UnitTest++.vsnet2005.lib Output\Lib\Win64\Release\libUnitTestCpp.lib

goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0