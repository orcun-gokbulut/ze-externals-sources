class BulletLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_INSTALL_PREFIX", "PATH", "./output"),
                         ZECMakeParameter("INSTALL_LIBS", "BOOL", "1"),
                         ZECMakeParameter("BUILD_MINICL_OPENCL_DEMOS", "BOOL", "NO"),
                         ZECMakeParameter("BUILD_DEMOS", "BOOL", "NO"),
                         ZECMakeParameter("BUILD_EXTRAS", "BOOL", "NO"),
                         ZECMakeParameter("BUILD_CPU_DEMOS", "BOOL", "NO"),
                         ZECMakeParameter("CMAKE_BUILD_TYPE", "STRING", "Debug" if Debug else "Release"),]
        else:
            Parameter = None
        ZEBuild.SetWorkingDirectory(ZEBuild.CurrentLibrary.BuildDirectory)
        ZEBuild.CMake(self.SourceDirectory, Parameter)

    def Compile(self, Debug):
        ZELibrary.Compile(self, Debug)
        ZEBuild.CMakeBuild(self.BuildDirectory, Debug)

    def Gather(self, Debug):
        ZEBuild.CMakeInstall(self.BuildDirectory, Debug)
        ZELibrary.Gather(self, Debug)
        if (Debug != None):
            if not ZEBuild.IsDirectoryExists(self.OutputDirectory + "/Include"):
                ZEBuild.CopyDirectory(self.BuildDirectory + "/output/output/include/bullet/", self.OutputDirectory + "/Include")

            if ZEBuild.Platform.MultiConfiguration:
                ZEBuild.CopyDirectory(self.BuildDirectory + "/output/lib", self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))
            else:
                ZEBuild.CopyDirectory(self.BuildDirectory + "/output/lib", self.OutputDirectory + "/Lib")

    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(BulletLibrary("Bullet", "SampleExtraLib.lib"))