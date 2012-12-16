import sys
import os
from ZEOperations import *
from ZEBuild import *

class ZEAutoTools:
    @staticmethod
    def Configure(Library, SourceDirectory, Parameters):
        ParameterString = "--prefix=\"" + Library.InstallDirectory + "\" --exec-prefix=\"" + Library.InstallDirectory + "\" "
        if (Parameters != None):
            for Parameter in Parameters:
                ParameterString += Parameter + " "

        ConfigureScriptPath = os.path.normpath(Library.SourceDirectory + "/" + SourceDirectory + "/configure")

        ReturnValue = ZEOperations.Call("chmod", "+x " + ConfigureScriptPath)
        if (ReturnValue != 0):
            raise ZEBuildException("Can not change configure scripts permision.")

        ZEOperations.SetWorkingDirectory(Library.BuildDirectory)
        
        ReturnValue = ZEOperations.Call(ConfigureScriptPath, ParameterString)
        if (ReturnValue != 0):
            raise ZEBuildException("Configure script failed.")

    @staticmethod
    def Build(Library, Configuration):
        ZEOperations.SetWorkingDirectory(Library.BuildDirectory)
        ReturnValue = ZEOperations.Call("make", "")
        if (ReturnValue != 0):
            raise ZEBuildException("Make failed.")

    @staticmethod
    def Install(Library, Configuration):
        ZEOperations.SetWorkingDirectory(Library.BuildDirectory)
        ReturnValue = ZEOperations.Call("make", "install")
        if (ReturnValue != 0):
            raise ZEBuildException("Make install failed.")