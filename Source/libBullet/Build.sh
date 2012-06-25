
ProjectName=libBullet 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.
 
# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output

echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
Remove Build
mkdir Build
cd Build
cmake ../Source -DCMAKE_INSTALL_PREFIX:PATH="./output" -DINSTALL_LIBS:BOOL="1" -DBUILD_MINICL_OPENCL_DEMOS:BOOL=NO -DBUILD_DEMOS:BOOL=NO -DBUILD_EXTRAS:BOOL=NO -DBUILD_CPU_DEMOS:BOOL=NO -DCMAKE_BUILD_TYPE:STRING="Release"
ErrorCheck

cmake --build . --config release --clean-first
ErrorCheck
cmake --build . --config release --target install
ErrorCheck


Copy "output/lib/*" "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Release/Bullet"
Copy "output/output/include/bullet" "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Include"
cd ..


# Debug
Remove Build
mkdir Build
cd Build
cmake ../Source -DCMAKE_INSTALL_PREFIX:PATH="./output" -DINSTALL_LIBS:BOOL="1" -DBUILD_MINICL_OPENCL_DEMOS:BOOL=NO -DBUILD_DEMOS:BOOL=NO -DBUILD_EXTRAS:BOOL=NO -DBUILD_CPU_DEMOS:BOOL=NO -DCMAKE_BUILD_TYPE:STRING="Debug"
ErrorCheck

cmake --build . --config debug --clean-first
ErrorCheck
cmake --build . --config debug --target install
ErrorCheck


Copy "output/lib/*" "../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib/Debug/Bullet"
cd ..

Copy Output/$ZE_BUILD_PLATFORM ../../Output/

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
