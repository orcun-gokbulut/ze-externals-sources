import os
import optparse
from ZELog import *
from ZEBuild import *
from ZELibrary import *
from ZEOperations import *
from ZECMake import *
from ZEAutoTools import *
from ZEPrebuildLibrary import *

class ZEBuildDriver:
    @staticmethod
    def ParseArguments():
        ZELog.Log("Parsing command line arguments.")

        parser = optparse.OptionParser()
        parser.add_option("-p", "--platform", dest="Platform", action="store", type="string",
                          help="Platform name (Windows, Linux, Unix, MacOS, PS3, xBox360, iPhone, iPhoneSimulator, Android)")
        parser.add_option("-a", "--architecture", dest="Architecture", action="store", type="string",
                          help="Architecture name (x86, x64, PPC, ARM)")
        parser.add_option("-c", "--cmake-generator", dest="CMakeGenerator", action="store", type="string",
                          help="CMake generator name. (VS20XX, NMake, Make, XCode)")
        parser.add_option("-o", "--output-directory", dest="OutputDirectory", action="store", type="string",
                          help="Combined output directory path.")
        (Options, args) = parser.parse_args()

        ZEPlatform.Platform = Options.Platform
        ZEPlatform.Architecture = Options.Architecture
        ZEPlatform.CMakeGenerator = Options.CMakeGenerator
        ZEBuild.OutputDirectory = Options.OutputDirectory
        if (ZEBuild.OutputDirectory != None):
            ZEBuild.OutputDirectory = os.path.abspath(ZEBuild.OutputDirectory)
        ZEPlatform.PlatformString = ZEPlatform.Platform + "-" + ZEPlatform.Architecture

        if (ZEPlatform.CMakeGenerator[:6] == "Visual" or ZEPlatform.CMakeGenerator == "Xcode"):
            ZEPlatform.MultiConfiguration = True
        else:
            ZEPlatform.MultiConfiguration = False

        ZEBuild.RootDirectory = ZEOperations.GetWorkingDirectory()

        if (ZEPlatform.Platform == "Windows"):
            ZEPlatform.LibExtension = ".lib"
            ZEPlatform.DLLExpension = ".dll"
            ZEPlatform.BinExtension = ".exe"
        elif (ZEPlatform.Platform == "Linux"):
            ZEPlatform.LibExtension = ".a"
            ZEPlatform.DLLExpension = ".so"
            ZEPlatform.BinExtension = ""
        elif (ZEPlatform.Platform == "MacOS" or ZEPlatform.Platform == "iPhone" or ZEPlatform.Platform == "iPhone-Simulator"):
            ZEPlatform.LibExtension = ".a"
            ZEPlatform.DLLExpension = ".dynlib"
            ZEPlatform.BinExtension = ""

    @staticmethod
    def BuildLibrary(Library):
        ZEBuild.CurrentLibrary = Library
        Library.Build()

    @staticmethod
    def ScanDirectory(Directory):
        ZELog.Log("Scanning directory \"" + Directory + "\"")
        if (os.path.isfile(Directory + "/build.py")):
            ZELog.Log("build.py found at \"" + Directory + "\".")
            ZEOperations.SetWorkingDirectory(Directory)
            execfile(Directory + "/build.py", globals(), locals())
            ZEBuild.CurrentLibrary = None

    @staticmethod
    def ScanAndBuild():
        DirectoryBase = os.getcwd()
        DirectoryList = os.listdir(os.getcwd() + "/Source")

        ZELog.Log("Scanning directories...")
        for Directory in DirectoryList:
            CurrentDirectory = DirectoryBase + "/Source/" + Directory
            try:
                if (os.path.isdir(CurrentDirectory)):
                    ZEBuild.ScanDirectory(CurrentDirectory)

            except ZEBuildException as e:
                ZELog.Error(e.ErrorText)
            #except Exception as e:
            #    ZELog.Error("Unknown Exception occured: " + e.message)