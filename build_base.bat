:Initialization
@echo off
@echo [ZEBuild Externals] Info : Building externals.
call build_initialize.bat

:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up externals.
del /s /q /f Output
rmdir /s /q Output

:Build
cd Source
call build_source.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

:End
@echo [ZEBuild Externals] Success : Externals is build successfully.
pause
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building externals. 
pause
exit /b 1