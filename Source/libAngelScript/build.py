class libAngelScript(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        ZEBuild.CMake(self, "projects/cmake", None)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZEBuild.CMakeBuild(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZEBuild.CMakeInstall(self, Configuration)
        ZEBuild.CopyInstallToOutputAuto(self, Configuration)


ZEBuild.BuildLibrary(libAngelScript("libAngelScript", ""))