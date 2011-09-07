:Initialization
@echo off
set ProjectName=libzlib
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Output
rmdir /s /q Output
del /q /f Source\zconf.h

:Build32
@echo [ZEBuild Externals] Info : Building %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -D BUILD_SHARED_LIBS:BOOL=NO  -G "Visual Studio 9 2008" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild zlib.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Release\zlib.lib Output\Lib\Win32\
move Output\Lib\Win32\zlib.lib Output\Lib\Win32\libzlib.lib

:Build64
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -D BUILD_SHARED_LIBS:BOOL=NO  -G "Visual Studio 9 2008 Win64" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild zlib.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Release\zlib.lib Output\Lib\Win64\
move Output\Lib\Win64\zlib.lib Output\Lib\Win64\libzlib.lib

:CopyOutput
@echo [ZEBuild Externals] Info : Copying output of %ProjectName%.
cd ..
xcopy /r /y Source\zlib.h Output\Include\
xcopy /r /y Build\zconf.h Output\Include\
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0