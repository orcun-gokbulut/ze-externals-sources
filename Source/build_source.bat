:Sub Directories
cd lib3dsMax
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libAngelScript
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libATICompress
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libBullet
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libCG
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libCurl
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libDirectX
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libFreeImage
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libmpg123
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libLua
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libOGG
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libOpenAL
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libPhysX2
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libPhysX3
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libQT
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libVorbis
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libvrpn
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

cd libzlib
call build.bat
if %ERRORLEVEL% NEQ 0 GOTO Error
cd ..

:End
exit /b 0

:Error
exit /b 1
