:Initialization
@echo off
@echo [ZEBuild Externals] Info : Building externals.
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" 

set ZE_BUILD_CMAKE=%CD%\Utils\Win\cmake\bin\cmake.exe
@echo [ZEBuild Externals] CMake path : %ZE_BUILD_CMAKE%

set ZE_BUILD_SCONS=%CD%\Utils\Win\python\Scripts\scons.bat
@echo [ZEBuild Externals] SCons path : %ZE_BUILD_SCONS%

:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up externals.
del /s /q /f Output
rmdir /s /q Output

:Build
cd Source
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

:Gather
xcopy /r /y /e Source\lib*\Output\*.* Output\
goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building externals. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : Externals is build successfully.