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
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Windows\x86\Lib\Debug\
move ..\Output\Windows\x86\Lib\Debug\OpenAL32.lib ..\Output\Windows\x86\Lib\Debug\libOpenAL.lib
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\Windows\x86\Dll\Debug\
xcopy /r /y /e Output\include\AL ..\Output\Windows\x86\Include\AL\
cd ..

:Build32
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
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Windows\x86\Lib\Release\
move ..\Output\Windows\x86\Lib\Release\OpenAL32.lib ..\Output\Windows\x86\Lib\Release\libOpenAL.lib
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\Windows\x86\Dll\Release\
xcopy /r /y /e Output\include\AL ..\Output\Windows\x86\Include\AL\
cd ..

:Build64Debug
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
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Windows\x64\Lib\Debug\
move ..\Output\Windows\x64\Lib\Debug\OpenAL32.lib ..\Output\Windows\x64\Lib\Debug\libOpenAL.lib
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\Windows\x64\Dll\Debug\
move ..\Output\Windows\x64\Dll\Debug\OpenAL32.dll ..\Output\Windows\x64\Dll\Debug\OpenAL64.dll
xcopy /r /y /e Output\include\AL ..\Output\Windows\x64\Include\AL\
cd ..

:Build64Release
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
xcopy /r /y /e Output\lib\OpenAL32.lib ..\Output\Windows\x64\Lib\Release\
move ..\Output\Windows\x64\Lib\Release\OpenAL32.lib ..\Output\Windows\x64\Lib\Release\libOpenAL.lib
xcopy /r /y /e Output\bin\OpenAL32.dll ..\Output\Windows\x64\Dll\Release\
move ..\Output\Windows\x64\Dll\Release\OpenAL32.dll ..\Output\Windows\x64\Dll\Release\OpenAL64.dll
cd ..

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1