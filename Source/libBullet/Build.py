class libBullet(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        Parameter = [ZECMakeParameter("INSTALL_LIBS", "BOOL", "1"),
                     ZECMakeParameter("BUILD_MINICL_OPENCL_DEMOS", "BOOL", "NO"),
                     ZECMakeParameter("BUILD_DEMOS", "BOOL", "NO"),
                     ZECMakeParameter("BUILD_EXTRAS", "BOOL", "NO"),
                     ZECMakeParameter("BUILD_CPU_DEMOS", "BOOL", "NO")]

        ZECMake.Configure(self, "", Parameter)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZECMake.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZECMake.Install(self, Configuration)
        ZEOperations.CopyInstallToOutput(self, Configuration, "include/bullet", "lib")

ZEBuildDriver.BuildLibrary(libBullet("libBullet", ""))