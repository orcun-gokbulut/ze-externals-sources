import os, sys, optparse, subprocess, distutils

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
    
    @staticmethod
    def CreateDirectory(DirectoryName):
        try:
            DirectoryName = os.path.normpath(DirectoryName)
            if(not os.path.isdir(DirectoryName)):
                os.makedirs(os.path.normpath(DirectoryName))
        except:
            raise ZEBuildException("Can not create directory. Directory Name : " + DirectoryName)
    
    @staticmethod
    def CopyDirectory(SourceDirectory, DestinationDirectory):
        try:
            SourceDirectory = os.path.normpath(SourceDirectory)
            DestinationDirectory = os.path.normpath(DestinationDirectory)
            distutils.dir_util.copy_tree(os.path.normpath(SourceDirectory), DestinationDirectory)
        except:
            raise ZEBuildException("Can not copy directory. Source Directory Name : " + SourceDirectory + ", Destination Directory : " + DestinationDirectory)
        
    @staticmethod
    def RemoveDirectory(DirectoryName):
        #try:
            DirectoryName = os.path.normpath(DirectoryName)
            if(os.path.isdir(DirectoryName)):
               os.removedirs(DirectoryName)
        #except:
        #    raise ZEBuildException("Can not remove directory. Directory Name : " + DirectoryName)

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
        print(ZEBuild.OutputStr(Level, Text))

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
    def CMake(Library, Parameters):
        for Parameter in Parameters:
            ParameterString += "-D" + Parameter.Name + ":" + Parameter.Type + "=" + Parameter.Value + " "
            
        ReturnValue = subprocess.call("cmake -G \"" + ZEBuild.CMakeGeneratorName + "\"" + ParameterString + " \"" + Library.SourceDirectory + "\"")
        if (ReturnValue != 0):
            raise ZEBuildException(Library, "CMake configure failed.")
    
    @staticmethod
    def CMakeBuild(Library, Debug):
        ReturnValue = subprocess.call("cmake --build \"" + Library.BinaryDirectory + "\"" + " --config Debug" if Debug else "")
        if (ReturnValue != 0):
            raise ZEBuildException(Library, "CMake build failed.")
  
    @staticmethod
    def CMakeInstall(Library, Debug):
        ReturnValue = subprocess.call("cmake --build \"" + Library.BinaryDirectory + "\" --target install " + " --config Debug" if Debug else "")
        if (ReturnValue != 0):
            raise ZEBuildException(Library, "CMake build failed.")    

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
    def Clean(self):
        ZEBuild.Log("Cleaning.")

        ZEBuild.RemoveDirectory(self.OutputDirectory)
        ZEBuild.CreateDirectory(self.OutputDirectory)
        
        ZEBuild.RemoveDirectory(self.LogDirectory)
        ZEBuild.CreateDirectory(self.LogDirectory)      

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
            self.Clean()
            self.Configure(False)
            self.Compile(False)
            self.Gather(False)

            self.GenerateCMakeList()
        
    def __init__(self, Name, ExtraLibraries):
        self.Name = Name
        self.RootDirectory = os.getcwd();
        self.OutputDirectory = self.RootDirectory + "/Output"
        self.LogDirectory = self.RootDirectory  + "/Log"
        self.BuildDirectory = self.RootDirectory  + "/Build"
        self.SourceDirectory = self.RootDirectory  + "/Source"
        self.ExtraLibraries = ExtraLibraries
        
  
def Main():
    ZEBuild.Log("ZEBuild System v0.1")    
    ZEBuild.Log("Initializing...")
    parser = optparse.OptionParser()
    parser.add_option("-p", "--platform", dest="platform", action="store", type="string",
                      help="Platform name (Windows, Linux, Unix, MacOS, PS3, xBox360, iPhone, iPhoneSimulator, Android)")
    parser.add_option("-a", "--architecture", dest="architecture", action="store", type="string",
                      help="Architecture name (x86, x64, PPC, ARM)")
    parser.add_option("-c", "--cmake_generator", dest="cmake_generator", action="store", type="string",
                      help="CMake generator name. (Visual Studio 20XX, NMake, Unix Makefiles, XCode)")
    (Options, args) = parser.parse_args()
    
    ZEBuild.Platform = ZEPlatform()
    ZEBuild.Platform.Platform = Options.platform
    ZEBuild.Platform.Architecture = Options.architecture
    ZEBuild.Platform.CMakeGenerator = Options.cmake_generator
    
    if (ZEBuild.Platform.CMakeGenerator[:2] == "VS" or ZEBuild.Platform.CMakeGenerator == "XCode"):
        ZEBuild.Platform.MultiConfiguration = True
    else:
        ZEBuild.Platform.MultiConfiguration = False
    
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
    
    
if __name__ == '__main__':
    Main()