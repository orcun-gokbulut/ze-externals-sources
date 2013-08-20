class libISense(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        ZECMake.Configure(self, "", None)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZECMake.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZECMake.Install(self, Configuration)
        ZEOperations.CopyInstallToOutputAuto(self, Configuration)
        
        if (Configuration == ZEBuild.CONFIG_DEBUG):
            ConfigurationString = "/Debug"
        elif (Configuration == ZEBuild.CONFIG_RELEASE):
            ConfigurationString = "/Release"
        else:
            ConfigurationString = ""
           
        ZEOperations.CopyDirectory(self.RootDirectory + "/Prebuild/" + ZEPlatform.PlatformStringNoToolset + "/DLL", self.OutputDirectory + "/DLL" + ConfigurationString)

if (ZEPlatform.Platform == "Windows" or ZEPlatform.Platform == "Linux" or ZEPlatform.Platform == "MacOSX"):
    ZEBuildDriver.BuildLibrary(libISense("libISense", ""))
