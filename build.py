import sys
from ZEBuild.ZELog import *
from ZEBuild.ZEBuildDriver import *
from ZEBuild.ZECMakeLists import *

def Main():
    ZELog.Log("ZEBuild System v0.1")
    ZELog.Log("Initializing...")

    
    ZEBuildDriver.ParseArguments()
    ZEOperations.CreateDirectory(ZEBuild.OutputDirectory)

    ZERootCMakeLists.Begin()
    ZEBuildDriver.ScanAndBuild()
    ZERootCMakeLists.End()

if (__name__ == "__main__"):
    Main()
