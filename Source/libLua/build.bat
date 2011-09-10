:Initialization
@echo off
set ProjectName=liblua
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
cmake -D LUA_BUILD_AS_DLL:BOOL=NO -G "Visual Studio 9 2008" ../Source/Lua"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild liblua_static.vcproj "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Release\liblua_static.lib ..\Output\Lib\Win32\
move ..\Output\Lib\Win32\liblua_static.lib ..\Output\Lib\Win32\liblua.lib
xcopy /r /y ..\Source\lua\src\lua.h ..\Output\Include\Win32\
xcopy /r /y ..\Source\lua\src\lualib.h ..\Output\Include\Win32\
xcopy /r /y ..\Source\lua\src\lauxlib.h ..\Output\Include\Win32\
xcopy /r /y luaconf.h ..\Output\Include\Win32\

:Build64
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -D LUA_BUILD_AS_DLL:BOOL=NO -G "Visual Studio 9 2008 Win64" ../Source/Lua"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild liblua_static.vcproj "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Release\liblua_static.lib ..\Output\Lib\Win64\
move ..\Output\Lib\Win64\liblua_static.lib ..\Output\Lib\Win64\liblua.lib
xcopy /r /y ..\Source\lua\src\lua.h ..\Output\Include\Win64\
xcopy /r /y ..\Source\lua\src\lualib.h ..\Output\Include\Win64\
xcopy /r /y ..\Source\lua\src\lauxlib.h ..\Output\Include\Win64\
xcopy /r /y luaconf.h ..\Output\Include\Win64\
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0