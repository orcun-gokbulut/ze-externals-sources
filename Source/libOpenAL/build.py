class libOpenAL(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        ZECMake.Configure(self, "", None)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZECMake.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZECMake.Install(self, Configuration)
        ZEOperations.CopyInstallToOutput(self, Configuration, "include", "lib", "bin")

ZEBuildDriver.BuildLibrary(libOpenAL("libOpenAL", ""))
