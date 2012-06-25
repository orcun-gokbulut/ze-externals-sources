
echo [ZEBuild Externals] Info : Building externals.
source Build-Initialize.sh

echo [ZEBuild Externals] Info : Cleaning up externals.
Remove Output

cd Source
source Build-Source.sh
ErrorCheck
cd ..

echo [ZEBuild Externals] Success : Externals is build successfully.
exit 0
