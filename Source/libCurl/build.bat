:Initialization
@echo off
set ProjectName=libcurl
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
cmake -D CURL_STATICLIB:BOOL=YES -D CMAKE_INSTALL_PREFIX:STRING=./Output32 -D BUILD_CURL_EXE:BOOL=NO -G "Visual Studio 9 2008" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild CURL.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /s Output32\include\curl\.svn
xcopy /r /y Output32\lib\*.* ..\Output\Lib\Win32\
xcopy /r /y Output32\include\curl\*.* ..\Output\Include\Win32\curl\

:Build64
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
cmake -D CURL_STATICLIB:BOOL=YES -D BUILD_CURL_EXE:BOOL=NO -D CMAKE_INSTALL_PREFIX:STRING=./Output64 -G "Visual Studio 9 2008 Win64" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild CURL.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /s Output64\include\curl\.svn
xcopy /r /y Output64\lib\*.* ..\Output\Lib\Win64\
xcopy /r /y Output64\include\curl\*.* ..\Output\Include\Win64\curl\
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0