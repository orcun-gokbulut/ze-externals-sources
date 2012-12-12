class OpenALLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_BUILD_TYPE", "STRING", "Debug" if Debug else "Release")]
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
                ZEBuild.CopyDirectory(self.SourceDirectory + "/include/AL", self.OutputDirectory + "/Include")

            if ZEBuild.Platform.MultiConfiguration:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Dll" + ("/Debug" if Debug else "/Release"))

                LibFileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + ("/OpenAL32.lib" if ZEBuild.Platform.Platform == "Windows" else "/OpenAL32.a")
                LibFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libOpenAL32.lib" if ZEBuild.Platform.Platform == "Windows" else "/libOpenAL32.a")
                
                DllFileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + ("/OpenAL32.dll" if ZEBuild.Platform.Platform == "Windows" else "/OpenAL32.so")
                DllFileDestination = self.OutputDirectory + "/Dll" + ("/Debug" if Debug else "/Release") + ("/OpenAL32.dll" if ZEBuild.Platform.Platform == "Windows" else "/OpenAL32.so")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Dll")

                LibFileSource = self.BuildDirectory + "/lib/libOpenAL32.a"
                LibFileDestination = self.OutputDirectory + "/Lib/libOpenAL32.a"

            ZEBuild.CopyFile(LibFileSource, LibFileDestination)
            ZEBuild.CopyFile(DllFileSource, DllFileDestination)

    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(OpenALLibrary("OpenAL", "SampleExtraLib.lib"))