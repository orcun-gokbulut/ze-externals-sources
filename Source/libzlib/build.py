class zLibLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_BUILD_TYPE", "STRING", "Debug" if Debug else "Release"),
                         ZECMakeParameter("BUILD_SHARED_LIBS", "BOOL", "NO")]
        else:
            Parameter = None
        ZEBuild.SetWorkingDirectory(ZEBuild.CurrentLibrary.BuildDirectory)
        ZEBuild.CMake(self.SourceDirectory, Parameter)

    def Compile(self, Debug):
        ZELibrary.Compile(self, Debug)
        ZEBuild.CMakeBuild(self.BuildDirectory, Debug)
    def Gather(self, Debug):
        ZELibrary.Gather(self, Debug)
        if (Debug != None):
            if not ZEBuild.IsDirectoryExists(self.OutputDirectory + "/Include"):
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Include")
                ZEBuild.CopyFile(self.SourceDirectory + "/zlib.h", self.OutputDirectory + "/Include" + "/zlib.h")
                ZEBuild.CopyFile(self.BuildDirectory + "/zconf.h", self.OutputDirectory + "/Include" + "/zconf.h")
            
            if ZEBuild.Platform.MultiConfiguration:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                VprnFileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + (("/zlibd.lib" if Debug else "/zlib.lib") if ZEBuild.Platform.Platform == "Windows" else "/libz.a")
                VprnFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/zlib.lib" if ZEBuild.Platform.Platform == "Windows" else "/zlib.a")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                VprnFileSource = self.BuildDirectory + "/libz.a"
                VprnFileDestination = self.OutputDirectory + "/Lib/zLib.a"
                
            ZEBuild.CopyFile(VprnFileSource, VprnFileDestination)
            
    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(zLibLibrary("zLib", "SampleExtraLib.lib"))