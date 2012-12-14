class FreeTypeLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("FT_CONFIG_MODULES_H", "STRING", "<ftmodule.h>")]
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
                ZEBuild.CopyDirectory(self.SourceDirectory + "/include", self.OutputDirectory + "/Include")

            if ZEBuild.Platform.Platform == "Windows" or ZEBuild.Platform.Platform == "MacOS":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                FileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + ("/libfreetype.lib" if ZEBuild.Platform.Platform == "Windows" else "/libfreetype.a")
                FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libFreeType.lib" if ZEBuild.Platform.Platform == "Windows" else "/libFreeType.a")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                FileSource = self.BuildDirectory + "/libfreetype.a"
                FileDestination = self.OutputDirectory + "/Lib/libFreeType.a"

            ZEBuild.CopyFile(FileSource, FileDestination)

    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(FreeTypeLibrary("FreeType", "SampleExtraLib.lib"))
