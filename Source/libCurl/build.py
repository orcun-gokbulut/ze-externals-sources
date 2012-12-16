class libCurl(ZELibrary):   
    def Configure(self, Configuration):
        ZELibrary.Configure(self, Configuration)
        if (ZEPlatform.Platform == "Windows"):
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

            ZECMake.Configure(self, "", Parameters)
        else:
            Parameters = [
                "--enable-shared=no",
                "--enable-static=yes",
                "--enable-symbol-hiding",
                "--disable-debug",
                "--enable-optimize",
                "--disable-warnings",
                "--disable-werror",
                "--disable-curldebug",
                "--enable-symbol-hiding",
                "--disable-ares",
                "--disable-dependency",
                "--enable-http",
                "--enable-ftp",
                "--disable-file",
                "--disable-ldap",
                "--disable-ldaps",
                "--disable-rtsp",
                "--disable-proxy",
                "--disable-dict",
                "--disable-telnet",
                "--enable-tftp",
                "--disable-pop3",
                "--disable-imap",
                "--disable-smtp",
                "--disable-gopher",
                "--disable-manual",
                "--enable-nonblocking"]
            ZEAutoTools.Configure(self, "", Parameters)    

    def Compile(self, Configuration):
        ZELibrary.Compile(self, Configuration)
        if (ZEPlatform.Platform == "Windows"):
            ZECMake.Build(self, Configuration)
        else:
            ZEAutoTools.Build(self, Configuration)

    def Gather(self, Configuration):
        ZELibrary.Gather(self, Configuration)
        if (ZEPlatform.Platform == "Windows"):
            if (Configuration == ZEBuild.CONFIG_NONE):
                ZEOperations.CopyDirectory(self.SourceDirectory + "/include/curl", self.OutputDirectory + "/Include/curl")
                ZEOperations.CopyDirectory(self.BuildDirectory + "/include/curl", self.OutputDirectory + "/Include/curl")
                ZEOperations.CopyFile("/lib/libcurl" + ZEPlatform.LibExtension, self.OutputDirectory + "/Lib/libCurl" + ZEPlatform.LibExtension)
            elif (Configuration == ZEBuild.CONFIG_DEBUG):
                ZEOperations.CreateDirectory(self.OutputDirectory + "/Lib/Debug")
                ZEOperations.CopyFile(self.BuildDirectory + "/lib/Debug/libcurl" + ZEPlatform.LibExtension, self.OutputDirectory + "/Lib/Debug/libCurl" + ZEPlatform.LibExtension)
            elif (Configuration == ZEBuild.CONFIG_RELEASE):
                ZEOperations.CopyDirectory(self.SourceDirectory + "/include/curl", self.OutputDirectory + "/Include/curl")
                ZEOperations.CopyDirectory(self.BuildDirectory + "/include/curl", self.OutputDirectory + "/Include/curl")
                ZEOperations.CreateDirectory(self.OutputDirectory + "/Lib/Release")
                ZEOperations.CopyFile(self.BuildDirectory + "/lib/Release/libcurl" + ZEPlatform.LibExtension, self.OutputDirectory + "/Lib/Release/libCurl" + ZEPlatform.LibExtension)
        else:
            ZEAutoTools.Install(self, Configuration)
            ZEOperations.CopyInstallToOutput(self, Configuration, "include", "lib")
    
ZEBuildDriver.BuildLibrary(libCurl("libCurl", ""))