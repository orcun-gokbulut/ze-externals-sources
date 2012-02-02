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

:Build32Release
cd ..
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

:Build64Debug
cd ..
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

:Build64Release
cd ..
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
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0