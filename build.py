import sys
from ZEBuild.ZELog import *
from ZEBuild.ZEBuildDriver import *

def Main():
    ZELog.Log("ZEBuild System v0.1")
    ZELog.Log("Initializing...")

    ZEBuildDriver.ParseArguments()
    #ZEBuildDriver.ScanAndBuild()
    ZEBuildDriver.ScanDirectory(ZEBuild.RootDirectory + "/Source/libFreeType")

if (__name__ == "__main__"):
    Main()
