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
xcopy /r /y Output\lib\vrpn.lib ..\Output\Lib\Win32\Debug\vrpn\
move ..\Output\Lib\Win32\Debug\vrpn\vrpn.lib ..\Output\Lib\Win32\Debug\vrpn\libvrpn.lib
xcopy /r /y Output\lib\quat.lib ..\Output\Lib\Win32\Debug\vrpn\
move ..\Output\Lib\Win32\Debug\vrpn\quat.lib ..\Output\Lib\Win32\Debug\vrpn\libquat.lib
xcopy /r /y Output\include\*.* ..\Output\Include\Win32\vrpn\

:Build32Release
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CMAKE_INSTALL_PREFIX:PATH="Output" -D VRPN_BUILD_CLIENTS:BOOL="0" -D VRPN_BUILD_SERVER_LIBRARY:BOOL="0" -D VRPN_BUILD_SERVERS:BOOL="0" -G "%ZE_BUILD_CMAKE_GENERATOR%" ../Source
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Output\lib\vrpn.lib ..\Output\Lib\Win32\Release\vrpn\
move ..\Output\Lib\Win32\Release\vrpn\vrpn.lib ..\Output\Lib\Win32\Release\vrpn\libvrpn.lib
xcopy /r /y Output\lib\quat.lib ..\Output\Lib\Win32\Release\vrpn\
move ..\Output\Lib\Win32\Release\vrpn\quat.lib ..\Output\Lib\Win32\Release\vrpn\libquat.lib

:Build64Debug
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CMAKE_INSTALL_PREFIX:PATH="Output" -D VRPN_BUILD_CLIENTS:BOOL="0" -D VRPN_BUILD_SERVER_LIBRARY:BOOL="0" -D VRPN_BUILD_SERVERS:BOOL="0" -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ../Source
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Output\lib\vrpn.lib ..\Output\Lib\Win64\Debug\vrpn\
move ..\Output\Lib\Win64\Debug\vrpn\vrpn.lib ..\Output\Lib\Win64\Debug\vrpn\libvrpn.lib
xcopy /r /y Output\lib\quat.lib ..\Output\Lib\Win64\Debug\vrpn\
move ..\Output\Lib\Win64\Debug\vrpn\quat.lib ..\Output\Lib\Win64\Debug\vrpn\libquat.lib
xcopy /r /y Output\include\*.* ..\Output\Include\Win64\vrpn\

:Build64Release
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CMAKE_INSTALL_PREFIX:PATH="Output" -D VRPN_BUILD_CLIENTS:BOOL="0" -D VRPN_BUILD_SERVER_LIBRARY:BOOL="0" -D VRPN_BUILD_SERVERS:BOOL="0" -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ../Source
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Output\lib\vrpn.lib ..\Output\Lib\Win64\Release\vrpn\
move ..\Output\Lib\Win64\Release\vrpn\vrpn.lib ..\Output\Lib\Win64\Release\vrpn\libvrpn.lib
xcopy /r /y Output\lib\quat.lib ..\Output\Lib\Win64\Release\vrpn\
move ..\Output\Lib\Win64\Release\vrpn\quat.lib ..\Output\Lib\Win64\Release\vrpn\libquat.lib

goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0