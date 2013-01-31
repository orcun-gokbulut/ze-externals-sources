class libDirectX(ZEPrebuildLibrary):
    pass

if (ZEPlatform.Platform == "Windows"):
    ZEBuildDriver.BuildLibrary(libDirectX("libDirectX", ""))