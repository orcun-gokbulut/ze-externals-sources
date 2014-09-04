class libClang(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)

        Parameters = [ZECMakeParameter("LLVM_INCLUDE_EXAMPLES", "BOOL", "0"),
                      ZECMakeParameter("LLVM_INCLUDE_TESTS", "BOOL", "0"),
                      ZECMakeParameter("LLVM_BUILD_TOOLS", "BOOL", "0"),
                      ZECMakeParameter("LLVM_BUILD_DOCS", "BOOL", "1"),
                      ZECMakeParameter("LLVM_INCLUDE_UTILS", "BOOL", "1")]

        ZECMake.Configure(self, "", Parameters)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZECMake.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZECMake.Install(self, Configuration)
        ZEOperations.CopyInstallToOutput(self, Configuration, "include", "lib")


class libClangPrebuild(ZEPrebuildLibrary):
    pass

ZEBuildDriver.BuildLibrary(libClang("libClang", ""))
