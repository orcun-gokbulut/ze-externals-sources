class libZLib(ZELibrary):
    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        Parameters = [ZECMakeParameter("BUILD_SHARED_LIBS", "BOOL", "NO")]
        ZECMake.Configure(self, "", Parameters)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZECMake.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZECMake.Install(self, Configuration)
        ZEOperations.CopyInstallToOutput(self, Configuration, "include", "lib")

ZEBuildDriver.BuildLibrary(libZLib("libZLib", ""))