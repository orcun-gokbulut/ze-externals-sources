
ProjectName=libAngelScript 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.


# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output
echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build
$ZE_BUILD_CMAKE ../Source/projects/cmake -DCMAKE_BUILD_TYPE:STRING="Release"
ErrorCheck

cmake --build .
ErrorCheck

Copy "../Source/include/angelscript.h" "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include"
Copy "lib/libAngelscript.a" "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release"
cd ..

# Build as Debug
Remove Build
mkdir Build
cd Build
$ZE_BUILD_CMAKE ../Source/projects/cmake -DCMAKE_BUILD_TYPE:STRING="Debug"
ErrorCheck

cmake --build .
ErrorCheck

Copy "lib/libAngelscript.a" "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug"
cd ..

Copy "Output/$ZE_BUILD_PLATFORM" "../../Output/"

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
