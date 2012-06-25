
ProjectName=libmpg123

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.


# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output
echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build
sh ../Source/configure --prefix="$PWD/output"
ErrorCheck
make
make install

ErrorCheck

Copy output/lib/libmpg123.so "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Dll"
Copy ../Source/src/libmpg123/mpg123.h.in "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include"
Copy output/include/mpg123.h "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include"

cd ..

Copy "Output/$ZE_BUILD_PLATFORM" "../../Output/"

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
