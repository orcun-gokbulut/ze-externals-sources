import sys
import os
from ZEBuild import *
from ZELog import *
from ZEOperations import *
from ZECMakeLists import *

class ZELibrary:
    LogFile = None

    def Check(self):
        ZELog.Log("Checking...")

    def CleanOutput(self):
        ZELog.Log("Cleaning output...")
        ZEOperations.RemoveDirectory(self.OutputDirectory)
        ZEOperations.CreateDirectory(self.OutputDirectory)     

    def Clean(self):
        ZELog.Log("Cleaning build...")

        ZEOperations.SetWorkingDirectory(self.RootDirectory)
        ZEOperations.RemoveDirectory(self.BuildDirectory)
        ZEOperations.CreateDirectory(self.BuildDirectory)

    def Configure(self, Configuration):
        if (Configuration == ZEBuild.CONFIG_NONE):
            ZELog.Log("Configuring...")
        else:
            ZELog.Log("Configuring... Configuration : " + ("Debug" if Configuration == ZEBuild.CONFIG_DEBUG else "Release"))

    def Compile(self, Configuration):
        if (Configuration == ZEBuild.CONFIG_NONE):
            ZELog.Log("Compiling...")
        else:
            ZELog.Log("Compiling... Configuration : " + ("Debug" if Configuration == ZEBuild.CONFIG_DEBUG else "Release"))

    def Gather(self, Configuration):
        if (Configuration == ZEBuild.CONFIG_NONE):
            ZELog.Log("Gathering outputs...")
        else:
            ZELog.Log("Gathering outputs... Configuration : " + ("Debug" if Configuration == ZEBuild.CONFIG_DEBUG else "Release"))

    def GenerateCMakeList(self):
        ZELog.Log("Generating CMakeFiles.txt...")
        Script = ZELibraryCMakeLists()
        Script.Begin(self)
        Script.AddSubLibrary(self.Name)
        Script.End()

    def CopyVersionTxt(self):
        ZELog.Log("Copying Version.txt...")
        ZEOperations.CopyFile(self.RootDirectory + "/Version.txt", self.OutputDirectory + "/Version.txt")
        
    def Build(self):
        try:
            ZEOperations.CreateDirectory(self.LogDirectory)
            self.LogFile = open(self.LogDirectory + "/" + self.Name + "-build-" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S") + ".log", "w")

            ZELog.Log("Building platform library " + self.Name)          

            self.Check()
        
            self.CleanOutput()

            self.CopyVersionTxt()

            if (ZEPlatform.MultiConfiguration == False):

                ZEBuild.Configration = ZEBuild.CONFIG_NONE
                self.Clean()
                self.Configure(ZEBuild.CONFIG_NONE)
                self.Compile(ZEBuild.CONFIG_NONE)
                self.Gather(ZEBuild.CONFIG_NONE)
            else:
                MainBuildDirectory = self.BuildDirectory
                
                ZELog.Log("Building Debug configuration...")

                ZEBuild.Configration = ZEBuild.CONFIG_DEBUG
                self.BuildDirectory = MainBuildDirectory + "/Debug"
                self.InstallDirectory = self.BuildDirectory + "/ZEInstall"                
                self.Clean()
                self.Configure(ZEBuild.CONFIG_DEBUG)
                self.Compile(ZEBuild.CONFIG_DEBUG)
                self.Gather(ZEBuild.CONFIG_DEBUG)

                ZELog.Log("Building Release configuration...")

                ZEBuild.Configration = ZEBuild.CONFIG_RELEASE
                self.BuildDirectory = MainBuildDirectory + "/Release"
                self.InstallDirectory = self.BuildDirectory + "/ZEInstall"
                self.Clean()
                self.Configure(ZEBuild.CONFIG_RELEASE)
                self.Compile(ZEBuild.CONFIG_RELEASE)
                self.Gather(ZEBuild.CONFIG_RELEASE)
            
                self.BuildDirectory = MainBuildDirectory
            
            self.GenerateCMakeList()
            self.BuildResult = True
            ZELog.Log(self.Name + " library has been successfully build.")

        except ZEBuildException as e:
            self.BuildResult = False
            ZELog.Error(e.ErrorText)
            raise ZEBuildException("Building library has failed.")

        finally:
            if (self.LogFile != None):
                self.LogFile.close()

    def __init__(self, Name, ExtraLibraries):
        self.Name = Name
        self.RootDirectory = os.getcwd();
        self.SourceDirectory = self.RootDirectory + "/Source"
        self.LogDirectory = os.path.normpath(ZEBuild.RootDirectory + "/Log/" + self.Name)
        self.OutputDirectory = os.path.normpath(ZEBuild.OutputDirectory + "/" + self.Name)
        self.BuildDirectory = os.path.normpath(ZEBuild.RootDirectory + "/Build/" + self.Name)
        self.InstallDirectory = os.path.normpath(self.BuildDirectory + "/ZEInstall")

        self.ExtraLibraries = ExtraLibraries