class AngleScriptLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)
        print("Clean***")

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        ZEBuild.CMake("c:\\fsd", None)
        print("Configure***")

    def Compile(self, Debug):
        ZELibrary.Compile(self, Debug)
        print("Compile***")

    def Gather(self, Debug):
        ZELibrary.Gather(self, Debug)
        print("Gather***")

ZEBuild.BuildLibrary(AngleScriptLibrary("AngelScript", "Test.lib"))