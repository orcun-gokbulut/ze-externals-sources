class libOpenAL(ZELibrary):
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

            if ZEBuild.Platform.Platform == "Windows":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Dll" + ("/Debug" if Debug else "/Release"))

                LibFileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + "/OpenAL32.lib"
                LibFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libOpenAL32.lib"
                
                DllFileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + "/OpenAL32.dll"
                DllFileDestination = self.OutputDirectory + "/Dll" + ("/Debug" if Debug else "/Release") + "/OpenAL32.dll"
                
                ZEBuild.CopyFile(LibFileSource, LibFileDestination)
                ZEBuild.CopyFile(DllFileSource, DllFileDestination)
                
            elif ZEBuild.Platform.Platform == "MacOS":
                if ZEBuild.Platform.CMakeGenerator == "Xcode":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Dll" + ("/Debug" if Debug else "/Release"))
                    FileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + "/libopenal.dylib"
                    FileDestination = self.OutputDirectory + "/Dll" + ("/Debug" if Debug else "/Release") +"/libOpenAL.dylib"
                elif ZEBuild.Platform.CMakeGenerator == "Unix Makefiles":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Dll")
                    FileSource = self.BuildDirectory + "/libopenal.1.13.0.dylib"
                    FileDestination = self.OutputDirectory + "/Dll/libOpenAL.dylib"

                ZEBuild.CopyFile(FileSource, FileDestination)

            elif ZEBuild.Platform.Platform == "Linux":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Dll")
                FileSource = self.BuildDirectory + "/libopenal.so.1.13.0"
                FileDestination = self.OutputDirectory + "/Dll/libOpenAL.so"
                    
                ZEBuild.CopyFile(FileSource, FileDestination)

ZEBuild.BuildLibrary(libOpenAL("libOpenAL", ""))
