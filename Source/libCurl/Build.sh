
ProjectName=libcurl 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.
 
# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output

echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build
$ZE_BUILD_CMAKE ../Source -DCMAKE_BUILD_TYPE:STRING="Release" -DCURL_STATICLIB:BOOL=YES -DBUILD_CURL_EXE:BOOL=NO -DBUILD_CURL_TESTS:BOOL=0
ErrorCheck

cmake --build .
ErrorCheck

Copy ../Source/include/curl ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include
Copy include/curl/* ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include/curl
Copy lib/libcurl.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release
cd ..

# Build as Debug
Remove Build
mkdir Build
cd Build
$ZE_BUILD_CMAKE ../Source -DCMAKE_BUILD_TYPE:STRING="Debug" -DCURL_STATICLIB:BOOL=YES -DBUILD_CURL_EXE:BOOL=NO -DBUILD_CURL_TESTS:BOOL=0
ErrorCheck

cmake --build .
ErrorCheck

Copy lib/libcurl.a ../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug
cd ..

Copy Output/$ZE_BUILD_PLATFORM ../../Output/

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
