:Initialization
@echo off
set ProjectName=libzlib
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Output
rmdir /s /q Output
del /q /f Source\zconf.h

:Build32Debug
@echo [ZEBuild Externals] Info : Building %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D BUILD_SHARED_LIBS:BOOL=NO -G "%ZE_BUILD_CMAKE_GENERATOR%" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Debug\zlibd.lib ..\Output\Windows\x86\Lib\Debug\
move ..\Output\Windows\x86\Lib\Debug\zlibd.lib ..\Output\Windows\x86\Lib\Debug\libzlib.lib
xcopy /r /y ..\Source\zlib.h ..\Output\Windows\x86\Include\
xcopy /r /y .\zconf.h ..\Output\Windows\x86\Include\
cd ..

:Build32Release
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D BUILD_SHARED_LIBS:BOOL=NO -G "%ZE_BUILD_CMAKE_GENERATOR%" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Release\zlib.lib ..\Output\Windows\x86\Lib\Release\
move ..\Output\Windows\x86\Lib\Release\zlib.lib ..\Output\Windows\x86\Lib\Release\libzlib.lib
cd ..

:Build64Debug
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D BUILD_SHARED_LIBS:BOOL=NO -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Debug\zlibd.lib ..\Output\Windows\x64\Lib\Debug\
move ..\Output\Windows\x64\Lib\Debug\zlibd.lib ..\Output\Windows\x64\Lib\Debug\libzlib.lib
xcopy /r /y ..\Source\zlib.h ..\Output\Windows\x64\Include\
xcopy /r /y .\zconf.h ..\Output\Windows\x64\Include\
cd ..

:Build64Release
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D BUILD_SHARED_LIBS:BOOL=NO -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Release\zlib.lib ..\Output\Windows\x64\Lib\Release\
move ..\Output\Windows\x64\Lib\Release\zlib.lib ..\Output\Windows\x64\Lib\Release\libzlib.lib
cd ..

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1