@echo off
@echo [ZEBuild Externals] Info : Building externals.
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86_amd64

set ZE_BUILD_CMAKE_GENERATOR=Visual Studio 10
set ZE_BUILD_CMAKE=cmake.exe -D CMAKE_CXX_FLAGS_DEBUG:STRING="/D_DEBUG /MDd /Ob0 /Od /RTC1" - D CMAKE_C_FLAGS_DEBUG:STRING="/D_DEBUG /MDd /Ob0 /Od /RTC1"
@echo [ZEBuild Externals] CMake path : %ZE_BUILD_CMAKE%