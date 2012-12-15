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
    
    CONFIG_NONE = 0
    CONFIG_DEBUG = 1
    CONFIG_RELEASE = 2

    @staticmethod
    def CopyFile(SourcePath, DestinationPath):
	ZEBuild.DebugLog("Copying file. Source File : \"" + SourcePath + "\". Destination File : \"" + DestinationPath + "\".")
        SourcePath = os.path.normpath(SourcePath)
        DestinationPath = os.path.normpath(DestinationPath)
        shutil.copyfile(SourcePath, DestinationPath)

    @staticmethod
    def RemoveFile(SourcePath):
	ZEBuild.DebugLog("Deleting file. File : \"" + SourcePath + "\"")
        SourcePath = os.path.normpath(SourcePath)
        os.remove(SourcePath)

    @staticmethod
    def RenameFile(SourcePath, DestinationPath):
	ZEBuild.DebugLog("Copying file. Source File : \"" + SourcePath + "\". Destination File : \"" + DestinationPath + "\".")
        SourcePath = os.path.normpath(SourcePath)
        DestinationPath = os.path.normpath(DestinationPath)
        os.rename(SourcePath, DestinationPath)

    @staticmethod
    def CreateDirectory(DirectoryName):
	ZEBuild.DebugLog("Creating directory. Directory Name : \"" + DirectoryName + "\"")
        try:
            DirectoryName = os.path.normpath(DirectoryName)
            if(not os.path.isdir(DirectoryName)):
                os.makedirs(DirectoryName)
        except:
            raise ZEBuildException("Can not create directory. Directory Name : " + DirectoryName)

    @staticmethod
    def CopyDirectory(SourceDirectory, DestinationDirectory):
	ZEBuild.DebugLog("Copying directory. Source Direcyory : \"" + SourceDirectory + "\". Destination Directory : \"" + DestinationDirectory + "\"")

        try:
	    if (not os.path.isdir(SourceDirectory)):
		return
	    
	    ZEBuild.CreateDirectory(DestinationDirectory)
	    
	    DirectoryList = os.listdir(SourceDirectory)
	    for CurrentFile in DirectoryList:
		
		SourceFile = SourceDirectory + "/" + CurrentFile
		DestinationFile = DestinationDirectory + "/" + CurrentFile
		
		if (os.path.isdir(SourceFile)):
		    if (CurrentFile == ".svn" or CurrentFile == ".git"):
			continue

		    ZEBuild.CopyDirectory(SourceFile, DestinationFile)
		else:
		    shutil.copyfile(SourceFile, DestinationFile)
        except:
            raise ZEBuildException("Can not copy directory. Source Directory Name : " + SourceDirectory + ". Destination Directory : " + DestinationDirectory + "\".")

    @staticmethod
    def GetConfig():
	if (ZEBuild.Configration == ZEBuild.CONFIG_NONE):
	    return ""
	elif (ZEBuild.Configration == ZEBuild.CONFIG_DEBUG):
	    return "Debug"
	elif (ZEBuild.Configration == ZEBuild.CONFIG_RELEASE):
	    return "Release"
	else:
	    raise ZEBuildException("Unknown configuration. Configuration : " + ZEBuild.Configration)
 
    @staticmethod
    def CopyInstallToOutput(Library, Configuration, IncludeDirectory = None, LibDirectory = None, DLLDirectory = None, BinDirectory = None):
	if (IncludeDirectory != None):
	    if (Configuration == ZEBuild.CONFIG_NONE or Configuration == ZEBuild.CONFIG_RELEASE):
		ZEBuild.CopyDirectory(Library.InstallDirectory + "/" + IncludeDirectory, Library.OutputDirectory + "/Include")
	
	if (LibDirectory != None):
	    if (Configuration == ZEBuild.CONFIG_NONE):
		ZEBuild.CopyDirectory(Library.InstallDirectory + "/" + LibDirectory, Library.OutputDirectory + "/Lib")
	    else:
		ZEBuild.CopyDirectory(Library.InstallDirectory + "/" + LibDirectory, Library.OutputDirectory + "/Lib/" + ZEBuild.GetConfig())

	if (DLLDirectory != None):
	    if (Configuration == ZEBuild.CONFIG_NONE):
		ZEBuild.CopyDirectory(Library.InstallDirectory + "/" + DLLDirectory, Library.OutputDirectory + "/DLL")
	    else:
		ZEBuild.CopyDirectory(Library.InstallDirectory + "/" + DLLDirectory, Library.OutputDirectory + "/DLL/" + ZEBuild.GetConfig())
    
	if (BinDirectory != None):
	    if (Configuration == ZEBuild.CONFIG_NONE):
		ZEBuild.CopyDirectory(Library.InstallDirectory + "/" + BinDirectory, Library.OutputDirectory + "/Bin")
	    else:
		if (Configuration == ZEBuild.CONFIG_RELEASE):
		    ZEBuild.CopyDirectory(Library.InstallDirectory + "/" + BinDirectory, Library.OutputDirectory + "/Bin")
	        
    @staticmethod   
    def CopyInstallToOutputAuto(Library, Configuration):
	ZEBuild.CopyInstallToOutput(Library, Configuration, "Include", "Lib", "DLL", "Bin")

    @staticmethod
    def RemoveDirectory(DirectoryName):
	ZEBuild.DebugLog("Removing directory. Directory Name : \"" + DirectoryName + "\"")
        try:
            DirectoryName = os.path.normpath(DirectoryName)
            if(os.path.isdir(DirectoryName)):
                shutil.rmtree(DirectoryName)
        except:
            raise ZEBuildException("Can not remove directory. Directory Name : \"" + DirectoryName + "\".")

    @staticmethod
    def IsDirectoryExists(DirectoryName):
        if os.path.exists(os.path.normpath(DirectoryName)):
            return True
        else:
            return False

    @staticmethod
    def SetWorkingDirectory(DirectoryName):
	ZEBuild.DebugLog("Changing working directory. Directory Name : \"" + DirectoryName + "\".")

        try:
            DirectoryName = os.path.normpath(DirectoryName)
            os.chdir(DirectoryName)
        except:
            raise ZEBuildException("Can not change working directory. Directory Name : \"" + DirectoryName + "\".")

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

	if (Level != "Debug"):
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
    def DebugLog(Text):
	ZEBuild.Output("Debug", Text)

    @staticmethod
    def Call(Command, Arguments):
	ZEBuild.DebugLog("Running command. Command : " + Command + ". Arguments : " + Arguments)
        if (ZEBuild.CurrentLibrary != None and ZEBuild.CurrentLibrary.LogFile != None and ZEBuild.CurrentLibrary.LogFile.closed != True):
            ReturnValue = subprocess.call(Command + " " + Arguments, shell=True, stdout = ZEBuild.CurrentLibrary.LogFile, stderr = ZEBuild.CurrentLibrary.LogFile)
            ZEBuild.CurrentLibrary.LogFile.flush()
            return ReturnValue
        else:
            return subprocess.call(Command + " " + Arguments, shell = True)

    @staticmethod
    def CMake(Library, SourceDirectory, Parameters):
        ParameterString = ""
        if (Parameters != None):
            for Parameter in Parameters:
                ParameterString += "-D" + Parameter.Name + ":" + Parameter.Type + "=\"" + Parameter.Value + "\" "

	ParameterString += "-DCMAKE_INSTALL_PREFIX=\"" + ZEBuild.CurrentLibrary.InstallDirectory + "\" "
	
	if (ZEBuild.Platform.Platform == "Linux"):
	    ParameterString += " -DCMAKE_TOOLCHAIN_FILE=" + ZEBuild.RootDirectory + "/CMake/toolchain-linux-" + ("x86" if ZEBuild.Platform.Architecture == "x86" else "x64") + ".cmake"
	
	FullSourceDirectory = os.path.normpath(Library.SourceDirectory + "/" + SourceDirectory)
        Arguments = " -G \"" + ZEBuild.Platform.CMakeGenerator + "\" " + ParameterString + " \"" + FullSourceDirectory + "\""

	ZEBuild.SetWorkingDirectory(Library.BuildDirectory)

        ReturnValue = ZEBuild.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake configure failed.")

    @staticmethod
    def CMakeBuild(Library, Configuration):
        ConfigurationText = ""        

	ZEBuild.SetWorkingDirectory(Library.BuildDirectory)

        if (Configuration == ZEBuild.CONFIG_NONE):
            ConfigurationText = ""
	elif (Configuration == ZEBuild.CONFIG_DEBUG):
	    ConfigurationText = "--config Debug"
	elif (Configuration == ZEBuild.CONFIG_RELEASE):
	    ConfigurationText = "--config Release"
	    
	Arguments = " --build \"" + Library.BuildDirectory + "\" " + ConfigurationText
        ReturnValue = ZEBuild.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake build failed.")

    @staticmethod
    def CMakeInstall(Library, Configuration):
        ConfigurationText = ""

	ZEBuild.SetWorkingDirectory(Library.BuildDirectory)

	if (Configuration == ZEBuild.CONFIG_NONE):
            ConfigurationText = ""
	elif (Configuration == ZEBuild.CONFIG_DEBUG):
	    ConfigurationText = "--config Debug"
	elif (Configuration == ZEBuild.CONFIG_RELEASE):
	    ConfigurationText = "--config Release"
	    
        Arguments = " --build \"" + Library.BuildDirectory + "\" --target install " + ConfigurationText

        ReturnValue = ZEBuild.Call("cmake", Arguments)
        if (ReturnValue != 0):
            raise ZEBuildException("CMake install failed.")

    @staticmethod
    def GenerateCMakeList(OutputDirectory, SystemLibs, Combinable):
        BuildDirectory = os.path.normpath(OutputDirectory)
        CMakeList = open(OutputDirectory + "/CMakeList.txt", "w")
        CMakeList.write("cmake_required(VERSION 2.8)\n\n")
        CMakeList.write("ze_register_platform_library(NAME "+ ZEBuild.CurrentLibrary.Name + " " +
                        "\tPLATFORM " + ZEBuild.Platform.Platform + " " +
                        "\tARCHITECTURE " + ZEBuild.Platform.Architecture + " " + 
                        ("\tSYSTEM_LIBS " + SystemLibs) if SystemLibs != "" else "" + " " +
                        ("\tCOMBINABLE)" if Combinable != False else "") + 
	                ")")
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


