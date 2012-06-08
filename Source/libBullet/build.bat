:Initialization
@echo off
set ProjectName=libBullet
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Output
rmdir /s /q Output

:Build32Release
@echo [ZEBuild Externals] Info : Building %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR%" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" -DINSTALL_LIBS:BOOL="1" -DBUILD_MINICL_OPENCL_DEMOS:BOOL=NO -DBUILD_DEMOS:BOOL=NO -DBUILD_EXTRAS:BOOL=NO -DBUILD_CPU_DEMOS:BOOL=NO ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\*.lib ..\Output\Windows\x86\Lib\Release\Bullet\
xcopy /r /y /e Output\output\include\bullet ..\Output\Windows\x86\Include\Bullet\
cd ..

:Build32Debug
@echo [ZEBuild Externals] Info : Building %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR%" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" -DINSTALL_LIBS:BOOL="1" -DBUILD_MINICL_OPENCL_DEMOS:BOOL=NO -DBUILD_DEMOS:BOOL=NO -DBUILD_EXTRAS:BOOL=NO -DBUILD_CPU_DEMOS:BOOL=NO ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\*.lib ..\Output\Windows\x86\Lib\Debug\Bullet\
cd ..

:Build64Release
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" -DINSTALL_LIBS:BOOL="1" -DBUILD_MINICL_OPENCL_DEMOS:BOOL=NO -DBUILD_DEMOS:BOOL=NO -DBUILD_EXTRAS:BOOL=NO -DBUILD_CPU_DEMOS:BOOL=NO ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\*.lib ..\Output\Windows\x64\Lib\Release\Bullet\
xcopy /r /y /e Output\output\include\bullet ..\Output\Windows\x64\Include\Bullet\
cd ..

:Build64Debug
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" -DINSTALL_LIBS:BOOL="1" -DBUILD_MINICL_OPENCL_DEMOS:BOOL=NO -DBUILD_DEMOS:BOOL=NO -DBUILD_EXTRAS:BOOL=NO -DBUILD_CPU_DEMOS:BOOL=NO ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\*.lib ..\Output\Windows\x64\Lib\Debug\Bullet\
cd ..

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1