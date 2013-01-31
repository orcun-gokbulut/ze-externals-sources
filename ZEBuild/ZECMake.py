import sys
from ZEBuild import *
from ZEOperations import *

class ZECMakeParameter:
    def __init__(self, Name, Type, Value):
        self.Name = Name
        self.Type = Type
        self.Value = Value

    def __getparameters__():
        return [Name, Type, Value] 

class ZECMake:
    @staticmethod
    def Configure(Library, SourceDirectory, Parameters):
        ParameterString = ""
        if (Parameters != None):
            for Parameter in Parameters:
                ParameterString += "-D" + Parameter.Name + ":" + Parameter.Type + "=\"" + Parameter.Value + "\" "

        ParameterString += "-DCMAKE_INSTALL_PREFIX=\"" + ZEBuild.CurrentLibrary.InstallDirectory + "\" "

        if (ZEPlatform.Platform == "MacOSX"):
            if (ZEPlatform.Architecture == "x86"):
                ParameterString += " -DCMAKE_OSX_ARCHITECTURES:STRING=\"i386\""
            elif (ZEPlatform.Architecture == "x64"):
                    ParameterString += " -DCMAKE_OSX_ARCHITECTURES:STRING=\"x86_64\""
        
        elif (ZEPlatform.Platform == "iOS"):
            ParameterString += " -DCMAKE_OSX_ARCHITECTURES:STRING=\"armv7\""
            
        elif (ZEPlatform.Platform == "iOS-Simulator"):
            if (ZEPlatform.Architecture == "x86"):
                ParameterString += " -DCMAKE_OSX_ARCHITECTURES:STRING=\"i386\""
            elif (ZEPlatform.Architecture == "x64"):
                    ParameterString += " -DCMAKE_OSX_ARCHITECTURES:STRING=\"x86_64\""

        elif (ZEPlatform.Platform == "Linux"):
            ParameterString += " -DCMAKE_TOOLCHAIN_FILE:STRING=" + ZEBuild.RootDirectory + "/CMake/toolchain-linux-" + str.lower(ZEPlatform.Architecture) + ".cmake"
        
        if (ZEPlatform.SDKRoot != None):
            ParameterString += " -DCMAKE_OSX_SYSROOT:STRING=\"" + ZEBuild.SDKRoot + "\""

        FullSourceDirectory = os.path.normpath(Library.SourceDirectory + "/" + SourceDirectory)
        Arguments = ""
        if (ZEPlatform.CMakeGenerator != None):
            Arguments += "-G \"" + ZEPlatform.CMakeGenerator + "\" "

        Arguments += ParameterString + " \"" + FullSourceDirectory + "\""

        ZEOperations.SetWorkingDirectory(Library.BuildDirectory)

        ReturnValue = ZEOperations.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake configure failed.")

    @staticmethod
    def Build(Library, Configuration):
        ConfigurationText = ""        

        ZEOperations.SetWorkingDirectory(Library.BuildDirectory)

        if (Configuration == ZEBuild.CONFIG_NONE):
            ConfigurationText = ""
        elif (Configuration == ZEBuild.CONFIG_DEBUG):
            ConfigurationText = "--config Debug"
        elif (Configuration == ZEBuild.CONFIG_RELEASE):
            ConfigurationText = "--config Release"

        Arguments = " --build \"" + Library.BuildDirectory + "\" " + ConfigurationText
        ReturnValue = ZEOperations.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake build failed.")

    @staticmethod
    def Install(Library, Configuration):
        ConfigurationText = ""

        ZEOperations.SetWorkingDirectory(Library.BuildDirectory)

        if (Configuration == ZEBuild.CONFIG_NONE):
            ConfigurationText = ""
        elif (Configuration == ZEBuild.CONFIG_DEBUG):
            ConfigurationText = "--config Debug"
        elif (Configuration == ZEBuild.CONFIG_RELEASE):
            ConfigurationText = "--config Release"

        Arguments = " --build \"" + Library.BuildDirectory + "\" --target install " + ConfigurationText

        ReturnValue = ZEOperations.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake install failed.")

    @staticmethod
    def GenerateCMakeList(OutputDirectory, SystemLibs, Combinable):
        BuildDirectory = os.path.normpath(OutputDirectory)
        CMakeList = open(OutputDirectory + "/CMakeList.txt", "w")
        CMakeList.write("cmake_required(VERSION 2.8)\n\n")
        CMakeList.write("ze_register_platform_library(NAME "+ ZEBuild.CurrentLibrary.Name + " " +
                        "\tPLATFORM " + ZEPlatform.Platform + " " +
                        "\tARCHITECTURE " + ZEPlatform.Architecture + " " + 
                        ("\tSYSTEM_LIBS " + SystemLibs) if SystemLibs != "" else "" + " " +
                        ("\tCOMBINABLE)" if Combinable != False else "") + 
                        ")")
        CMakeList.flush()
        CMakeList.close()