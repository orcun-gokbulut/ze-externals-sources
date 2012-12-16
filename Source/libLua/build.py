class libLua(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        ZECMake.Configure(self, "", None)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZECMake.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZEOperations.CopyInstallToOutputAuto(self, Configuration)

ZEBuildDriver.BuildLibrary(libLua("libLua", ""))
