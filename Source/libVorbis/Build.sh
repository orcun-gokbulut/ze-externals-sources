
ProjectName=libVorbis 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.


# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output
echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build
sh ../Source/configure --prefix="$PWD/output" --with-ogg-libraries="$PWD/../../libOGG/Build/output/lib" --with-ogg-includes="$PWD/../../libOGG/Build/output/Include/ogg" --disable-oggtest
ErrorCheck
make
make install

ErrorCheck

Copy output/lib/libvorbis.a "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib"
Copy output/lib/libvorbisfile.a "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib"
Copy output/include/vorbis "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include"

cd ..

Copy "Output/$ZE_BUILD_PLATFORM" "../../Output/"

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
