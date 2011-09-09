:Sub Directories
cd lib3dsMax
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e lib3dsMax\Output\*.* ..\Output\

cd libDirectX
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libDirectX\Output\*.* ..\Output\

cd libFreeImage
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libFreeImage\Output\*.* ..\Output\

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