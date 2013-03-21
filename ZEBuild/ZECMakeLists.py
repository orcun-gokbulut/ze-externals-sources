from os.path import *
from ZEBuild import *

class ZELibraryCMakeLists:
    @staticmethod
    def GetPlatformCheckParameters():
        if (ZEPlatform.Platform == "iOS"):
            return "PLATFORMS iOS ARCHITECTURES ArmV7"
        elif (ZEPlatform.Platform == "iOS-Simulator"):
            return "PLATFORMS iOS-Simulator ARCHITECTURES x86"
        else:
            return "PLATFORMS " + ZEPlatform.Platform + " ARCHITECTURES " + ZEPlatform.Architecture
        
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
        
        self.File.write("ze_external_register_library(" + Name + Parameters + "\n\t" + ZELibraryCMakeLists.GetPlatformCheckParameters() + ")\n\n")
        self.File.flush()
    
    def End(self):
        self.File.close()