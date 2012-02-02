:Initialization
@echo off
@echo [ZEBuild Externals] Info : Building externals.
call "C:\Program Files (x86)\Microsoft Visual Studio 10\vc\vsvars32.bat"

set ZE_BUILD_CMAKE_GENERATOR=Visual Studio 10
set ZE_BUILD_CMAKE=cmake.exe -D CMAKE_CXX_FLAGS_DEBUG:STRING="/D_DEBUG /MDd /Ob0 /Od /RTC1" - D CMAKE_C_FLAGS_DEBUG:STRING="/D_DEBUG /MDd /Ob0 /Od /RTC1"
@echo [ZEBuild Externals] CMake path : %ZE_BUILD_CMAKE%

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
pause
exit /b 1

:End
@echo [ZEBuild Externals] Success : Externals is build successfully.
pause
