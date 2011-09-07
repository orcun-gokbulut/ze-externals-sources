:Initialization
@echo off
@echo [ZEBuild Externals] Info : Building externals.

:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up externals.
del /s /q /f Output
rmdir /s /q Output
del /q /f Source\zconf.h

:Build
cd Source
call build.bat
cd ..

:Gather
xcopy /r /y /e Source\lib*\Output\*.* Output\

:End
@echo [ZEBuild Externals] Success : Externals is build successfully.
