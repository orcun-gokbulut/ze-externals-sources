
ProjectName=libOpenAL 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.
 
# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output

echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build

$ZE_BUILD_CMAKE ../Source -DCMAKE_BUILD_TYPE:STRING="Release"
ErrorCheck

cmake --build .
ErrorCheck


Copy ../Source/include/AL ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy libopenal.so ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Dll/Release
cd ..

# Build as Debug
Remove Build
mkdir Build
cd Build
$ZE_BUILD_CMAKE ../Source -DCMAKE_BUILD_TYPE:STRING="Debug"
ErrorCheck

cmake --build .
ErrorCheck

Copy libopenal.so ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Dll/Debug
cd ..

Copy Output/$ZE_BUILD_PLATFORM ../../Output/

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
