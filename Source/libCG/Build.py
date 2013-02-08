class libCG(ZEPrebuildLibrary):
    pass

if (ZEPlatform.Platform == "Windows" or ZEPlatform.Platform == "Linux"):
    ZEBuildDriver.BuildLibrary(libCG("libCG", ""))