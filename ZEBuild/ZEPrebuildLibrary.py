import sys
import os
from ZELibrary import *
#from ZEOperations import *

class ZEPrebuildLibrary(ZELibrary):
    def Check(self):
        ZELibrary.Check(self)
        DirectoryList = os.listdir(self.SourceDirectory)
        for Directory in DirectoryList:
            if (os.path.isdir(self.SourceDirectory + "/" + Directory)):
                if (Directory == ZEPlatform.PlatformString):
                    return

        raise ZEBuildException("Prebuild library " + self.Name + " does not have " + ZEPlatform.PlatformString + " platform support.")

    def Gather(self, Configuration):
        if (Configuration == ZEBuild.CONFIG_NONE or Configuration == ZEBuild.CONFIG_RELEASE):
            ZEOperations.CopyDirectory(self.SourceDirectory + "/" + ZEPlatform.PlatformString, self.OutputDirectory)