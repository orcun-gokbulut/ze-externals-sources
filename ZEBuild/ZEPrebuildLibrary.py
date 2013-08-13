import sys
import os
from ZELibrary import *
#from ZEOperations import *

class ZEPrebuildLibrary(ZELibrary):
    def Check(self):
        ZELibrary.Check(self)
        DirectoryList = os.listdir(self.RootDirectory + "/Prebuild")
        for Directory in DirectoryList:
            if (ZEOperations.IsDirectory(self.RootDirectory + "/Prebuild/" + Directory)):
                if (Directory == ZEPlatform.PlatformString):
                    ZEPrebuildLibrary.SourceDirectory = ZEPlatform.PlatformString
                    return

        for Directory in DirectoryList:
            if (ZEOperations.IsDirectory(self.RootDirectory + "/Prebuild/" + Directory)):
                if (Directory == ZEPlatform.Platform + "-" + ZEPlatform.Architecture):
                    ZEPrebuildLibrary.SourceDirectory = ZEPlatform.Platform + "-" + ZEPlatform.Architecture
                    return
                                            
        raise ZEBuildException("Prebuild library " + self.Name + " does not have " + ZEPlatform.PlatformString + " or generic platform support.")

    def Gather(self, Configuration):
        if (Configuration == ZEBuild.CONFIG_NONE or Configuration == ZEBuild.CONFIG_RELEASE):
            ZEOperations.CopyDirectory(self.RootDirectory + "/Prebuild/" + ZEPrebuildLibrary.SourceDirectory, self.OutputDirectory)