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
msbuild Source\FreeImage.2008.sln /p:configuration=debug /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimaged.lib Output\Lib\Win32\Debug\
move  Output\Lib\Win32\Debug\freeimaged.lib Output\Lib\Win32\Debug\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.h Output\Include\Win32\
xcopy /r /y Source\Dist\freeimaged.dll Output\Dll\Win32\Debug\

:Build32Release
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
msbuild Source\FreeImage.2008.sln /p:configuration=release /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win32\Release\
move  Output\Lib\Win32\Release\freeimage.lib Output\Lib\Win32\Release\libfreeimage.lib
xcopy /r /y Source\Dist\freeimage.dll Output\Dll\Win32\Release\

:Build64Debug
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
msbuild Source\FreeImage.2008.sln  /p:configuration=debug /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimaged.lib Output\Lib\Win64\Debug\
move  Output\Lib\Win64\Debug\freeimaged.lib Output\Lib\Win64\Debug\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.h Output\Include\Win64\
xcopy /r /y Source\Dist\freeimaged.dll Output\Dll\Win64\Debug\

:Build64Release
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
msbuild Source\FreeImage.2008.sln /p:configuration=release /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Lib\Win64\Release\
move  Output\Lib\Win64\Release\freeimage.lib Output\Lib\Win64\Release\libfreeimage.lib
xcopy /r /y Source\Dist\freeimage.dll Output\Dll\Win64\Release\

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1