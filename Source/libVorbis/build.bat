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
xcopy /r /y Source\win32\VS2010\Win32\Debug\libvorbis.lib Output\Windows\x86\Lib\Debug\
xcopy /r /y Source\win32\VS2010\Win32\Debug\libvorbisfile_static.lib Output\Windows\x86\Lib\Debug\
move  Output\Windows\x86\Lib\Debug\libvorbisfile_static.lib Output\Windows\x86\Lib\Debug\libvorbisfile.lib
xcopy /r /y Source\include\vorbis\codec.h Output\Windows\x86\Include\vorbis\
xcopy /r /y Source\include\vorbis\vorbisenc.h Output\Windows\x86\Include\vorbis\
xcopy /r /y Source\include\vorbis\vorbisfile.h Output\Windows\x86\Include\vorbis\

:Build32Release
msbuild /t:clean Source\win32\VS2010\vorbis_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\vorbis_static.sln /p:configuration=release /p:platform=win32
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\Win32\Release\libvorbis.lib Output\Windows\x86\Lib\Release\
xcopy /r /y Source\win32\VS2010\Win32\Release\libvorbisfile_static.lib Output\Windows\x86\Lib\Release\
move  Output\Windows\x86\Lib\Release\libvorbisfile_static.lib Output\Windows\x86\Lib\Release\libvorbisfile.lib

:Build64Debug
msbuild /t:clean Source\win32\VS2010\vorbis_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\vorbis_static.sln /p:configuration=debug /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\x64\Debug\libvorbis.lib Output\Windows\x64\Lib\Debug\
xcopy /r /y Source\win32\VS2010\x64\Debug\libvorbisfile_static.lib Output\Windows\x64\Lib\Debug\
move  Output\Windows\x64\Lib\Debug\libvorbisfile_static.lib Output\Windows\x64\Lib\Debug\libvorbisfile.lib
xcopy /r /y Source\include\vorbis\codec.h Output\Windows\x64\Include\vorbis\
xcopy /r /y Source\include\vorbis\vorbisenc.h Output\Windows\x64\Include\vorbis\
xcopy /r /y Source\include\vorbis\vorbisfile.h Output\Windows\x64\Include\vorbis\

:Build64Release
msbuild /t:clean Source\win32\VS2010\vorbis_static.sln
if %ERRORLEVEL% NEQ 0 GOTO Error
msbuild Source\win32\VS2010\vorbis_static.sln /p:configuration=release /p:platform=x64
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2010\x64\Release\libvorbis.lib Output\Windows\x64\Lib\Release\
xcopy /r /y Source\win32\VS2010\x64\Release\libvorbisfile_static.lib Output\Windows\x64\Lib\Release\
move  Output\Windows\x64\Lib\Release\libvorbisfile_static.lib Output\Windows\x64\Lib\Release\libvorbisfile.lib

:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1