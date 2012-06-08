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
xcopy /r /y Debug\liblua_static.lib ..\Output\Windows\x86\Lib\Debug\
move ..\Output\Windows\x86\Lib\Debug\liblua_static.lib ..\Output\Windows\x86\Lib\Debug\liblua.lib
xcopy /r /y ..\Source\lua\src\lua.h ..\Output\Windows\x86\Include\
xcopy /r /y ..\Source\lua\src\lualib.h ..\Output\Windows\x86\Include\
xcopy /r /y ..\Source\lua\src\lauxlib.h ..\Output\Windows\x86\Include\
xcopy /r /y luaconf.h ..\Output\Windows\x86\Include\
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
xcopy /r /y Release\liblua_static.lib ..\Output\Windows\x86\Lib\Release\
move ..\Output\Windows\x86\Lib\Release\liblua_static.lib ..\Output\Windows\x86\Lib\Release\liblua.lib
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
xcopy /r /y Debug\liblua_static.lib ..\Output\Windows\x64\Lib\Debug\
move ..\Output\Windows\x64\Lib\Debug\liblua_static.lib ..\Output\Windows\x64\Lib\Debug\liblua.lib
xcopy /r /y ..\Source\lua\src\lua.h ..\Output\Windows\x64\Include\
xcopy /r /y ..\Source\lua\src\lualib.h ..\Output\Windows\x64\Include\
xcopy /r /y ..\Source\lua\src\lauxlib.h ..\Output\Windows\x64\Include\
xcopy /r /y luaconf.h ..\Output\Windows\x64\Include\
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
xcopy /r /y Release\liblua_static.lib ..\Output\Windows\x64\Lib\Release\
move ..\Output\Windows\x64\Lib\Release\liblua_static.lib ..\Output\Windows\x64\Lib\Release\liblua.lib
cd ..

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1