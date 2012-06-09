@echo off

cd Output
pushd .

for /r %%I in (*.lib) do (
	echo Checking file %%~fI
	echo "%%~fI" | find "x86" > nul
	if Not ErrorLevel 1 (
		dumpbin /HEADERS "%%~fI" | find "8664 machine" > nul
		if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Architecture mismatch. x64 lib in x86 lib directory."
	)
	
	echo "%%~fI" | find "x64" > nul
	if Not ErrorLevel 1 (
		dumpbin /HEADERS "%%~fI" | find "14C machine"
		if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Architecture mismatch. x64 lib in x86 lib directory."
	)
	
	dumpbin /RAWDATA "%%~fI" | find "libcmt""" > nul
	if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Runtime mismatch. Library uses static multi threaded runtime (libcmt)."
	
	dumpbin /RAWDATA  "%%~fI" | find "libc " > nul
	if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Runtime mismatch. Library uses static single threaded runtime (libc)."
			
	echo "%%I" | find "Debug" > nul
	if Not ErrorLevel 1 (
		dumpbin /RAWDATA "%%~fI" | find /i"msvcrt"""
		if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Runtime configuration mismatch. Library uses release runtime library instead of debug."
	)

	echo %%I | find "Release" > nul
	if Not ErrorLevel 1 (
		dumpbin /RAWDATA "%%~fI" | find /i "msvcrtd"""
		if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Runtime configuration mismatch. Library uses debug runtime library instead of release."
	)
)
popd

for /r %%I in (*.dll) do (
	echo "%%~fI" | find "x86"
	if Not ErrorLevel 1 (
		dumpbin /HEADERS "%%~fI" | find "8664 machine"
		if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Architecture mismatch. x64 lib in x86 lib directory."
	)
	
	echo "%%~fI" | find "x64"
	if Not ErrorLevel 1 (
		dumpbin /HEADERS "%%~fI" | find "14C machine"
		if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Architecture mismatch. x64 lib in x86 lib directory."
	)
	
	dumpbin /RAWDATA "%%~fI" | find "libcmt"""
	if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Runtime mismatch. Library uses static multi threaded runtime (libcmt)."
	
	dumpbin /RAWDATA  "%%~fI" | find "libc"""
	if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Runtime mismatch. Library uses static single threaded runtime (libc)."
			
	echo "%%I" | find "Debug"
	if Not ErrorLevel 1 (
		dumpbin /RAWDATA "%%~fI" | find /i"msvcrt"""
		if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Runtime configuration mismatch. Library uses release runtime library instead of debug."
	)

	echo "%%I" | find "Release"
	if Not ErrorLevel 1 (
		dumpbin /RAWDATA "%%~fI" | find /i "msvcrtd"""
		if Not ErrorLevel 1 echo "ERROR ERROR ERROR !!! Runtime configuration mismatch. Library uses debug runtime library instead of release."
	)
)
popd