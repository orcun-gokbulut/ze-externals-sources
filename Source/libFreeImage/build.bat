:Initialization
@echo off
set ProjectName=libFreeImage
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
del /s /q /f Output
rmdir /s /q Output

:Build32
@echo [ZEBuild Externals] Info : Building %ProjectName%.

vcbuild /Clean Source\FreeImage.2008.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\FreeImage.2008.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win32\
move  Output\Lib\Win32\freeimage.lib Output\Lib\Win32\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.dll Output\Dll\Win32\
xcopy /r /y Source\Dist\FreeImage.h Output\Include\Win32\

:Build64
vcbuild /Clean Source\FreeImage.2008.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\FreeImage.2008.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win64\
move  Output\Lib\Win64\freeimage.lib Output\Lib\Win64\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.dll Output\Dll\Win64\
xcopy /r /y Source\Dist\FreeImage.h Output\Include\Win64\
goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0