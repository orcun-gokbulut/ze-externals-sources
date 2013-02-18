class libDirectX(ZEPrebuildLibrary):
   def GenerateCMakeList(self):
      ZELog.Log("Generating CMakeFiles.txt...")
      Script = ZELibraryCMakeLists()
      Script.Begin(self)
      Script.AddSubLibrary("libDirectSound", "dsound dxguid")
      Script.AddSubLibrary("libDirect3DCompiler", "d3dcompiler")
      Script.AddSubLibrary("libDirect3D9", "d3d9 d3dx9")
      Script.AddSubLibrary("libDirect3D10", "d3d10 d3dx10")
      Script.AddSubLibrary("libDirect3D101", "d3d10_1 d3dx10_1")
      Script.AddSubLibrary("libDirect3D11", "d3d11 d3dx11")
      Script.AddSubLibrary("libDirectInput", "dinput")      
      Script.End()

if (ZEPlatform.Platform == "Windows"):
   ZEBuildDriver.BuildLibrary(libDirectX("libDirectX", ""))