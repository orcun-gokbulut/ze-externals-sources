class libCurl(ZELibrary):
    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_BUILD_TYPE", "STRING", "Debug" if Debug else "Release"),
                         ZECMakeParameter("CURL_STATICLIB", "BOOL", "YES"),
                         ZECMakeParameter("BUILD_CURL_EXE", "BOOL", "NO"),
                         ZECMakeParameter("BUILD_CURL_TESTS", "BOOL", "NO")]
        else:
            Parameter = None
        ZEBuild.SetWorkingDirectory(ZEBuild.CurrentLibrary.BuildDirectory)
        ZEBuild.CMake(self.SourceDirectory, Parameter)

    def Compile(self, Debug):
        ZELibrary.Compile(self, Debug)
        ZEBuild.CMakeBuild(self.BuildDirectory, Debug)

    def Gather(self, Debug):
        ZELibrary.Gather(self, Debug)
        if (Debug != None):
            if not ZEBuild.IsDirectoryExists(self.OutputDirectory + "/Include"):
                ZEBuild.CopyDirectory(self.SourceDirectory + "/include/curl/", self.OutputDirectory + "/Include")
                ZEBuild.CopyDirectory(self.BuildDirectory + "/include/curl/", self.OutputDirectory + "/Include/curl")

            if ZEBuild.Platform.MultiConfiguration:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                FileSource = self.BuildDirectory + "/lib" + ("/Debug" if Debug else "/Release") + ("/libcurl.lib" if ZEBuild.Platform.Platform == "Windows" else "/libcurl.a")
                FileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libCurl.lib" if ZEBuild.Platform.Platform == "Windows" else "/libCurl.a")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                FileSource = self.BuildDirectory + "/lib/libcurl.a"
                FileDestination = self.OutputDirectory + "/Lib/libCurl.a"

            ZEBuild.CopyFile(FileSource, FileDestination)


ZEBuild.BuildLibrary(libCurl("libCurl", ""))