:Initialization
@echo off
set ProjectName=liblua
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
%ZE_BUILD_CMAKE% -D LUA_BUILD_AS_DLL:BOOL=NO -G "%ZE_BUILD_CMAKE_GENERATOR%" ../Source/Lua"
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Debug\liblua_static.lib ..\Output\Lib\Win32\Debug\
move ..\Output\Lib\Win32\Debug\liblua_static.lib ..\Output\Lib\Win32\Debug\liblua.lib
xcopy /r /y ..\Source\lua\src\lua.h ..\Output\Include\Win32\
xcopy /r /y ..\Source\lua\src\lualib.h ..\Output\Include\Win32\
xcopy /r /y ..\Source\lua\src\lauxlib.h ..\Output\Include\Win32\
xcopy /r /y luaconf.h ..\Output\Include\Win32\
cd ..

:Build32Release
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D LUA_BUILD_AS_DLL:BOOL=NO -G "%ZE_BUILD_CMAKE_GENERATOR%" ../Source/Lua"
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Release\liblua_static.lib ..\Output\Lib\Win32\Release\
move ..\Output\Lib\Win32\Release\liblua_static.lib ..\Output\Lib\Win32\Release\liblua.lib
cd ..

:Build64Debug
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D LUA_BUILD_AS_DLL:BOOL=NO -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ../Source/Lua"
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Debug\liblua_static.lib ..\Output\Lib\Win64\Debug\
move ..\Output\Lib\Win64\Debug\liblua_static.lib ..\Output\Lib\Win64\Debug\liblua.lib
xcopy /r /y ..\Source\lua\src\lua.h ..\Output\Include\Win64\
xcopy /r /y ..\Source\lua\src\lualib.h ..\Output\Include\Win64\
xcopy /r /y ..\Source\lua\src\lauxlib.h ..\Output\Include\Win64\
xcopy /r /y luaconf.h ..\Output\Include\Win64\
cd ..

:Build64Release
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D LUA_BUILD_AS_DLL:BOOL=NO -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" ../Source/Lua"
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Release\liblua_static.lib ..\Output\Lib\Win64\Release\
move ..\Output\Lib\Win64\Release\liblua_static.lib ..\Output\Lib\Win64\Release\liblua.lib
cd ..

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1