:Initialization
@echo off
@echo [ZEBuild Externals] Info : Building externals.
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" 

:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up externals.
del /s /q /f Output
rmdir /s /q Output

:Build
cd Source
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

:Gather
xcopy /r /y /e Source\lib*\Output\*.* Output\
goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building externals. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : Externals is build successfully.
