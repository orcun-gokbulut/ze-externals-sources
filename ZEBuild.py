import os
import sys
import subprocess
import optparse
import shutil
import datetime

class ZEBuildException:
    def __init__(self, ErrorText):
        self.ErrorText = ErrorText

    def __str__(self):
        return ZEBuild.OutputStr("Error", ErrorText)

class ZEPlatform:
    def __init__(self):
        pass

class ZEBuild:
    CurrentLibrary = None
    OutputDirectoryPrefix = None

    @staticmethod
    def CopyFile(SourcePath, DestinationPath):
        SourcePath = os.path.normpath(SourcePath)
        DestinationPath = os.path.normpath(DestinationPath)
        shutil.copyfile(SourcePath, DestinationPath)

    @staticmethod
    def RemoveFile(SourcePath):
        SourcePath = os.path.normpath(SourcePath)
        os.remove(SourcePath)

    @staticmethod
    def RenameFile(SourcePath, DestinationPath):
        SourcePath = os.path.normpath(SourcePath)
        DestinationPath = os.path.normpath(DestinationPath)
        os.rename(SourcePath, DestinationPath)

    @staticmethod
    def CreateDirectory(DirectoryName):
        try:
            DirectoryName = os.path.normpath(DirectoryName)
            if(not os.path.isdir(DirectoryName)):
                os.makedirs(DirectoryName)
        except:
            raise ZEBuildException("Can not create directory. Directory Name : " + DirectoryName)

    @staticmethod
    def CopyDirectory(SourceDirectory, DestinationDirectory):
        try:
            SourceDirectory = os.path.normpath(SourceDirectory)
            DestinationDirectory = os.path.normpath(DestinationDirectory)
            shutil.copytree(SourceDirectory, DestinationDirectory)
        except:
            raise ZEBuildException("Can not copy directory. Source Directory Name : " + SourceDirectory + ", Destination Directory : " + DestinationDirectory)

    @staticmethod
    def RemoveDirectory(DirectoryName):
        try:
            DirectoryName = os.path.normpath(DirectoryName)
            if(os.path.isdir(DirectoryName)):
                shutil.rmtree(DirectoryName)
        except:
            raise ZEBuildException("Can not remove directory. Directory Name : " + DirectoryName)

    @staticmethod
    def IsDirectoryExists(DirectoryName):
        if os.path.exists(os.path.normpath(DirectoryName)):
            return True
        else:
            return False

    @staticmethod
    def SetWorkingDirectory(DirectoryName):
        try:
            DirectoryName = os.path.normpath(DirectoryName)
            os.chdir(DirectoryName)
        except:
            raise ZEBuildException("Can not change working directory. Directory Name : " + DirectoryName)

    @staticmethod
    def GetWorkingDirectory():
        return os.path.normpath(os.getcwd())

    @staticmethod  
    def OutputStr(Level, Text):
        if (ZEBuild.CurrentLibrary != None):
            return "[" + Level + "] " + ZEBuild.CurrentLibrary.Name + " : " + Text
        else:
            return "[" + Level + "] ZEBuild : " + Text

    @staticmethod
    def Output(Level, Text):
        OutputText = ZEBuild.OutputStr(Level, Text)

        if (ZEBuild.CurrentLibrary != None and ZEBuild.CurrentLibrary.LogFile != None and ZEBuild.CurrentLibrary.LogFile.closed == False):
            ZEBuild.CurrentLibrary.LogFile.write(OutputText + "\n")
            ZEBuild.CurrentLibrary.LogFile.flush()

        print(OutputText)

    @staticmethod
    def Error(Text):
        ZEBuild.Output("Error", Text)

    @staticmethod
    def Warning(Text):
        ZEBuild.Output("Warning", Text)

    @staticmethod
    def Log(Text):
        ZEBuild.Output("Log", Text)

    @staticmethod
    def Call(Command, Arguments):
        if (ZEBuild.CurrentLibrary != None and ZEBuild.CurrentLibrary.LogFile != None and ZEBuild.CurrentLibrary.LogFile.closed != True):
            ReturnValue = subprocess.call(Command + " " + Arguments, shell=True, stdout = ZEBuild.CurrentLibrary.LogFile, stderr = ZEBuild.CurrentLibrary.LogFile)
            ZEBuild.CurrentLibrary.LogFile.flush()
            return ReturnValue
        else:
            return subprocess.call(Command + " " + Arguments, shell = True)

    @staticmethod
    def CMake(SourceDirectory, Parameters):
        ParameterString = ""
        SourceDirectory = os.path.normpath(SourceDirectory)
        if (Parameters != None):
            for Parameter in Parameters:
                ParameterString += "-D" + Parameter.Name + ":" + Parameter.Type + "=\"" + Parameter.Value + "\" "

        Arguments = " -G \"" + ZEBuild.Platform.CMakeGenerator + "\" " + ParameterString + " \"" + SourceDirectory + "\""
        if (ZEBuild.Platform.Platform == "Linux"):
            Arguments += " -DCMAKE_TOOLCHAIN_FILE=" + ZEBuild.RootDirectory + "/CMake/toolchain-linux-" + ("x86" if ZEBuild.Platform.Architecture == "x86" else "x64") + ".cmake"

        ReturnValue = ZEBuild.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake configure failed.")

    @staticmethod
    def CMakeBuild(BuildDirectory, Debug):
        ConfigurationText = ""        

        os.path.normpath(BuildDirectory)
        os.chdir(BuildDirectory)

        if (Debug != None):
            ConfigurationText = " --config Debug" if Debug else " --config Release"
        Arguments = " --build \"" + BuildDirectory + "\"" + ConfigurationText      

        ReturnValue = ZEBuild.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake build failed.")

    @staticmethod
    def CMakeInstall(BuildDirectory, Debug):
        ConfigurationText = ""

        os.path.normpath(BuildDirectory)
        os.chdir(BuildDirectory)

        if (Debug != None):
            ConfigurationText = " --config Debug" if Debug else " --config Release"  
        Arguments = " --build \"" + BuildDirectory + "\" --target install " + ConfigurationText

        ReturnValue = ZEBuild.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake build failed.")

    @staticmethod
    def GenerateCMakeList(OutputDirectory, CMakeVersion, SystemLibs, Combinable):
        BuildDirectory = os.path.normpath(OutputDirectory)
        CMakeList = open(OutputDirectory + "/CMakeList.txt", "w")
        CMakeList.write("cmake_required(VERSION " + CMakeVersion + ")\n\n")
        CMakeList.write("ze_register_platform_library(NAME "+ ZEBuild.CurrentLibrary.Name + 
                        " PLATFORM " + ZEBuild.Platform.Platform + 
                        " ARCHITECTURE " + ZEBuild.Platform.Architecture + 
                        ((" SYSTEM_LIBS " + SystemLibs) if SystemLibs != "" else "") +
                        (" COMBINABLE)" if Combinable != False else ")"))
        CMakeList.flush()
        CMakeList.close()


    @staticmethod
    def BuildLibrary(Library):
        ZEBuild.CurrentLibrary = Library
        Library.Build()

