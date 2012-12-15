class libFreeImage(ZELibrary):
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
        
    def Gather(self, Debug):
        ZELibrary.Gather(self, Debug)
        ZEBuild.CMakeInstall(self.BuildDirectory, Debug)
        if (Debug != None):
            if not ZEBuild.IsDirectoryExists(self.OutputDirectory + "/Include"):
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Include")
                ZEBuild.CopyFile(self.SourceDirectory + "/FreeImage.h", self.OutputDirectory + "/Include/FreeImage.h")

            if ZEBuild.Platform.MultiConfiguration:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                FileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + ("/libFreeImage.lib" if ZEBuild.Platform.Platform == "Windows" else "/liblibFreeImage.a")
                FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libFreeImage.lib" if ZEBuild.Platform.Platform == "Windows" else "/libFreeImage.a")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                FileSource = self.BuildDirectory + "/liblibFreeImage.a"
                FileDestination = self.OutputDirectory + "/Lib/libFreeImage.a"

            ZEBuild.CopyFile(FileSource, FileDestination)

ZEBuild.BuildLibrary(libFreeImage("libFreeImage", ""))