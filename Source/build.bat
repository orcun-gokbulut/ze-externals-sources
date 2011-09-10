:Sub Directories
cd lib3dsMax
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e lib3dsMax\Output\*.* ..\Output\

cd libCG
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libCG\Output\*.* ..\Output\

cd libCurl
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libCurl\Output\*.* ..\Output\

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

cd libLua
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libLua\Output\*.* ..\Output\

cd libOGG
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libogg\Output\*.* ..\Output\

cd libUnitTestCpp
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libUnitTestCpp\Output\*.* ..\Output\

cd libVorbis
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..
xcopy /r /y /e libVorbis\Output\*.* ..\Output\

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
