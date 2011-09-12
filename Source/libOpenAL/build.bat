:Initialization
@echo off
set ProjectName=libOpenAL
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Output
rmdir /s /q Output

:Build32
@echo [ZEBuild Externals] Info : Building %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -G "Visual Studio 9 2008" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild OpenAL.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Lib\Win32\
move ..\Output\Lib\Win32\OpenAL32.lib ..\Output\Lib\Win32\libOpenAL.lib
xcopy /r /y /e Output\include\AL ..\Output\Include\Win32\AL\
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\DLL\Win32\

:Build64
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -G "Visual Studio 9 2008 Win64" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild OpenAL.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Lib\Win64\
move ..\Output\Lib\Win64\OpenAL32.lib ..\Output\Lib\Win64\libOpenAL.lib
xcopy /r /y /e Output\include\AL ..\Output\Include\Win64\AL\
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\DLL\Win64\
move ..\Output\DLL\Win64\OpenAL32.dll ..\Output\DLL\Win64\OpenAL64.dll
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0