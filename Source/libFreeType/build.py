class libFreeType(ZELibrary):
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

            if ZEBuild.Platform.Platform == "Windows":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                FileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + "/libfreetype.lib"
                FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libFreeType.lib"
                
            elif ZEBuild.Platform.Platform == "MacOS":
                if ZEBuild.Platform.CMakeGenerator == "Xcode":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                    FileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + "/libfreetype.a"
                    FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libFreeType.a"
                    
                else:
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                    FileSource = self.BuildDirectory + "/libfreetype.a"
                    FileDestination = self.OutputDirectory + "/Lib/libFreeType.a"
                    
            elif ZEBuild.Platform.Platform == "Linux":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                FileSource = self.BuildDirectory + "/libfreetype.a"
                FileDestination = self.OutputDirectory + "/Lib/libFreeType.a"

            ZEBuild.CopyFile(FileSource, FileDestination)

ZEBuild.BuildLibrary(libFreeType("libFreeType", ""))
