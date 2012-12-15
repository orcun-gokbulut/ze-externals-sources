class libCurl(ZELibrary):
    def Configure(self, Debug):
        ZELibrary.Configure(self, Configuration)
        Parameters = [ZECMakeParameter("CURL_STATICLIB", "BOOL", "YES"),
                     ZECMakeParameter("BUILD_CURL_EXE", "BOOL", "NO"),
                     ZECMakeParameter("BUILD_CURL_TESTS", "BOOL", "NO"),
                     ZECMakeParameter("CURL_DISABLE_LDAP", "BOOL", "YES"),
                     ZECMakeParameter("CURL_LDAP_WIN", "BOOL", "YES"),
                     ZECMakeParameter("CURL_DISABLE_HTTP", "BOOL", "NO"),
                     ZECMakeParameter("CURL_DISABLE_HTTPS", "BOOL", "NO"),
                     ZECMakeParameter("CURL_DISABLE_FTP", "BOOL", "NO"),
                     ZECMakeParameter("CURL_DISABLE_FILE", "BOOL", "YES"),
                     ZECMakeParameter("CURL_DISABLE_CRYPTO_AUTH", "BOOL", "YES"),
                     ZECMakeParameter("CURL_DISABLE_TELNET", "BOOL", "YES"),
                     ZECMakeParameter("CURL_DISABLE_DICT", "BOOL", "YES"),
                     ZECMakeParameter("CURL_DISABLE_LDAP", "BOOL", "YES"),
                     ZECMakeParameter("CURL_DISABLE_LDAPS", "BOOL", "YES")]

        ZEBuild.CMake(self, "", Parameters)

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        ZEBuild.CMakeBuild(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration
        ZEBuild.CMakeInstall(self, Configuration)
        
        if (Configuration != ZEBuild.CONFIG_NONE):
            ZEBuild.CopyDirectory(self.SourceDirectory + "/include/curl/", self.OutputDirectory + "/Include")
            ZEBuild.CopyDirectory(self.BuildDirectory + "/include/curl/", self.OutputDirectory + "/Include")

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