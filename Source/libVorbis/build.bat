:Initialization
@echo off
set ProjectName=libVorbis
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
del /s /q /f Output
rmdir /s /q Output
del /q /f Source\zconf.h

:Build32Debug
@echo [ZEBuild Externals] Info : Building %ProjectName%.
msbuild /t:clean Source\win32\VS2010\vorbis_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\vorbis_static.sln /p:configuration=debug /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\Win32\Debug\libvorbis_static.lib Output\Lib\Win32\Debug\
move  Output\Lib\Win32\Debug\libvorbis_static.lib Output\Lib\Win32\Debug\libvorbis.lib
xcopy /r /y Source\win32\VS2010\Win32\Debug\libvorbisfile_static.lib Output\Lib\Win32\Debug\
move  Output\Lib\Win32\Debug\libvorbisfile_static.lib Output\Lib\Win32\Debug\libvorbisfile.lib
xcopy /r /y Source\include\vorbis\codec.h Output\Include\Win32\vorbis\
xcopy /r /y Source\include\vorbis\vorbisenc.h Output\Include\Win32\vorbis\
xcopy /r /y Source\include\vorbis\vorbisfile.h Output\Include\Win32\vorbis\

:Build32Release
msbuild /t:clean Source\win32\VS2010\vorbis_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\vorbis_static.sln /p:configuration=release /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\Win32\Release\libvorbis_static.lib Output\Lib\Win32\Release\
move  Output\Lib\Win32\Release\libvorbis_static.lib Output\Lib\Win32\Release\libvorbis.lib
xcopy /r /y Source\win32\VS2010\Win32\Release\libvorbisfile_static.lib Output\Lib\Win32\Release\
move  Output\Lib\Win32\Release\libvorbisfile_static.lib Output\Lib\Win32\Release\libvorbisfile.lib

:Build64Debug
msbuild /t:clean Source\win32\VS2010\vorbis_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\vorbis_static.sln /p:configuration=debug /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\x64\Debug\libvorbis_static.lib Output\Lib\Win64\Debug\
move  Output\Lib\Win64\Debug\libvorbis_static.lib Output\Lib\Win64\Debug\libvorbis.lib
xcopy /r /y Source\win32\VS2010\x64\Debug\libvorbisfile_static.lib Output\Lib\Win64\Debug\
move  Output\Lib\Win64\Debug\libvorbisfile_static.lib Output\Lib\Win64\Debug\libvorbisfile.lib
xcopy /r /y Source\include\vorbis\codec.h Output\Include\Win64\vorbis\
xcopy /r /y Source\include\vorbis\vorbisenc.h Output\Include\Win64\vorbis\
xcopy /r /y Source\include\vorbis\vorbisfile.h Output\Include\Win64\vorbis\

:Build64Release
msbuild /t:clean Source\win32\VS2010\vorbis_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\vorbis_static.sln /p:configuration=release /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\x64\Release\libvorbis_static.lib Output\Lib\Win64\Release\
move  Output\Lib\Win64\Release\libvorbis_static.lib Output\Lib\Win64\Release\libvorbis.lib
xcopy /r /y Source\win32\VS2010\x64\Release\libvorbisfile_static.lib Output\Lib\Win64\Release\
move  Output\Lib\Win64\Release\libvorbisfile_static.lib Output\Lib\Win64\Release\libvorbisfile.lib
goto End

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0