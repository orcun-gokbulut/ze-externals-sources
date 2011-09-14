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

vcbuild /c AngelScript.sln
vcbuild AngelScript.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Release\Angelscript.lib ..\Output\Lib\Win32\Release\
move ..\Output\Lib\Win32\Release\Angelscript.lib ..\Output\Lib\Win32\Release\libAngelscript.lib
xcopy /r /y ..\Source\include\angelscript.h ..\Output\Include\Win32\

vcbuild /c AngelScript.sln
vcbuild AngelScript.sln "Debug|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Debug\Angelscript.lib ..\Output\Lib\Win32\Debug\
move ..\Output\Lib\Win32\Debug\Angelscript.lib ..\Output\Lib\Win32\Debug\libAngelscript.lib

:Build64
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -G "Visual Studio 9 2008 Win64" ..\Source\projects\cmake
if %ERRORLEVEL% NEQ 0 GOTO Error

vcbuild /c AngelScript.sln
vcbuild AngelScript.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Release\Angelscript.lib ..\Output\Lib\Win64\Release\
move ..\Output\Lib\Win64\Release\Angelscript.lib ..\Output\Lib\Win64\Release\libAngelscript.lib
xcopy /r /y ..\Source\include\angelscript.h ..\Output\Include\Win64\

vcbuild /c AngelScript.sln
vcbuild AngelScript.sln "Debug|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Debug\Angelscript.lib ..\Output\Lib\Win64\Debug\
move ..\Output\Lib\Win64\Debug\Angelscript.lib ..\Output\Lib\Win64\Debug\libAngelscript.lib

goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0