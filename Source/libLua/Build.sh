
ProjectName=liblua 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.
 
# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output

echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build

$ZE_BUILD_CMAKE ../Source/lua -DCMAKE_BUILD_TYPE:STRING="Release" -DLUA_BUILD_AS_DLL:BOOL=NO
ErrorCheck

cmake --build .
ErrorCheck


Copy ../Source/lua/src/lua.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy ../Source/lua/src/lualib.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy ../Source/lua/src/lauxlib.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy luaconf.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include

Copy libliblua_static.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release
mv ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release/libliblua_static.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release/liblua.a

cd ..

# Build as Debug
Remove Build
mkdir Build
cd Build
$ZE_BUILD_CMAKE ../Source/lua -DCMAKE_BUILD_TYPE:STRING="Debug" -DLUA_BUILD_AS_DLL:BOOL=NO
ErrorCheck

cmake --build .
ErrorCheck

Copy libliblua_static.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug
mv ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug/libliblua_static.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug/liblua.a
cd ..

Copy Output/$ZE_BUILD_PLATFORM ../../Output/

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
