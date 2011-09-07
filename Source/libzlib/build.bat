:Initialization
@echo off
set ProjectName=libzlib
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
mkdir Build
cd Build
cmake -D BUILD_SHARED_LIBS:BOOL=NO ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild zlib.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error

:CopyOutput
@echo [ZEBuild Externals] Info : Copying output of %ProjectName%.
cd ..
xcopy /r /y Build\Release\zlib.lib \Output\Lib\Win32\libzlib.lib
xcopy /r /y Source\zlib.h \Output\Include
xcopy /r /y Build\zconf.h \Output\Include
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0