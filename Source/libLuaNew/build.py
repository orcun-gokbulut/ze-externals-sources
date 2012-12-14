class LuaLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_BUILD_TYPE", "STRING", "Debug" if Debug else "Release"),
                         ZECMakeParameter("CMAKE_INSTALL_PREFIX", "STRING", self.BuildDirectory)]
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
                
            if ZEBuild.Platform.Platform == "Windows":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                LibFileSource = self.BuildDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/liblua.lib"
                LibFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libLua.lib"
                
            elif ZEBuild.Platform.Platform == "MacOS":
                if ZEBuild.Platform.CMakeGenerator == "Xcode":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                    LibFileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + "/liblua.a"
                    LibFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libLua.a"
                    
                elif ZEBuild.Platform.CMakeGenerator == "Unix Makefiles":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                    LibFileSource = self.BuildDirectory + "/Lib/liblua.a"
                    LibFileDestination = self.OutputDirectory + "/Lib/libLua.a"
                    
            elif ZEBuild.Platform.Platform == "Linux":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                LibFileSource = self.BuildDirectory + "/Lib/liblua.a"
                LibFileDestination = self.OutputDirectory + "/Lib/libLua.a"

            ZEBuild.CopyFile(LibFileSource, LibFileDestination)
            
    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(LuaLibrary("Lua", "SampleExtraLib.lib"))
