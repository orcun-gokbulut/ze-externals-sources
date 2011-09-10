:Initialization
@echo off
set ProjectName=libBullet
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
cmake -G "Visual Studio 9 2008" ..\Source\projects\cmake
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild AngelScript.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Release\Angelscript.lib ..\Output\Lib\Win32\
move ..\Output\Lib\Win32\Angelscript.lib ..\Output\Lib\Win32\libAngelscript.lib
xcopy /r /y ..\Source\include\angelscript.h ..\Output\Include\Win32\

:Build64
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -G "Visual Studio 9 2008 Win64" ..\Source\projects\cmake
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild AngelScript.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Release\Angelscript.lib ..\Output\Lib\Win64\
move ..\Output\Lib\Win64\Angelscript.lib ..\Output\Lib\Win64\libAngelscript.lib
xcopy /r /y ..\Source\include\angelscript.h ..\Output\Include\Win64\

goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0