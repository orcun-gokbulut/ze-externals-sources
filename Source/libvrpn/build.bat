:Initialization
@echo off
set ProjectName=libvrpn
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Output
rmdir /s /q Output

:Build32Debug
@echo [ZEBuild Externals] Info : Building %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CMAKE_INSTALL_PREFIX:PATH="Output" -D VRPN_BUILD_CLIENTS:BOOL="0" -D VRPN_BUILD_SERVER_LIBRARY:BOOL="0" -D VRPN_BUILD_SERVERS:BOOL="0" -G "%ZE_BUILD_CMAKE_GENERATOR%" ../Source
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Output\lib\vrpn.lib ..\Output\Windows\x86\Lib\Debug\vrpn\
move ..\Output\Windows\x86\Lib\Debug\vrpn\vrpn.lib ..\Output\Windows\x86\Lib\Debug\vrpn\libvrpn.lib
xcopy /r /y Output\lib\quat.lib ..\Output\Windows\x86\Lib\Debug\vrpn\
move ..\Output\Windows\x86\Lib\Debug\vrpn\quat.lib ..\Output\Windows\x86\Lib\Debug\vrpn\libquat.lib
xcopy /r /y Output\include\*.* ..\Output\Windows\x86\Include\vrpn\
cd ..

:Build32Release
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CMAKE_INSTALL_PREFIX:PATH="Output" -D VRPN_BUILD_CLIENTS:BOOL="0" -D VRPN_BUILD_SERVER_LIBRARY:BOOL="0" -D VRPN_BUILD_SERVERS:BOOL="0" -G "%ZE_BUILD_CMAKE_GENERATOR%" ../Source
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Output\lib\vrpn.lib ..\Output\Windows\x86\Lib\Release\vrpn\
move ..\Output\Windows\x86\Lib\\Release\vrpn\vrpn.lib ..\Output\Windows\x86\Lib\Release\vrpn\libvrpn.lib
xcopy /r /y Output\lib\quat.lib ..\Output\Windows\x86\Lib\Release\vrpn\
move ..\Output\Windows\x86\Lib\Release\vrpn\quat.lib ..\Output\Windows\x86\Lib\Release\vrpn\libquat.lib
cd ..

:Build64Debug
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CMAKE_INSTALL_PREFIX:PATH="Output" -D VRPN_BUILD_CLIENTS:BOOL="0" -D VRPN_BUILD_SERVER_LIBRARY:BOOL="0" -D VRPN_BUILD_SERVERS:BOOL="0" -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ../Source
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Output\lib\vrpn.lib ..\Output\Windows\x64\Lib\Debug\vrpn\
move ..\Output\Windows\x64\Lib\Debug\vrpn\vrpn.lib ..\Output\Windows\x64\Lib\Debug\vrpn\libvrpn.lib
xcopy /r /y Output\lib\quat.lib ..\Output\Windows\x64\Lib\Debug\vrpn\
move ..\Output\Windows\x64\Lib\Debug\vrpn\quat.lib ..\Output\Windows\x64\Lib\Debug\vrpn\libquat.lib
xcopy /r /y Output\include\*.* ..\Output\Windows\x64\Include\vrpn\
cd ..

:Build64Release
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CMAKE_INSTALL_PREFIX:PATH="Output" -D VRPN_BUILD_CLIENTS:BOOL="0" -D VRPN_BUILD_SERVER_LIBRARY:BOOL="0" -D VRPN_BUILD_SERVERS:BOOL="0" -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ../Source
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Output\lib\vrpn.lib ..\Output\Windows\x64\Lib\Release\vrpn\
move ..\Output\Windows\x64\Lib\Release\vrpn\vrpn.lib ..\Output\Windows\x64\Lib\Release\vrpn\libvrpn.lib
xcopy /r /y Output\lib\quat.lib ..\Output\Windows\x64\Lib\Release\vrpn\
move ..\Output\Windows\x64\Lib\Release\vrpn\quat.lib ..\Output\Windows\x64\Lib\Release\vrpn\libquat.lib
cd ..

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1