class ZEBuildDriver:
    @staticmethod
    def ParseArguments():
        ZEBuild.Log("Parsing command line arguments.")
    
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
	ZEBuild.Platform.PlatformString = ZEBuild.Platform.Platform + "-" + ZEBuild.Platform.Architecture

        if (ZEBuild.Platform.CMakeGenerator[:6] == "Visual" or ZEBuild.Platform.CMakeGenerator == "Xcode"):
            ZEBuild.Platform.MultiConfiguration = True
        else:
            ZEBuild.Platform.MultiConfiguration = False
			
	ZEBuild.RootDirectory = ZEBuild.GetWorkingDirectory()
    
    @staticmethod
    def ScanDirectory(Directory):
	ZEBuild.Log("Scanning directory \"" + Directory + "\"")
	if (os.path.isfile(Directory + "/build.py")):
	    ZEBuild.Log("build.py found at \"" + Directory + "\".")
	    ZEBuild.SetWorkingDirectory(Directory)
	    execfile(Directory + "/build.py", globals(), locals())
	    ZEBuild.CurrentLibrary = None
    
    @staticmethod
    def ScanAndBuild():
        DirectoryBase = os.getcwd()
        DirectoryList = os.listdir(os.getcwd() + "/Source")
        
        ZEBuild.Log("Scanning directories...")
        for Directory in DirectoryList:
            CurrentDirectory = DirectoryBase + "/Source/" + Directory
            try:
                if (os.path.isdir(CurrentDirectory)):
		    ZEBuild.ScanDirectory(CurrentDirectory)

            except ZEBuildException as e:
                ZEBuild.Error(e.ErrorText)
            #except Exception as e:
            #    ZEBuild.Error("Unknown Exception occured: " + e.message)

