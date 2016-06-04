:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Copyright (C) 2015 The Qt Company Ltd.
:: Contact: http://www.qt.io/licensing/
::
:: This file is part of the tools applications of the Qt Toolkit.
::
:: $QT_BEGIN_LICENSE:LGPL21$
:: Commercial License Usage
:: Licensees holding valid commercial Qt licenses may use this file in
:: accordance with the commercial license agreement provided with the
:: Software or, alternatively, in accordance with the terms contained in
:: a written agreement between you and The Qt Company. For licensing terms
:: and conditions see http://www.qt.io/terms-conditions. For further
:: information use the contact form at http://www.qt.io/contact-us.
::
:: GNU Lesser General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU Lesser
:: General Public License version 2.1 or version 3 as published by the Free
:: Software Foundation and appearing in the file LICENSE.LGPLv21 and
:: LICENSE.LGPLv3 included in the packaging of this file. Please review the
:: following information to ensure the GNU Lesser General Public License
:: requirements will be met: https://www.gnu.org/licenses/lgpl.html and
:: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
::
:: As a special exception, The Qt Company gives you certain additional
:: rights. These rights are described in The Qt Company LGPL Exception
:: version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
::
:: $QT_END_LICENSE$
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@echo off
set QTSRC=%~dp0
set QTDIR=%CD%

if not exist %QTSRC%.gitignore goto sconf
echo Please wait while bootstrapping configure ...

for %%C in (cl.exe icl.exe g++.exe perl.exe jom.exe) do set %%C=%%~$PATH:C

if "%perl.exe%" == "" (
    echo Perl not found in PATH. Aborting. >&2
    exit /b 1
)
if not exist mkspecs (
    md mkspecs
    if errorlevel 1 goto exit
)

rem Extract Qt's version from .qmake.conf
for /f "eol=# tokens=1,2,3,4 delims=.= " %%i in (%QTSRC%.qmake.conf) do (
    if %%i == MODULE_VERSION (
        set QTVERMAJ=%%j
        set QTVERMIN=%%k
        set QTVERPAT=%%l
    )
)
set QTVERSION=%QTVERMAJ%.%QTVERMIN%.%QTVERPAT%

perl %QTSRC%bin\syncqt.pl -minimal -version %QTVERSION% -module QtCore -outdir "%QTDIR%" %QTSRC%
if errorlevel 1 goto exit

if not exist tools\configure (
    md tools\configure
    if errorlevel 1 goto exit
)
cd tools\configure
if errorlevel 1 goto exit

set make=nmake
if not "%jom.exe%" == "" set make=jom

echo #### Generated by configure.bat - DO NOT EDIT! ####> Makefile
echo/>> Makefile
echo QTVERSION = %QTVERSION%>> Makefile
rem These must have trailing spaces to avoid misinterpretation as 5>>, etc.
echo QT_VERSION_MAJOR = %QTVERMAJ% >> Makefile
echo QT_VERSION_MINOR = %QTVERMIN% >> Makefile
echo QT_VERSION_PATCH = %QTVERPAT% >> Makefile
if not "%icl.exe%" == "" (
    echo CXX = icl>>Makefile
    echo EXTRA_CXXFLAGS = /Zc:forScope>>Makefile
    rem This must have a trailing space.
    echo QTSRC = %QTSRC% >> Makefile
    set tmpl=win32
) else if not "%cl.exe%" == "" (
    echo CXX = cl>>Makefile
    echo EXTRA_CXXFLAGS =>>Makefile
    rem This must have a trailing space.
    echo QTSRC = %QTSRC% >> Makefile
    set tmpl=win32
) else if not "%g++.exe%" == "" (
    echo CXX = g++>>Makefile
    echo EXTRA_CXXFLAGS =>>Makefile
    rem This must NOT have a trailing space.
    echo QTSRC = %QTSRC:\=/%>> Makefile
    set tmpl=mingw
    set make=mingw32-make
) else (
    echo No suitable compiler found in PATH. Aborting. >&2
    cd ..\..
    exit /b 1
)
echo/>> Makefile
type %QTSRC%tools\configure\Makefile.%tmpl% >> Makefile

%make%
if errorlevel 1 (cd ..\.. & exit /b 1)

cd ..\..

:conf
configure.exe -srcdir %QTSRC% %*
goto exit

:sconf
%QTSRC%configure.exe %*
:exit
