import sys
import os
import subprocess
import shutil

from ZELog import *
from ZEBuild import *

class ZEOperations:
    @staticmethod
    def CopyFile(SourcePath, DestinationPath):
        ZELog.DebugLog("Copying file. Source File : \"" + SourcePath + "\". Destination File : \"" + DestinationPath + "\".")
        SourcePath = os.path.normpath(SourcePath)
        DestinationPath = os.path.normpath(DestinationPath)
        shutil.copyfile(SourcePath, DestinationPath)

    @staticmethod
    def RemoveFile(SourcePath):
        ZELog.DebugLog("Deleting file. File : \"" + SourcePath + "\"")
        SourcePath = os.path.normpath(SourcePath)
        os.remove(SourcePath)

    @staticmethod
    def RenameFile(SourcePath, DestinationPath):
        ZELog.DebugLog("Copying file. Source File : \"" + SourcePath + "\". Destination File : \"" + DestinationPath + "\".")
        SourcePath = os.path.normpath(SourcePath)
        DestinationPath = os.path.normpath(DestinationPath)
        os.rename(SourcePath, DestinationPath)

    @staticmethod
    def CreateDirectory(DirectoryName):
        ZELog.DebugLog("Creating directory. Directory Name : \"" + DirectoryName + "\"")
        try:
            DirectoryName = os.path.normpath(DirectoryName)
            if(not os.path.isdir(DirectoryName)):
                os.makedirs(DirectoryName)
        except:
            raise ZEBuildException("Can not create directory. Directory Name : " + DirectoryName)

    @staticmethod
    def CopyDirectory(SourceDirectory, DestinationDirectory):
        ZELog.DebugLog("Copying directory. Source Direcyory : \"" + SourceDirectory + "\". Destination Directory : \"" + DestinationDirectory + "\"")

        try:
            if (not os.path.isdir(SourceDirectory)):
                return

            ZEOperations.CreateDirectory(DestinationDirectory)

            DirectoryList = os.listdir(SourceDirectory)
            for CurrentFile in DirectoryList:

                SourceFile = SourceDirectory + "/" + CurrentFile
                DestinationFile = DestinationDirectory + "/" + CurrentFile

                if (os.path.isdir(SourceFile)):
                    if (CurrentFile == ".svn" or CurrentFile == ".git"):
                        continue

                    ZEOperations.CopyDirectory(SourceFile, DestinationFile)
                else:
                    shutil.copyfile(SourceFile, DestinationFile)
        except:
            raise ZEBuildException("Can not copy directory. Source Directory Name : " + SourceDirectory + ". Destination Directory : " + DestinationDirectory + "\".")

    @staticmethod
    def CopyInstallToOutput(Library, Configuration, IncludeDirectory = None, LibDirectory = None, DLLDirectory = None, BinDirectory = None):
        if (IncludeDirectory != None):
            if (Configuration == ZEBuild.CONFIG_NONE or Configuration == ZEBuild.CONFIG_RELEASE):
                ZEOperations.CopyDirectory(Library.InstallDirectory + "/" + IncludeDirectory, Library.OutputDirectory + "/Include")

        if (LibDirectory != None):
            if (Configuration == ZEBuild.CONFIG_NONE):
                ZEOperations.CopyDirectory(Library.InstallDirectory + "/" + LibDirectory, Library.OutputDirectory + "/Lib")
            else:
                ZEOperations.CopyDirectory(Library.InstallDirectory + "/" + LibDirectory, Library.OutputDirectory + "/Lib/" + ZEBuild.GetConfig())

        if (DLLDirectory != None):
            if (Configuration == ZEBuild.CONFIG_NONE):
                ZEOperations.CopyDirectory(Library.InstallDirectory + "/" + DLLDirectory, Library.OutputDirectory + "/DLL")
            else:
                ZEOperations.CopyDirectory(Library.InstallDirectory + "/" + DLLDirectory, Library.OutputDirectory + "/DLL/" + ZEBuild.GetConfig())

        if (BinDirectory != None):
            if (Configuration == ZEBuild.CONFIG_NONE):
                ZEOperations.CopyDirectory(Library.InstallDirectory + "/" + BinDirectory, Library.OutputDirectory + "/Bin")
            else:
                if (Configuration == ZEBuild.CONFIG_RELEASE):
                    ZEOperations.CopyDirectory(Library.InstallDirectory + "/" + BinDirectory, Library.OutputDirectory + "/Bin")

    @staticmethod   
    def CopyInstallToOutputAuto(Library, Configuration):
        ZEOperations.CopyInstallToOutput(Library, Configuration, "Include", "Lib", "DLL", "Bin")

    @staticmethod
    def RemoveDirectory(DirectoryName):
        ZELog.DebugLog("Removing directory. Directory Name : \"" + DirectoryName + "\"")
        try:
            DirectoryName = os.path.normpath(DirectoryName)
            if(os.path.isdir(DirectoryName)):
                shutil.rmtree(DirectoryName)
        except:
            raise ZEBuildException("Can not remove directory. Directory Name : \"" + DirectoryName + "\".")

    @staticmethod
    def NormalizePath(DirectoryName):
        return os.path.normpath(DirectoryName)

    @staticmethod
    def IsDirectoryExists(DirectoryName):
        if (os.path.exists(ZEOperations.NormalizePath(DirectoryName))):
            return True
        else:
            return False

    @staticmethod
    def IsDirectory(DirectoryName):
        if (ZEOperations.IsDirectoryExists(DirectoryName) and os.path.isdir(ZEOperations.NormalizePath(DirectoryName))):
            return True
        else:
            return False


    @staticmethod
    def SetWorkingDirectory(DirectoryName):
        ZELog.DebugLog("Changing working directory. Directory Name : \"" + DirectoryName + "\".")

        try:
            DirectoryName = os.path.normpath(DirectoryName)
            os.chdir(DirectoryName)
        except:
            raise ZEBuildException("Can not change working directory. Directory Name : \"" + DirectoryName + "\".")

    @staticmethod
    def GetWorkingDirectory():
        return os.path.normpath(os.getcwd())

    @staticmethod
    def Call(Command, Arguments):
        ZELog.DebugLog("Running command. Command : " + Command + ". Arguments : " + Arguments)
        if (ZEBuild.CurrentLibrary != None and ZEBuild.CurrentLibrary.LogFile != None and ZEBuild.CurrentLibrary.LogFile.closed != True):
            ReturnValue = subprocess.call(Command + " " + Arguments, shell=True, stdout = ZEBuild.CurrentLibrary.LogFile, stderr = ZEBuild.CurrentLibrary.LogFile)
            ZEBuild.CurrentLibrary.LogFile.flush()
            return ReturnValue
        else:
            return subprocess.call(Command + " " + Arguments, shell = True)