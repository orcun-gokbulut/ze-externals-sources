class FreeImageLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_INSTALL_PREFIX", "PATH", self.BuildDirectory if Debug else "Release")]
        else:
            Parameter = None
        ZEBuild.SetWorkingDirectory(ZEBuild.CurrentLibrary.BuildDirectory)
        ZEBuild.CMake(self.SourceDirectory, Parameter)

    def Compile(self, Debug):
        ZELibrary.Compile(self, Debug)
        ZEBuild.CMakeBuild(self.BuildDirectory, Debug)
        ZEBuild.CMakeInstall(self.BuildDirectory, Debug)
        
    def Gather(self, Debug):
        ZELibrary.Gather(self, Debug)
        if (Debug != None):

            if not ZEBuild.IsDirectoryExists(self.OutputDirectory + "/Include"):
                ZEBuild.CopyDirectory(self.BuildDirectory + "/Include", self.OutputDirectory + "/Include")

            if ZEBuild.Platform.MultiConfiguration:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                FileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + ("/libFreeImage.lib" if ZEBuild.Platform.Platform == "Windows" else "/liblibFreeImage.a")
                FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libFreeImage.lib" if ZEBuild.Platform.Platform == "Windows" else "/liblibFreeImage.a")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                FileSource = self.BuildDirectory + "liblibFreeImage.a"
                FileDestination = self.OutputDirectory + "/Lib/liblibFreeImage.a"

            ZEBuild.CopyFile(FileSource, FileDestination)

    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(FreeImageLibrary("FreeImage", "SampleExtraLib.lib"))