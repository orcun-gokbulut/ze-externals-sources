:Sub Directories
cd libogg
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libzlib
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building externals. 
exit /b 1

:End
exit /b 0
