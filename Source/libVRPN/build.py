class libVRPN(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        Parameters = [ZECMakeParameter("BUILD_TESTING", "BOOL", "NO"),
                      ZECMakeParameter("VRPN_BUILD_JAVA", "BOOL", "NO"),
                      ZECMakeParameter("VRPN_BUILD_PYTHON", "BOOL", "NO"),
                      ZECMakeParameter("VRPN_BUILD_CLIENT_LIBRARY", "BOOL", "YES"),
                      ZECMakeParameter("VRPN_BUILD_CLIENTS", "BOOL", "NO"),
                      ZECMakeParameter("VRPN_BUILD_SERVER_LIBRARY", "BOOL", "NO"),
                      ZECMakeParameter("VRPN_BUILD_SERVERS", "BOOL", "NO")]
        ZECMake.Configure(self, "", Parameters)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZECMake.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        ZECMake.Install(self, Configuration)
        ZEOperations.CopyInstallToOutput(self, Configuration, "include", "lib")

ZEBuildDriver.BuildLibrary(libVRPN("libVRPN", ""))