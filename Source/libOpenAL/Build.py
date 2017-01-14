class libOpenAL(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        Parameters = [ZECMakeParameter("CURL_STATICLIB", "BOOL", "YES"),
            ZECMakeParameter("ALSOFT_CONFIG", "BOOL", "NO"),
            ZECMakeParameter("ALSOFT_EXAMPLES", "BOOL", "NO"),
            ZECMakeParameter("ALSOFT_TESTS", "BOOL", "NO"),
            ZECMakeParameter("ALSOFT_UTILS", "BOOL", "NO")]
            
        ZECMake.Configure(self, "", Parameters)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZECMake.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZECMake.Install(self, Configuration)
        ZEOperations.CopyInstallToOutput(self, Configuration, "include", "lib", "bin")

ZEBuildDriver.BuildLibrary(libOpenAL("libOpenAL", ""))
