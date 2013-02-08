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
        ZERootCMakeLists.File.write("ze_external_initialize(" + ZERootCMakeLists.GetPlatformCheckParameters() + ")\n\n");

    @staticmethod 
    def AddLibrary(Library):
        ZERootCMakeLists.File.write("add_subdirectory(" + os.path.relpath(Library.OutputDirectory, start = ZEBuild.OutputDirectory) + ")\n")
        ZERootCMakeLists.File.flush()
    
    @staticmethod 
    def End():
        ZERootCMakeLists.File.close()

class ZELibraryCMakeLists:   
    def Begin(self, Library):
        self.Library = Library
        self.File = open(self.Library.OutputDirectory + "/CMakeLists.txt", mode="w")
        self.File.write("cmake_minimum_required (VERSION 2.8)\n\n")
        
        
    def AddSubLibrary(self, Name, Libs = None, DLLs = None, IncludeFolder = None , LibFolder = None, DLLFolder = None, NonCombinable = False, SystemLibs = None):
        Parameters = ""
        
        if (Libs != None):
            Parameters += "\n\tLIBS " + Libs
        
        if (DLLs != None):
            Parameters += "\n\tDLLS " + DLLs
        
        if (SystemLibs != None):
            Parameters += "\n\tSYSTEM_LIBS " + SystemLibs
        
        if (IncludeFolder != None):
            Parameters += "\n\tLIB_FOLDER " + IncludeFolder

        if (LibFolder != None):
            Parameters += "\n\tINCLUDE_FOLDER " + LibFolder

        if (DLLFolder != None):
            Parameters += "\n\tDLL_FOLDER " + DLLFolder

        if (NonCombinable):
            Parameters += "\n\tNON_COMBINABLE"
        
        self.File.write("ze_external_register_library(" + Name + Parameters + "\n\t" + ZERootCMakeLists.GetPlatformCheckParameters() + ")\n\n")
        self.File.flush()
    
    def End(self):
        self.File.close()
        ZERootCMakeLists.AddLibrary(self.Library)