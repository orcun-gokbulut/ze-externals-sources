class libOGGVorbis(ZELibrary):
    def Configure(self, Debug):
        ZELibrary.Configure(self, Debug)
        if (Debug != None):
            Parameter = [ZECMakeParameter("CMAKE_INSTALL_PREFIX", "PATH", self.BuildDirectory if Debug else "Release")]
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
                ZEBuild.CopyDirectory(self.SourceDirectory + "/OGG/ogg", self.OutputDirectory + "/Include/ogg")
                ZEBuild.CopyDirectory(self.SourceDirectory + "/Vorbis/vorbis", self.OutputDirectory + "/Include/vorbis")
                
            if ZEBuild.Platform.MultiConfiguration:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release"))

                OggFileSource = self.BuildDirectory + "/OGG" + ("/Debug" if Debug else "/Release") + ("/libOGG.lib" if ZEBuild.Platform.Platform == "Windows" else "/liblibOGG.a")
                OggFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libOGG.lib" if ZEBuild.Platform.Platform == "Windows" else "/libOGG.a")
                
                VorbisFileSource = self.BuildDirectory + "/Vorbis" + ("/Debug" if Debug else "/Release") + ("/libVorbis.lib" if ZEBuild.Platform.Platform == "Windows" else "/liblibVorbis.a")
                VorbisFileDestination = self.OutputDirectory + "/Lib" + ("/Debug" if Debug else "/Release") + ("/libVorbis.lib" if ZEBuild.Platform.Platform == "Windows" else "/libVorbis.a")
            else:
                ZEBuild.CreateDirectory(self.OutputDirectory + "/Lib")

                OggFileSource = self.BuildDirectory + "/OGG/liblibOGG.a"
                OggFileDestination = self.OutputDirectory + "/Lib/libOGG.a"
                
                VorbisFileSource = self.BuildDirectory + "/Vorbis/liblibVorbis.a"
                VorbisFileDestination = self.OutputDirectory + "/Lib/libVorbis.a"

            ZEBuild.CopyFile(OggFileSource, OggFileDestination)
            ZEBuild.CopyFile(VorbisFileSource, VorbisFileDestination)

ZEBuild.BuildLibrary(libOGGVorbis("libOGGVorbis", ""))
