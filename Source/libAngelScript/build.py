class libAngelScript(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        ZEOperations.Configure(self, "projects/cmake", None)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZEOperations.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZECMake.Install(self, Configuration)
        ZEOperations.CopyInstallToOutputAuto(self, Configuration)


ZEBuildDriver.BuildLibrary(libAngelScript("libAngelScript", ""))