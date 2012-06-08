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
xcopy /r /y Source\Dist\freeimaged.lib Output\Windows\x86\Lib\Debug\
move  Output\Windows\x86\Lib\Debug\freeimaged.lib Output\Windows\x86\Lib\Debug\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.h Output\Windows\x86\Include\
xcopy /r /y Source\Dist\freeimaged.dll Output\Windows\x86\Dll\Debug\

:Build32Release
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
msbuild Source\FreeImage.2008.sln /p:configuration=release /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Windows\x86\Lib\Release\
move  Output\Windows\x86\Lib\Release\freeimage.lib Output\Windows\x86\Lib\Release\libfreeimage.lib
xcopy /r /y Source\Dist\freeimage.dll Output\Windows\x86\Dll\Release\

:Build64Debug
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
msbuild Source\FreeImage.2008.sln  /p:configuration=debug /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimaged.lib Output\Windows\x64\Lib\Debug\
move  Output\Windows\x64\Lib\Debug\freeimaged.lib Output\Windows\x64\Lib\Debug\libfreeimage.lib
xcopy /r /y Source\Dist\FreeImage.h Output\Windows\x64\Include\
xcopy /r /y Source\Dist\freeimaged.dll Output\Windows\x64\Dll\Debug\

:Build64Release
cd Source
call Clean.bat
cd..
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /f Source\Dist
msbuild Source\FreeImage.2008.sln /p:configuration=release /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\Dist\freeimage.lib Output\Windows\x64\Lib\Release\
move  Output\Windows\x64\Lib\Release\freeimage.lib Output\Windows\x64\Lib\Release\libfreeimage.lib
xcopy /r /y Source\Dist\freeimage.dll Output\Windows\x64\Dll\Release\

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1