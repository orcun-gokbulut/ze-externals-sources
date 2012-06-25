
ProjectName=libvrpn 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.
 
# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output

echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build

$ZE_BUILD_CMAKE ../Source -DCMAKE_BUILD_TYPE:STRING="Release" -DVRPN_BUILD_CLIENTS:BOOL="0" -DVRPN_BUILD_SERVER_LIBRARY:BOOL="0" -DVRPN_BUILD_SERVERS:BOOL="0"
ErrorCheck

cmake --build .
ErrorCheck


Copy vrpn_Configure.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy libvrpn.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release/vrpn
Copy quat/libquat.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release/vrpn
cd ..

# Build as Debug
Remove Build
mkdir Build
cd Build
$ZE_BUILD_CMAKE ../Source -DCMAKE_BUILD_TYPE:STRING="Debug" -DVRPN_BUILD_CLIENTS:BOOL="0" -DVRPN_BUILD_SERVER_LIBRARY:BOOL="0" -DVRPN_BUILD_SERVERS:BOOL="0"
ErrorCheck

cmake --build .
ErrorCheck

Copy libvrpn.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug/vrpn
Copy quat/libquat.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug/vrpn
cd ..

Copy Output/$ZE_BUILD_PLATFORM ../../Output/

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
