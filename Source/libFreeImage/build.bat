:Initialization
@echo off
set ProjectName=libFreeImage
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Output
rmdir /s /q Output

:Build32Debug
@echo [ZEBuild Externals] Info : Building %ProjectName%.
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\Source\FreeImageLib\FreeImageLib.2008.vcproj "Debug|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimaged.lib Output\Lib\Win32\Debug\
move  Output\Lib\Win32\Debug\freeimaged.lib Output\Lib\Win32\Debug\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.h Output\Include\Win32\

:Build32Release
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\Source\FreeImageLib\FreeImageLib.2008.vcproj "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win32\Release\
move  Output\Lib\Win32\Release\freeimage.lib Output\Lib\Win32\Release\libfreeimage.lib

:Build64Debug
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\Source\FreeImageLib\FreeImageLib.2008.vcproj "Debug|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimaged.lib Output\Lib\Win64\Debug\
move  Output\Lib\Win64\Debug\freeimaged.lib Output\Lib\Win64\Debug\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.h Output\Include\Win64\

:Build64Release
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
vcbuild Source\Source\FreeImageLib\FreeImageLib.2008.vcproj "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win64\Release\
move  Output\Lib\Win64\Release\freeimage.lib Output\Lib\Win64\Release\libfreeimage.lib

goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0