:Initialization
@echo off
set ProjectName=libATICompress
@echo [ZEBuild Externals] Info : Starting to build external %ProjectName%.
 
:CopyOutput
xcopy /r /y /e Output ..\..\Output\

:End
@echo [ZEBuild Externals] Success : %ProjectName% build external is completed successfully.
exit /b 0

:Error
@echo [ZEBuild Externals] Error : Error occured while building %ProjectName%. 
exit /b 1