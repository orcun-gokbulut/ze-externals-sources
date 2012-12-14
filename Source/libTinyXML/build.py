class TinyXMLLibrary(ZELibrary):
    def Clean(self):
        ZELibrary.Clean(self)

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
                
            if ZEBuild.Platform.Platform == "Windows" or ZEBuild.Platform.Platform == "MacOS":
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                FileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + ("/libtinyxml.lib" if ZEBuild.Platform.Platform == "Windows" else "/libtinyxml.a")
                FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libTinyXml.lib" if ZEBuild.Platform.Platform == "Windows" else "/libTinyXml.a")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                FileSource = self.BuildDirectory + "/lib/libtinyxml.a"
                FileDestination = self.OutputDirectory + "/Lib/libTinyXml.a"

            ZEBuild.CopyFile(FileSource, FileDestination)
            
    def GenerateCMakeList(self):
        ZEBuild.GenerateCMakeList(self.OutputDirectory, "2.8", "", True)

#Usage : ZEBuild.BuildLibrary(ClassName([LibraryName], [[ExtraLib],[ExtraLib],[ExtraLib]]
ZEBuild.BuildLibrary(TinyXMLLibrary("TinyXML", "SampleExtraLib.lib"))
