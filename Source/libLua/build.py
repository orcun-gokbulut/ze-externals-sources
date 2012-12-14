class LuaLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_BUILD_TYPE", "STRING", "Debug" if Debug else "Release"),
                         ZECMakeParameter("LUA_BUILD_AS_DLL", "BOOL", "NO")]
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
                ZEBuild.CopyFile(self.SourceDirectory + "/lua/src/lua.h", self.OutputDirectory + "/Include/lua.h")
                ZEBuild.CopyFile(self.SourceDirectory + "/lua/src/lualib.h", self.OutputDirectory + "/Include/lualib.h")
                ZEBuild.CopyFile(self.SourceDirectory + "/lua/src/lauxlib.h", self.OutputDirectory + "/Include/lauxlib.h")
                
            if ZEBuild.Platform.Platform == "Windows":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                LibFileSource = self.BuildDirectory + "/lua/build/" +("/Debug" if Debug else "/Release") + "/libliblua_static.lib"
                LibFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libTinyXml.lib"
                
                DllFileSource = self.BuildDirectory + "/lua/build/" +("/Debug" if Debug else "/Release") + "/libliblua_static.lib"
                DllFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libTinyXml.lib"
                
            elif ZEBuild.Platform.Platform == "MacOS":
                if ZEBuild.Platform.CMakeGenerator == "Xcode":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                    LibFileSource = self.BuildDirectory + "/lua/build/" + ("/Debug" if Debug else "/Release") + "/libliblua_static.a"
                    LibFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libLua.a"
                    
                    DllFileSource = self.BuildDirectory + "/lua/build/" + ("/Debug" if Debug else "/Release") + "/liblua.dylib"
                    DllFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libLua.dylib"
                    
                elif ZEBuild.Platform.CMakeGenerator == "Unix Makefiles":
                    ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                    LibFileSource = self.BuildDirectory + "/lua/build/libliblua_static.a"
                    LibFileDestination = self.OutputDirectory + "/Lib/libLua.a"
                    
                    DllFileSource = self.BuildDirectory + "/lua/build/liblua.dylib"
                    DllFileDestination = self.OutputDirectory + "/Lib/libLua.dylib"
                    
            elif ZEBuild.Platform.Platform == "Linux":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                LibFileSource = self.BuildDirectory + "/lua/build/libliblua_static.a"
                LibFileDestination = self.OutputDirectory + "/Lib/libLua.a"
                
                DllFileSource = self.BuildDirectory + "/lua/build/liblua.dylib"
                DllFileDestination = self.OutputDirectory + "/Lib/libLua.dylib"

            ZEBuild.CopyFile(LibFileSource, LibFileDestination)
            ZEBuild.CopyFile(DllFileSource, DllFileDestination)
            
    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(LuaLibrary("Lua", "SampleExtraLib.lib"))
