:Initialization
@echo off
set ProjectName=libOpenAL
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
%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR%" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Lib\Win32\Debug\
move ..\Output\Lib\Win32\Debug\OpenAL32.lib ..\Output\Lib\Win32\Debug\libOpenAL.lib
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\DLL\Win32\Debug\
xcopy /r /y /e Output\include\AL ..\Output\Include\Win32\AL\

:Build32
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR%" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Lib\Win32\Release\
move ..\Output\Lib\Win32\Release\OpenAL32.lib ..\Output\Lib\Win32\Release\libOpenAL.lib
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\DLL\Win32\Release\
xcopy /r /y /e Output\include\AL ..\Output\Include\Win32\AL\


:Build64Debug
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config debug --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Lib\Win64\Debug\
move ..\Output\Lib\Win64\Debug\OpenAL32.lib ..\Output\Lib\Win64\Debug\libOpenAL.lib
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\DLL\Win64\Debug\
move ..\Output\DLL\Win64\Debug\OpenAL32.dll ..\Output\DLL\Win64\Debug\OpenAL64.dll
xcopy /r /y /e Output\include\AL ..\Output\Include\Win64\AL\

:Build64Release
cd ..
del /s /q /f Build
rmdir /s /q Build
mkdir Build
cd Build
%ZE_BUILD_CMAKE% -G "%ZE_BUILD_CMAKE_GENERATOR% Win64" -DCMAKE_INSTALL_PREFIX:PATH=".\Output" ..\Source
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --clean-first
if %ERRORLEVEL% NEQ 0 GOTO Error
cmake --build . --config release --target install
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Lib\Win64\Release\
move ..\Output\Lib\Win64\Release\OpenAL32.lib ..\Output\Lib\Win64\Release\libOpenAL.lib
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\DLL\Win64\Release\
move ..\Output\DLL\Win64\Release\OpenAL32.dll ..\Output\DLL\Win64\Release\OpenAL64.dll
goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
cd ..
exit /b 0