class ZECMakeParameter:
    def __init__(self, Name, Type, Value):
        self.Name = Name
        self.Type = Type
        self.Value = Value

    def __getparameters__():
        return [Name, Type, Value] 

class ZELibrary:
    LogFile = None
    def Clean(self):
        ZEBuild.Log("Cleaning.")

        ZEBuild.RemoveDirectory(self.OutputDirectory)
        ZEBuild.CreateDirectory(self.OutputDirectory)     

        ZEBuild.RemoveDirectory(self.BuildDirectory)
        ZEBuild.CreateDirectory(self.BuildDirectory)

    def Configure(self, Debug):
        ZEBuild.Log("Configuring build.")

    def Compile(self, Debug):
        ZEBuild.Log("Compiling library.")

    def Gather(self, Debug):
        ZEBuild.Log("Gathering generated resources.")

    def GenerateCMakeList(self):
        ZEBuild.Log("Generating CMakeLists.txt file.")

    def Build(self):
        try:
            ZEBuild.CreateDirectory(self.LogDirectory)
            self.LogFile = open(self.LogDirectory + "/Build-" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S") + ".log", "w")

            ZEBuild.Log("Building platform library " + self.Name)          

            if (ZEBuild.Platform.MultiConfiguration == False):
                self.Clean()
                self.Configure(False)
                self.Compile(False)
                self.Gather(False)
                self.GenerateCMakeList()
            else:
                ZEBuild.Log("Building debug configuration.")
                self.Clean()
                self.Configure(True)
                self.Compile(True)
                self.Gather(True)

                ZEBuild.Log("Building release configuration.")
                self.Configure(False)
                self.Compile(False)
                self.Gather(False)

                self.GenerateCMakeList()

        except ZEBuildException as e:
            ZEBuild.Error(e.ErrorText)
            raise ZEBuildException("Building library has failed.")

        except Exception as e:
            ZEBuild.Error("Unknown Exception occured: " + e.message)
            raise ZEBuildException("Building library has failed.")

        finally:
            self.LogFile.close()

    def __init__(self, Name, ExtraLibraries):
        self.Name = Name
        self.RootDirectory = os.getcwd();
        if (ZEBuild.OutputDirectory != None):
            self.OutputDirectory = ZEBuild.OutputDirectory + "/" + self.Name
        else:
            self.OutputDirectory = self.RootDirectory + "/Output"
        self.LogDirectory = self.RootDirectory  + "/Log"
        self.BuildDirectory = self.RootDirectory  + "/Build"
        self.SourceDirectory = self.RootDirectory  + "/Source"

        self.OutputDirectory = os.path.normpath(self.OutputDirectory)
        self.LogDirectory = os.path.normpath(self.LogDirectory)
        self.BuildDirectory = os.path.normpath(self.BuildDirectory)
        self.SourceDirectory = os.path.normpath(self.SourceDirectory)

        self.ExtraLibraries = ExtraLibraries


def Main():
    ZEBuild.Log("ZEBuild System v0.1")
    ZEBuild.Log("Initializing...")

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

    ZEBuild.Platform = ZEPlatform()
    ZEBuild.Platform.Platform = Options.Platform
    ZEBuild.Platform.Architecture = Options.Architecture
    ZEBuild.Platform.CMakeGenerator = Options.CMakeGenerator
    ZEBuild.OutputDirectory = Options.OutputDirectory
    if (ZEBuild.OutputDirectory != None):
        ZEBuild.OutputDirectory = os.path.abspath(ZEBuild.OutputDirectory)

    if (ZEBuild.Platform.CMakeGenerator[:6] == "Visual" or ZEBuild.Platform.CMakeGenerator == "Xcode"):
        ZEBuild.Platform.MultiConfiguration = True
    else:
        ZEBuild.Platform.MultiConfiguration = False

    ZEBuild.RootDirectory = ZEBuild.GetWorkingDirectory()

    DirectoryBase = os.getcwd()
    DirectoryList = os.listdir(os.getcwd() + "/Source")

    ZEBuild.Log("Scanning directories...")
    for Directory in DirectoryList:
        CurrentDirectory = DirectoryBase + "/Source/" + Directory
        try:
            if (os.path.isdir(CurrentDirectory)):
                ZEBuild.Log("Scanning directory \"" + "Source/" + Directory + "\"")
                if (os.path.isfile(CurrentDirectory + "/build.py")):
                    ZEBuild.Log("Build.py found at \"" + "Source/" + Directory + "\".")
                    ZEBuild.SetWorkingDirectory(CurrentDirectory)
                    execfile(CurrentDirectory + "/build.py", globals(), locals())
                    ZEBuild.CurrentLibrary = None
        except ZEBuildException as e:
            ZEBuild.Error(e.ErrorText)
        '''except Exception as e:
            ZEBuild.Error("Unknown Exception occured: " + e.message)'''


if (__name__ == "__main__"):
    Main()
