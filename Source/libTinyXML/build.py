class libTinyXML(ZELibrary):
    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        Parameter = ""
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
                ZEBuild.CopyFile(self.SourceDirectory + "/tinyxml.h", self.OutputDirectory + "/Include/tinyxml.h")
                
            if ZEBuild.Platform.Platform == "Windows":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                FileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + "/libtinyxml.lib"
                FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libTinyXml.lib"
                
            elif ZEBuild.Platform.Platform == "MacOS":
                if ZEBuild.Platform.CMakeGenerator == "Xcode":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                    FileSource = self.BuildDirectory + "/lib" +("/Debug" if Debug else "/Release") + "/libtinyxml.a"
                    FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libTinyXml.a"
                    
                elif ZEBuild.Platform.CMakeGenerator == "Unix Makefiles":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                    FileSource = self.BuildDirectory + "/lib/libtinyxml.a"
                    FileDestination = self.OutputDirectory + "/Lib/libTinyXml.a"
                    
            elif ZEBuild.Platform.Platform == "Linux":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                FileSource = self.BuildDirectory + "/lib/libtinyxml.a"
                FileDestination = self.OutputDirectory + "/Lib/libTinyXml.a"

            ZEBuild.CopyFile(FileSource, FileDestination)

ZEBuild.BuildLibrary(libTinyXML("libTinyXML", ""))
