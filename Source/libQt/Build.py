import os

class libQt(ZELibrary):
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)

        Arguments  = "-prefix \"" + self.InstallDirectory + "\" "
        Arguments += "-debug-and-release "
        Arguments += "-optimized-tools "
        Arguments += "-opensource "
        Arguments += "-confirm-license "
        Arguments += "-shared "
        Arguments += "-nomake examples "
        Arguments += "-nomake tests "
        Arguments += "-no-ssl "

        OldPath = os.environ["PATH"];
        if (ZEPlatform.Platform == "Windows"):
            os.environ["PATH"] += ";" + self.SourceDirectory
        else:
            os.environ["PATH"] += ":" + self.SourceDirectory
            
        ZEOperations.SetWorkingDirectory(self.BuildDirectory)
        ReturnValue = ZEOperations.Call(self.SourceDirectory + "/configure", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("Qt configure failed.")

        os.environ["PATH"] = OldPath
        
        if (ZEPlatform.Platform == "Windows"):
            self.MakeUtility = "nmake"
        else:
            self.MakeUtility = "make"

            
    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        
        ReturnValue = ZEOperations.Call(self.MakeUtility, "")
        if (ReturnValue != 0):
            raise ZEBuildException("Make failed.")


    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        
        ReturnValue = ZEOperations.Call(self.MakeUtility, "install")
        if (ReturnValue != 0):
            raise ZEBuildException("Make Install failed.")
        
        ZEOperations.CopyDirectory(self.InstallDirectory, self.OutputDirectory)

        
    def GenerateCMakeList(self):
        ZELog.Log("Generating CMakeFiles.txt...")
        Script = ZELibraryCMakeLists()
        Script.Begin(self)
        Script.AddText("set(CMAKE_PREFIX_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake\" CACHE PATH \"\")\n\n")
        Script.AddSubLibrary(self.Name)
        Script.End()

        
    def __init__(self):
        ZELibrary.__init__(self, "libQt", "")
        self.SingleConfiguration = True
        
ZEBuildDriver.BuildLibrary(libQt())