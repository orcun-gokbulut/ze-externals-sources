from os.path import *
from ZEBuild import *

class ZERootCMakeLists:   
    @staticmethod
    def GetPlatformCheckParameters():
        if (ZEPlatform.Platform == "iOS"):
            return "PLATFORM iOS ARCHITECTURE ArmV7"
        elif (ZEPlatform.Platform == "iOS-Simulator"):
            return "PLATFORM iOS-Simulator ARCHITECTURE x86"
        else:
            return "PLATFORM " + ZEPlatform.Platform + " ARCHITECTURE " + ZEPlatform.Architecture
            
    @staticmethod
    def Begin():
        ZERootCMakeLists.File = open(ZEBuild.OutputDirectory + "/CMakeLists.txt", mode="w")
        ZERootCMakeLists.File.write("cmake_minimum_required (VERSION 2.8)\n\n");
        ZERootCMakeLists.File.write("ze_externals_initialize(" + ZERootCMakeLists.GetPlatformCheckParameters() + ")\n\n");

    @staticmethod 
    def AddLibrary(Library):
        ZERootCMakeLists.File.write("ze_externals_add_sub_directory(" + os.path.relpath(Library.OutputDirectory, start = ZEBuild.OutputDirectory) + ")\n")
    
    @staticmethod 
    def End():
        ZERootCMakeLists.File.close()

class ZELibraryCMakeLists:   
    def Begin(self, Library):
        self.Library = Library
        self.File = open(self.Library.OutputDirectory + "/CMakeLists.txt", mode="w")
        self.File.write("cmake_minimum_required (VERSION 2.8)\n\n")
        
        
    def AddSubLibrary(self, Libs = None, DLLs = None, IncludeFolder = None , LibFolder = None, DLLFolder = None, NonCombinable = False, SystemLibs = None):
        Parameters = ""
        
        if (Libs != None):
            Parameters += " LIBS " + Libs
        
        if (DLLs != None):
            Parameter += " DLLS " + DLLs
        
        if (SystemLibs != None):
            Parameters += " SYSTEM_LIBS " + SystemLibs
        
        if (IncludeFolder != None):
            Parameters += " LIB_FOLDER " + IncludeFolder

        if (Libs != None):
            Parameters += " INCLUDE_FOLDER " + Libs

        if (DLLFolder != None):
            Parameters += " DLL_FOLDER " + DLLFolder

        if (NonCombinable):
            Parameters += " NON_COMBINABLE"
        
        self.File.write("ze_externals_register_library(" + self.Library.Name + Parameters + " " + ZERootCMakeLists.GetPlatformCheckParameters() + ")\n")
    
    def End(self):
        self.File.close()
        ZERootCMakeLists.AddLibrary(self.Library)