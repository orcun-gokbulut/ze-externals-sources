
ProjectName=libogg

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

Copy output/lib/libogg.a "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib"
Copy output/include/ogg/ogg.h "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include/ogg"
Copy output/include/ogg/os_types.h "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include/ogg"

cd ..

Copy "Output/$ZE_BUILD_PLATFORM" "../../Output/"

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
