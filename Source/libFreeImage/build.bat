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

:Build
@echo [ZEBuild Externals] Info : Building %ProjectName%.

vcbuild /Clean Source\FreeImage.2008.sln
if %ERRORLEVEL% NEQ 0 GOTO Error

vcbuild Source\FreeImage.2008.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win32\
move  Output\Lib\Win32\freeimage.lib Output\Lib\Win32\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.dll Output\Dll\Win32\

vcbuild /Clean Source\FreeImage.2008.sln
if %ERRORLEVEL% NEQ 0 GOTO Error

vcbuild Source\FreeImage.2008.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimaged.lib Output\Lib\Win64\
move  Output\Lib\Win64\freeimage.lib Output\Lib\Win64\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.dll Output\Dll\Win64\

:CopyOutput
@echo [ZEBuild Externals] Info : Copying output of %ProjectName%.
xcopy /r /y Source\Dist\FreeImage.h Output\Include\
goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0