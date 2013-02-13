import os
import optparse
from ZELog import *
from ZEBuild import *
from ZELibrary import *
from ZEOperations import *
from ZECMake import *
from ZEAutoTools import *
from ZEPrebuildLibrary import *
from ZECMakeLists import *

class ZEBuildDriver:
    @staticmethod
    def ParseArguments():
        ZELog.Log("Parsing command line arguments.")

        parser = optparse.OptionParser()
        parser.add_option("-l", "--library", dest="Libraries", action="append", type="string",
                          help="Library name that will be build. If not given Source directory will be scanned and found libraries will be build. [Optional. Can be used multiple times if you want to build multiple libraries.]")
        parser.add_option("-p", "--platform", dest="Platform", action="store", type="string",
                          help="Platform name (Windows, Linux, MacOSX, PS3, XBox360, iOS, iOS-Simulator, Android) [Mandatory]")
        parser.add_option("-a", "--architecture", dest="Architecture", action="store", type="string",
                          help="Architecture name (x86, x64) [Madatory. For only Windows and Linux]")
        parser.add_option("-c", "--cmake-generator", dest="CMakeGenerator", action="store", type="string",
                          help="CMake generator name. (Look at generators section of the cmake --help command ouput) [Optional. Uses system default.]")
        parser.add_option("-s", "--sdk-root", dest="SDKRoot", action="store", type="string",
                  help="Platform SDK root path. [Optional For MacOSX. Mandatory For iOS and iOS-Simulator. For only MacOSX, iOS and iOS-Simulator]")
        parser.add_option("-o", "--output-directory", dest="OutputDirectory", action="store", type="string",
                          help="Output directory path. (For Expl: Output/Windows-x86 or SomeRepositoryFolder/Apple/iPhone)")

        (Options, args) = parser.parse_args()

        ZEPlatform.Platform = Options.Platform
        ZEPlatform.Architecture = Options.Architecture
        ZEPlatform.CMakeGenerator = Options.CMakeGenerator
        ZEPlatform.SDKRoot = Options.SDKRoot
        ZEBuild.OutputDirectory = Options.OutputDirectory
        ZEBuild.TargetLibraries = Options.Libraries
        
        ZEPlatform.PlatformString = ZEPlatform.Platform
        if (ZEPlatform.Architecture != None):
            ZEPlatform.PlatformString += "-" + ZEPlatform.Architecture

        if (ZEPlatform.CMakeGenerator[:6] == "Visual" or ZEPlatform.CMakeGenerator == "Xcode"):
            ZEPlatform.MultiConfiguration = True
        else:
            ZEPlatform.MultiConfiguration = False
           
        ZEBuild.RootDirectory = ZEOperations.GetWorkingDirectory()
        if (ZEBuild.OutputDirectory != None):
            ZEBuild.OutputDirectory = os.path.abspath(ZEBuild.OutputDirectory) + "/" + ZEPlatform.PlatformString
        else:
            ZEBuild.OutputDirectory = ZEBuild.RootDirectory + "/Output" +"/" + ZEPlatform.PlatformString

        if (ZEPlatform.Platform == "Windows"):
            ZEPlatform.LibExtension = ".lib"
            ZEPlatform.DLLExpension = ".dll"
            ZEPlatform.BinExtension = ".exe"
        elif (ZEPlatform.Platform == "Linux"):
            ZEPlatform.LibExtension = ".a"
            ZEPlatform.DLLExpension = ".so"
            ZEPlatform.BinExtension = ""
        elif (ZEPlatform.Platform == "MacOSX" or ZEPlatform.Platform == "iOS" or ZEPlatform.Platform == "iOS-Simulator"):
            ZEPlatform.LibExtension = ".a"
            ZEPlatform.DLLExpension = ".dylib"
            ZEPlatform.BinExtension = ""

    @staticmethod
    def BuildLibrary(Library):
        ZEBuild.Libraries.append(Library)
        ZEBuild.CurrentLibrary = Library
        Library.Build()

    @staticmethod
    def BuildDirectory(Directory):
        ZELog.Log("Scanning directory \"" + Directory + "\"")
        if (os.path.isfile(Directory + "/Build.py")):
            ZELog.Log("build.py found at \"" + Directory + "\".")
            ZEOperations.SetWorkingDirectory(Directory)
            execfile(Directory + "/build.py", globals(), locals())
            ZEBuild.CurrentLibrary = None

    @staticmethod
    def ShowResults():
        ResultText = "\nLibraries:\n"
        TotalResult = True
        for Library in ZEBuild.Libraries:
            ResultText += "  {0:<30} : {1}\n".format(Library.Name, ("Success" if Library.BuildResult else "Failed"))
            if (Library.BuildResult == False):
                TotalResult = False
            
        ZELog.Log("Build Result : " + ("Success" if TotalResult else "Failed") + "\n" + ResultText)
    
    @staticmethod
    def Build():
        ZEBuildDriver.ParseArguments()

        ZEOperations.CreateDirectory(ZEBuild.OutputDirectory)
        ZEOperations.CopyFile(ZEBuild.RootDirectory + "/Version.txt", ZEBuild.OutputDirectory + "/Version.txt")
        
        ZEBuild.Libraries = []

        DirectoryBase = os.getcwd()
        if (ZEBuild.TargetLibraries == None):
            DirectoryList = os.listdir(os.getcwd() + "/Source")
            ZERootCMakeLists.Begin()
        else:
            DirectoryList = ZEBuild.TargetLibraries

        ZELog.Log("Scanning directories...")
        for Directory in DirectoryList:
            CurrentDirectory = DirectoryBase + "/Source/" + Directory
            try:
                if (os.path.isdir(CurrentDirectory)):
                    ZEBuildDriver.BuildDirectory(CurrentDirectory)
        
            except ZEBuildException as e:
                ZELog.Error(e.ErrorText)
            #except Exception as e:
            #    ZELog.Error("Unknown Exception occured: " + e.message)
        
        if (ZEBuild.TargetLibraries == None):
            ZERootCMakeLists.End()
        
        ZEBuildDriver.ShowResults()
