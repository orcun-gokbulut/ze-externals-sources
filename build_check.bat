@echo off

cd Output
pushd .

for /r %%I in (*.lib) do (
	echo "%%~fI" | find "x86"
	if Not ErrorLevel 1 (
		dumpbin /HEADERS "%%~fI" | find "8664 machine"
		if Not ErrorLevel 1 echo Error: Architecture mismatch. x64 lib in x86 lib directory. File Name : "%%~fI"
	)
	
	echo "%%~fI" | find "x64"
	if Not ErrorLevel 1 (
		dumpbin /HEADERS "%%~fI" | find "14C machine"
		if Not ErrorLevel 1 echo Error: Architecture mismatch. x64 lib in x86 lib directory. File Name : "%%~fI"
	)
	
	dumpbin /RAWDATA %%~fI | find "libcmt"""
	if Not ErrorLevel 1 echo "Error: Runtime mismatch. Library uses static multi threaded runtime (libcmt). File Name : "%%I"
	
	dumpbin /RAWDATA  %%~fI | find "libc "
	if Not ErrorLevel 1 echo "Error: Runtime mismatch. Library uses static single threaded runtime (libc). File Name : "%%I"
			
	echo %%I | find "Debug"
	if Not ErrorLevel 1 (
		dumpbin /RAWDATA %%~fI | find /i"msvcrt"""
		if Not ErrorLevel 1 echo Error: Runtime configuration mismatch. Library uses release runtime library instead of debug. File Name : "%%I"
	)

	echo %%I | find "Release"
	if Not ErrorLevel 1 (
		dumpbin /RAWDATA %%~fI | find /i "msvcrtd"""
		if Not ErrorLevel 1 echo Error: Runtime configuration mismatch. Library uses debug runtime library instead of release. File Name : "%%I"
	)
)
popd

for /r %%I in (*.dll) do (
	echo "%%~fI" | find "x86"
	if Not ErrorLevel 1 (
		dumpbin /HEADERS "%%~fI" | find "8664 machine"
		if Not ErrorLevel 1 echo Error: Architecture mismatch. x64 lib in x86 lib directory. File Name : "%%~fI"
	)
	
	echo "%%~fI" | find "x64"
	if Not ErrorLevel 1 (
		dumpbin /HEADERS "%%~fI" | find "14C machine"
		if Not ErrorLevel 1 echo Error: Architecture mismatch. x64 lib in x86 lib directory. File Name : "%%~fI"
	)
	
	dumpbin /RAWDATA %%~fI | find "libcmt"""
	if Not ErrorLevel 1 echo "Error: Runtime mismatch. Library uses static multi threaded runtime (libcmt). File Name : "%%I"
	
	dumpbin /RAWDATA  %%~fI | find "libc"""
	if Not ErrorLevel 1 echo "Error: Runtime mismatch. Library uses static single threaded runtime (libc). File Name : "%%I"
			
	echo %%I | find "Debug"
	if Not ErrorLevel 1 (
		dumpbin /RAWDATA %%~fI | find /i"msvcrt"""
		if Not ErrorLevel 1 echo Error: Runtime configuration mismatch. Library uses release runtime library instead of debug. File Name : "%%I"
	)

	echo %%I | find "Release"
	if Not ErrorLevel 1 (
		dumpbin /RAWDATA %%~fI | find /i "msvcrtd"""
		if Not ErrorLevel 1 echo Error: Runtime configuration mismatch. Library uses debug runtime library instead of release. File Name : "%%I"
	)
)
popd