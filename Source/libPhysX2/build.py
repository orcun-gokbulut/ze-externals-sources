class libPhysX2(ZEPrebuildLibrary):
    pass

if (ZEPlatform.Platform == "Windows"):
    ZEBuildDriver.BuildLibrary(libPhysX2("libPhysX2", ""))