class libVRPN(ZELibrary):
    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_BUILD_TYPE", "STRING", "Debug" if Debug else "Release"),
                         ZECMakeParameter("BUILD_TESTING", "BOOL", "NO"),
                         ZECMakeParameter("VRPN_BUILD_JAVA", "BOOL", "NO"),
                         ZECMakeParameter("VRPN_BUILD_PYTHON", "BOOL", "NO"),
                         ZECMakeParameter("VRPN_BUILD_CLIENT_LIBRARY", "BOOL", "YES"),
                         ZECMakeParameter("VRPN_BUILD_CLIENTS", "BOOL", "NO"),
                         ZECMakeParameter("VRPN_BUILD_SERVER_LIBRARY", "BOOL", "NO"),
                         ZECMakeParameter("VRPN_BUILD_SERVERS", "BOOL", "NO")]
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
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Include")
                ZEBuild.CopyFile(self.BuildDirectory + "/vrpn_Configure.h", self.OutputDirectory + "/Include/vrpn_Configure.h")
            
            if ZEBuild.Platform.MultiConfiguration:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                VprnFileSource = self.BuildDirectory + ("/Debug" if Debug else "/Release") + ("/vrpn.lib" if ZEBuild.Platform.Platform == "Windows" else "/libvrpn.a")
                VprnFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libVrpn.lib" if ZEBuild.Platform.Platform == "Windows" else "/libVrpn.a")
                
                QuatFileSource = self.BuildDirectory + "/quat" + ("/Debug" if Debug else "/Release") + ("/quat.lib" if ZEBuild.Platform.Platform == "Windows" else "/libquat.a")
                QuatFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libQuat.lib" if ZEBuild.Platform.Platform == "Windows" else "/libQuat.a")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                VprnFileSource = self.BuildDirectory + "/libvrpn.a"
                VprnFileDestination = self.OutputDirectory + "/Lib/libVrpn.a"
                
                QuatFileSource = self.BuildDirectory + "/quat/libquat.a"
                QuatFileDestination = self.OutputDirectory + "/Lib/libQuat.a"

            ZEBuild.CopyFile(VprnFileSource, VprnFileDestination)
            ZEBuild.CopyFile(QuatFileSource, QuatFileDestination)

ZEBuild.BuildLibrary(libVRPN("libVRPN", ""))