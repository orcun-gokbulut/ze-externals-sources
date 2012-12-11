class AngleScriptLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_BUILD_TYPE", "STRING", "Debug" if Debug else "Release")]
        else:
            Parameter = None
        ZEBuild.SetWorkingDirectory(ZEBuild.CurrentLibrary.BuildDirectory)
        ZEBuild.CMake(self.SourceDirectory + "/projects/cmake", Parameter)

    def Compile(self, Debug):
        ZELibrary.Compile(self, Debug)
        ZEBuild.CMakeBuild(self.BuildDirectory, Debug)
            
    def Gather(self, Debug):
        ZELibrary.Gather(self, Debug)
        if (Debug != None):
            
            if Debug:
                ZEBuild.CopyDirectory(self.SourceDirectory + "/include", self.OutputDirectory + "/Include")
            
            ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))
            
            FileSource = self.SourceDirectory + "/lib" + ("/Debug" if Debug else "/Release") + "/Angelscript.lib"
            FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + "/libAngelscript.lib"
            
            ZEBuild.CopyFile(FileSource, FileDestination)
            
    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(AngleScriptLibrary("AngelScript", "SampleExtraLib.lib"))