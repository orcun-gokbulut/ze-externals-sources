:Initialization
@echo off
set ProjectName=libcurl
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Output
rmdir /s /q Output

:Build32Debug
@echo [ZEBuild Externals] Info : Building %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CURL_STATICLIB:BOOL=YES -D CMAKE_INSTALL_PREFIX:STRING=./Output -D BUILD_CURL_EXE:BOOL=NO -G "Visual Studio 9 2008" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild /c CURL.sln 
vcbuild CURL.sln "Debug|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /s Output\include\curl\.svn
xcopy /r /y /e Output\lib ..\Output\Lib\Win32\Debug\
xcopy /r /y /e Output\include\curl ..\Output\Include\Win32\curl\

:Build32Release
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CURL_STATICLIB:BOOL=YES -D CMAKE_INSTALL_PREFIX:STRING=./Output -D BUILD_CURL_EXE:BOOL=NO -G "Visual Studio 9 2008" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild /c CURL.sln 
vcbuild CURL.sln "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /s Output\include\curl\.svn
xcopy /r /y /e Output\lib ..\Output\Lib\Win32\Release\

:Build64Debug
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CURL_STATICLIB:BOOL=YES -D BUILD_CURL_EXE:BOOL=NO -D CMAKE_INSTALL_PREFIX:STRING=./Output -G "Visual Studio 9 2008 Win64" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild /c CURL.sln 
vcbuild CURL.sln "Debug|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /s Output\include\curl\.svn
xcopy /r /y /e Output\lib ..\Output\Lib\Win64\Debug\
xcopy /r /y /e Output\include\curl ..\Output\Include\Win64\curl\

:Build64Release
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -D CURL_STATICLIB:BOOL=YES -D BUILD_CURL_EXE:BOOL=NO -D CMAKE_INSTALL_PREFIX:STRING=./Output -G "Visual Studio 9 2008 Win64" ../Source"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild /c CURL.sln 
vcbuild CURL.sln "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild INSTALL.vcproj
if %ERRORLEVEL% NEQ 0 GOTO Error
del /q /s Output\include\curl\.svn
xcopy /r /y /e Output\lib ..\Output\Lib\Win64\Release\
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0