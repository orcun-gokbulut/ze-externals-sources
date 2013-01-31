class libATICompress(ZEPrebuildLibrary):
    pass

if (ZEPlatform.Platform == "Windows"):
    ZEBuildDriver.BuildLibrary(libATICompress("libATICompress", ""))