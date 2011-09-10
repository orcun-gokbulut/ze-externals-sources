:Initialization
@echo off
set ProjectName=libogg
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CleanUp
@echo [ZEBuild Externals] Info : Cleaning up %ProjectName%.
del /s /q /f Build
rmdir /s /q Build
del /s /q /f Output
rmdir /s /q Output
del /q /f Source\zconf.h

:Build32
@echo [ZEBuild Externals] Info : Building %ProjectName%.
vcbuild /Clean Source\win32\VS2008\libvorbis\libvorbis_static.vcproj 
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\win32\VS2008\libvorbis\libvorbis_static.vcproj "Release|Win32"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2008\Win32\Release\libvorbis_static.lib Output\Lib\Win32\
move  Output\Lib\Win32\libvorbis_static.lib Output\Lib\Win32\libvorbis.lib
xcopy /r /y Source\win32\VS2008\Win32\Release\libvorbisfile_static.lib Output\Lib\Win32\
move  Output\Lib\Win32\libvorbisfile_static.lib Output\Lib\Win32\libvorbisfile.lib
xcopy /r /y Source\include\vorbis\codec.h Output\Include\Win32\vorbis\
xcopy /r /y Source\include\vorbis\vorbisenc.h Output\Include\Win32\vorbis\
xcopy /r /y Source\include\vorbis\vorbisfile.h Output\Include\Win32\vorbis\

:Build64
@echo [ZEBuild Externals] Info : Copying output of %ProjectName%.
vcbuild /Clean Source\win32\VS2008\libvorbis\libvorbis_static.vcproj 
if %ERRORLEVEL% NEQ 0 GOTO Error
vcbuild Source\win32\VS2008\libvorbis\libvorbis_static.vcproj "Release|x64"
if %ERRORLEVEL% NEQ 0 GOTO Error
xcopy /r /y Source\win32\VS2008\x64\Release\libvorbis_static.lib Output\Lib\Win64\
move  Output\Lib\Win64\libvorbis_static.lib Output\Lib\Win64\libvorbis.lib
xcopy /r /y Source\win32\VS2008\x64\Release\libvorbisfile_static.lib Output\Lib\Win64\
move  Output\Lib\Win64\libvorbisfile_static.lib Output\Lib\Win64\libvorbisfile.lib
xcopy /r /y Source\include\vorbis\codec.h Output\Include\Win64\vorbis\
xcopy /r /y Source\include\vorbis\vorbisenc.h Output\Include\Win64\vorbis\
xcopy /r /y Source\include\vorbis\vorbisfile.h Output\Include\Win64\vorbis\

goto End

:Error
cd ..
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0