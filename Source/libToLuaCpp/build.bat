:Initialization
@echo off
set ProjectName=libtuluacpp
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
del /s /q /f Output
rmdir /s /q Output

:Build32
@echo [ZEBuild Externals] Info : Building %ProjectName%.
del /s /q /f Source\Lua
rmdir /s /q Source\Lua
xcopy /r /y /e ..\libLua\Output\Lib Source\Lua\Lib\
xcopy /r /y /e ..\libLua\Output\Include Source\Lua\Include\
vcbuild /Clean Source\Source\win32\vc7\toluapp.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\win32\vc7\toluapp.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
rem xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win32\
rem move  Output\Lib\Win32\freeimage.lib Output\Lib\Win32\libfreeimage.lib
rem xcopy /r /y Source\Dist\FreeImage.dll Output\Dll\Win32\
rem xcopy /r /y Source\Dist\FreeImage.h Output\Include\Win32\

:Build64
xcopy /r /y /e ..\libLua\Output\Lib\Win64\liblua.lib Source\Lib\
xcopy /r /y /e ..\libLua\Output\Include\Win64\Include Source\Include\
vcbuild /Clean Source\Source\win32\vc7\toluapp.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\win32\vc7\toluapp.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win64\
move  Output\Lib\Win64\freeimage.lib Output\Lib\Win64\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.dll Output\Dll\Win64\
xcopy /r /y Source\Dist\FreeImage.h Output\Include\Win64\

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1
