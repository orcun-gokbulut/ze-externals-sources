import os
import optparse
from ZELog import *

class ZEBuildException:
    def __init__(self, ErrorText):
        self.ErrorText = ErrorText

    def __str__(self):
        return ZELog.OutputStr("Error", ErrorText)

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
    def GetConfig():
        if (ZEBuild.Configration == ZEBuild.CONFIG_NONE):
            return ""
        elif (ZEBuild.Configration == ZEBuild.CONFIG_DEBUG):
            return "Debug"
        elif (ZEBuild.Configration == ZEBuild.CONFIG_RELEASE):
            return "Release"
        else:
            raise ZEBuildException("Unknown configuration. Configuration : " + ZEBuild.Configration)