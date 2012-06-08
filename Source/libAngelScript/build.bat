:Initialization
@echo off
set ProjectName=libAngelScript
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

%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR%" ..\Source\projects\cmake
if %ERRORLEVEL% NEQ 0 GOTO Error

cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Release\Angelscript.lib ..\Output\Windows\x86\Lib\Release\
move ..\Output\Windows\Lib\x86\Release\Angelscript.lib ..\Output\Windows\x86\Lib\Release\libAngelscript.lib
xcopy /r /y ..\Source\include\angelscript.h ..\Output\Windows\x86\Include\

cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Debug\Angelscript.lib ..\Output\Windows\x86\Lib\Debug\
move ..\Output\Windows\Lib\x86\Debug\Angelscript.lib ..\Output\Windows\x86\Lib\Debug\libAngelscript.lib
cd ..

:Build64
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build

%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ..\Source\projects\cmake
if %ERRORLEVEL% NEQ 0 GOTO Error

cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Release\Angelscript.lib ..\Output\Windows\x64\Lib\Release\
move ..\Output\Windows\Lib\x64\Release\Angelscript.lib ..\Output\Windows\x64\Lib\Release\libAngelscript.lib
xcopy /r /y ..\Source\include\angelscript.h ..\Output\Windows\x64\Include\

cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y lib\Debug\Angelscript.lib ..\Output\Windows\x64\Lib\Debug\
move ..\Output\Windows\Lib\x64\Debug\Angelscript.lib ..\Output\Windows\x64\Lib\Debug\libAngelscript.lib
cd ..

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1
