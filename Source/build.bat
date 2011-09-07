:Sub Directories
cd libogg
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libogg\Output\*.* ..\Output\

cd libzlib
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libzlib\Output\*.* ..\Output\

goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building externals. 
exit /b 1

:End
exit /b 0
