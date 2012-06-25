
ProjectName=libzlib 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.
 
# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output

echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build

$ZE_BUILD_CMAKE ../Source -DBUILD_SHARED_LIBS:BOOL=NO -DCMAKE_BUILD_TYPE:STRING="Release"
ErrorCheck

cmake --build .
ErrorCheck


Copy ../Source/zlib.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy ./zconf.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy ./libz.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release
mv libz.a libzlib.a
cd ..

# Build as Debug
Remove Build
mkdir Build
cd Build
$ZE_BUILD_CMAKE ../Source -DBUILD_SHARED_LIBS:BOOL=NO -DCMAKE_BUILD_TYPE:STRING="Debug"
ErrorCheck

cmake --build .
ErrorCheck

Copy ../Source/zlib.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy ./zconf.h ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy ./libz.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug
mv libz.a libzlib.a
cd ..

Copy Output/$ZE_BUILD_PLATFORM ../../Output/

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
