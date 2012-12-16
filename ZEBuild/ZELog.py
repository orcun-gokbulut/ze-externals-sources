import sys
import datetime
from ZEBuild import *

class ZELog:
    @staticmethod  
    def OutputStr(Level, Text):
        if (ZEBuild.CurrentLibrary != None):
            return "[" + Level + "] " + ZEBuild.CurrentLibrary.Name + " : " + Text
        else:
            return "[" + Level + "] ZEBuild : " + Text

    @staticmethod
    def Output(Level, Text):
	OutputText = ZELog.OutputStr(Level, Text)
	
	if (ZEBuild.CurrentLibrary != None and ZEBuild.CurrentLibrary.LogFile != None and ZEBuild.CurrentLibrary.LogFile.closed == False):
            ZEBuild.CurrentLibrary.LogFile.write(OutputText + "\n")
            ZEBuild.CurrentLibrary.LogFile.flush()

	if (Level != "Debug"):
	    print(OutputText)

    @staticmethod
    def Error(Text):
        ZELog.Output("Error", Text)

    @staticmethod
    def Warning(Text):
        ZELog.Output("Warning", Text)

    @staticmethod
    def Log(Text):
        ZELog.Output("Log", Text)

    @staticmethod
    def DebugLog(Text):
	ZELog.Output("Debug", Text)