class ZELibrary:
    LogFile = None
    
    def Check(self):
	ZEBuild.Log("Checking...")

    def CleanOutput(self):
	ZEBuild.Log("Cleaning output...")
	ZEBuild.RemoveDirectory(self.OutputDirectory)
	ZEBuild.CreateDirectory(self.OutputDirectory)     
    
    def Clean(self):
        ZEBuild.Log("Cleaning build...")

	ZEBuild.SetWorkingDirectory(self.RootDirectory)
        ZEBuild.RemoveDirectory(self.BuildDirectory)
        ZEBuild.CreateDirectory(self.BuildDirectory)

    def Configure(self, Configuration):
	if (Configuration == ZEBuild.CONFIG_NONE):
	    ZEBuild.Log("Configuring...")
	else:
	    ZEBuild.Log("Configuring... Configuration : " + ("Debug" if Configuration == ZEBuild.CONFIG_DEBUG else "Release"))

    def Compile(self, Configuration):
	if (Configuration == ZEBuild.CONFIG_NONE):
	    ZEBuild.Log("Compiling...")
	else:
	    ZEBuild.Log("Compiling... Configuration : " + ("Debug" if Configuration == ZEBuild.CONFIG_DEBUG else "Release"))

    def Gather(self, Configuration):
	if (Configuration == ZEBuild.CONFIG_NONE):
	    ZEBuild.Log("Gathering outputs...")
	else:
	    ZEBuild.Log("Gathering outputs... Configuration : " + ("Debug" if Configuration == ZEBuild.CONFIG_DEBUG else "Release"))

    def GenerateCMakeList(Configuration):
	if (Configuration == ZEBuild.CONFIG_NONE):
	    ZEBuild.Log("Generating CMakeFiles.txt...")
	else:
	    ZEBuild.Log("Generating CMakeFiles.txt... Configuration : " + ("Debug" if Configuration == ZEBuild.CONFIG_DEBUG else "Release"))


    def Build(self):
        try:
            ZEBuild.CreateDirectory(self.LogDirectory)
            self.LogFile = open(self.LogDirectory + "/Build-" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S") + ".log", "w")

            ZEBuild.Log("Building platform library " + self.Name)          

	    self.Check()
	    self.CleanOutput()

            if (ZEBuild.Platform.MultiConfiguration == False):

		ZEBuild.Configration = ZEBuild.CONFIG_NONE
                
                self.Clean()
                self.Configure(ZEBuild.CONFIG_NONE)
                self.Compile(ZEBuild.CONFIG_NONE)
                self.Gather(ZEBuild.CONFIG_NONE)
            else:
                ZEBuild.Log("Building Debug configuration...")
		
		ZEBuild.Configration = ZEBuild.CONFIG_DEBUG

                self.Clean()
                self.Configure(ZEBuild.CONFIG_DEBUG)
                self.Compile(ZEBuild.CONFIG_DEBUG)
                self.Gather(ZEBuild.CONFIG_DEBUG)

                ZEBuild.Log("Building Release configuration...")
		
		ZEBuild.Configration = ZEBuild.CONFIG_RELEASE
		
		self.Clean()
                self.Configure(ZEBuild.CONFIG_RELEASE)
                self.Compile(ZEBuild.CONFIG_RELEASE)
                self.Gather(ZEBuild.CONFIG_RELEASE)

	    self.GenerateCMakeList()
	    ZEBuild.Log(self.Name + " library has been successfully build.")
	    
        except ZEBuildException as e:
            ZEBuild.Error(e.ErrorText)
            raise ZEBuildException("Building library has failed.")

        finally:
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

class ZEPrebuildLibrary(ZELibrary):
    def Check(self):
        ZELibrary.Check(self)
        DirectoryList = os.listdir(self.SourceDirectory)
        for Directory in DirectoryList:
	    if (os.path.isdir(self.SourceDirectory + "/" + Directory)):
		if (Directory == ZEBuild.Platform.PlatformString):
		    return
        
        raise ZEBuildException("Prebuild library " + self.Name + " does not have " + ZEBuild.Platform.PlatformString + " platform support.")

    def Gather(self, Configuration):
	if (Configuration == ZEBuild.CONFIG_NONE or Configuration == ZEBuild.CONFIG_RELEASE):
	    ZEBuild.CopyDirectory(self.SourceDirectory + "/" + ZEBuild.Platform.PlatformString, self.OutputDirectory)

def Main():
    ZEBuild.Log("ZEBuild System v0.1")
    ZEBuild.Log("Initializing...")

    ZEBuildDriver.ParseArguments()
    #ZEBuildDriver.ScanAndBuild()
    ZEBuildDriver.ScanDirectory(ZEBuild.RootDirectory + "/Source/libCG")


if (__name__ == "__main__"):
    Main()
