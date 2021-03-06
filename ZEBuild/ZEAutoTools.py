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

        if (ZEPlatform.Platform == "Linux"):
            if (ZEPlatform.Architecture == "x86"):
                Architectures = "-m32"
            elif(ZEPlatform.Architecture == "x64"):
                Architectures = "-m64"

            ParameterString += "CFLAGS=\"" + Architectures  +  "\" "
            "CXXFLAGS=\"" + Architectures + "\" "

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

        Parameters = ""        
        if (ZEPlatform.Platform == "MacOSX" or ZEPlatform.Platform == "iOS" or ZEPlatform.Platform == "iOS-Simulator"):
            if (ZEPlatform.SDKRoot != None):
                SysRoot += "-syslibroot " + ZEPlatform.SDKRoot + " "
            else:
                SysRoot = ""

            if (ZEPlatform.Platform == "MacOSX"):
                Architectures = "-arch i386 -arch x86_64"
            elif (ZEPlatform.Platform == "iOS"):
                Architectures = "-arch armv6 -arch armv7"
            elif (ZEPlatform.Platform == "iOS-Simulator"):
                Architectures = "-arch i386"

            Parameters = "CFLAGS=\"" + Architectures + " " + SysRoot +  "\" "
            "CXXFLAGS=\"" + Architectures + " " + SysRoot +  "\" "
            "LDFLAGS=\"" + Architectures + " " + SysRoot +  "\""

        elif (ZEPlatform.Platform == "Linux"):
            if (ZEPlatform.Architecture == "x86"):
                Architectures = "-m32"
            elif(ZEPlatform.Architecture == "x64"):
                Architectures = "-m64"

            Parameters = "CFLAGS=\"" + Architectures  +  "\" "
            "CXXFLAGS=\"" + Architectures + "\" "

        ReturnValue = ZEOperations.Call("make", Parameters)
        if (ReturnValue != 0):
            raise ZEBuildException("Make failed.")

    @staticmethod
    def Install(Library, Configuration):
        ZEOperations.SetWorkingDirectory(Library.BuildDirectory)
        ReturnValue = ZEOperations.Call("make", "install")
        if (ReturnValue != 0):
            raise ZEBuildException("Make install failed.")