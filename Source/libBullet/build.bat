:Initialization
@echo off
set ProjectName=libBullet
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
cmake -G "Visual Studio 9 2008" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" -DINSTALL_LIBS:BOOL="1" -DBUILD_MINICL_OPENCL_DEMOS:BOOL=NO -DBUILD_DEMOS:BOOL=NO -DBUILD_EXTRAS:BOOL=NO -DBUILD_CPU_DEMOS:BOOL=NO ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild BULLET_PHYSICS.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\*.lib ..\Output\Lib\Win32\Bullet\
xcopy /r /y /e Output\output\include\bullet ..\Output\Include\Win32\Bullet\

:Build64
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -G "Visual Studio 9 2008 Win64" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" -DINSTALL_LIBS:BOOL="1" -DBUILD_MINICL_OPENCL_DEMOS:BOOL=NO -DBUILD_DEMOS:BOOL=NO -DBUILD_EXTRAS:BOOL=NO -DBUILD_CPU_DEMOS:BOOL=NO ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild BULLET_PHYSICS.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\*.lib ..\Output\Lib\Win64\Bullet\
xcopy /r /y /e Output\output\include\bullet ..\Output\Include\Win64\Bullet\
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0