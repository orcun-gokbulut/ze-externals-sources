import sys
import os
from ZEBuild import *
from ZELog import *
from ZEOperations import *

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

    def GenerateCMakeList(Configuration):
	if (Configuration == ZEBuild.CONFIG_NONE):
	    ZELog.Log("Generating CMakeFiles.txt...")
	else:
	    ZELog.Log("Generating CMakeFiles.txt... Configuration : " + ("Debug" if Configuration == ZEBuild.CONFIG_DEBUG else "Release"))


    def Build(self):
        try:
            ZEOperations.CreateDirectory(self.LogDirectory)
            self.LogFile = open(self.LogDirectory + "/Build-" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S") + ".log", "w")

            ZELog.Log("Building platform library " + self.Name)          

	    self.Check()
	    self.CleanOutput()

            if (ZEPlatform.MultiConfiguration == False):

		ZEBuild.Configration = ZEBuild.CONFIG_NONE
                
                self.Clean()
                self.Configure(ZEBuild.CONFIG_NONE)
                self.Compile(ZEBuild.CONFIG_NONE)
                self.Gather(ZEBuild.CONFIG_NONE)
            else:
                ZELog.Log("Building Debug configuration...")
		
		ZEBuild.Configration = ZEBuild.CONFIG_DEBUG

                self.Clean()
                self.Configure(ZEBuild.CONFIG_DEBUG)
                self.Compile(ZEBuild.CONFIG_DEBUG)
                self.Gather(ZEBuild.CONFIG_DEBUG)

                ZELog.Log("Building Release configuration...")
		
		ZEBuild.Configration = ZEBuild.CONFIG_RELEASE
		
		self.Clean()
                self.Configure(ZEBuild.CONFIG_RELEASE)
                self.Compile(ZEBuild.CONFIG_RELEASE)
                self.Gather(ZEBuild.CONFIG_RELEASE)

	    self.GenerateCMakeList()
	    ZELog.Log(self.Name + " library has been successfully build.")
	    
        except ZEBuildException as e:
            ZELog.Error(e.ErrorText)
            raise ZEBuildException("Building library has failed.")

        finally:
	    if (self.LogFile != None):
		self.LogFile.close()

    def __init__(self, Name, ExtraLibraries):
        self.Name = Name
        self.RootDirectory = os.getcwd();
        if (ZEBuild.OutputDirectory != None):
            self.OutputDirectory = os.path.normpath(ZEBuild.OutputDirectory + "/" + self.Name)
        else:
            self.OutputDirectory = os.path.normpath(self.RootDirectory + "/Output")
        self.LogDirectory = os.path.normpath(self.RootDirectory  + "/Log")
        self.BuildDirectory = os.path.normpath(self.RootDirectory  + "/Build")
        self.SourceDirectory = os.path.normpath(self.RootDirectory  + "/Source")
	self.InstallDirectory = os.path.normpath(self.BuildDirectory + "/ZEInstall")

        self.ExtraLibraries = ExtraLibraries