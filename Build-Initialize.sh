echo [ZEBuild Externals] Info : Building externals.
export ZE_BUILD_PLATFORM="Linux"
export ZE_BUILD_ARCH="x86"
export ZE_BUILD_CMAKE_GENERATOR="Unix Makefiles"
export ZE_BUILD_CMAKE="cmake"

ErrorCheck()
{
	if [ $? != 0 ]; then 
		echo [ZEBuild Externals] Error : Error occured while building $ProjectName.
		exit 1
	fi
}

# Argument 1: Source
# Argument 2: Destination
Copy()
{
	test ! -d $2 && mkdir -p $2
	cp -r $1 $2
}

# Argument 1: Target
Remove()
{
	test -e $1 && rm -rf $1
}
export -f Copy
export -f Remove
export -f ErrorCheck

echo [ZEBuild Externals] CMake path : $ZE_BUILD_CMAKE
