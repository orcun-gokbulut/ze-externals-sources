
ProjectName=libFreeImage 

echo [ZEBuild Externals] Info : Starting to build external $ProjectName.
 
# Clean Up
echo [ZEBuild Externals] Info : Cleaning up $ProjectName.
Remove Output

echo [ZEBuild Externals] Info : Building $ProjectName.

# Release
cd Source
make clean
#sh clean.sh
ErrorCheck
make
ErrorCheck

cd Dist
Copy libfreeimage.a ../../Output/$ZE_BUILD_PLATFORM/$ZE_BUILD_ARCH/Lib
Copy FreeImage.h ../../Output/Linux/x86/Include
Copy *.so ../../Output/Linux/x86/Dll

cd ../..

Copy Output/$ZE_BUILD_PLATFORM ../../Output/

echo [ZEBuild Externals] Success : $ProjectName build external is completed successfully.
