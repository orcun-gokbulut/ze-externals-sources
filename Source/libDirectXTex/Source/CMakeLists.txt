cmake_minimum_required(VERSION 2.8.10)
project(libDirectXTex)

set(Sources 
	BC.h 
	BC.cpp 
	BCDirectCompute.h
	BCDirectCompute.cpp 
	DDS.h 
	DirectXTex.h 
	DirectXTexP.h
	Filters.h 
	scoped.h 
	DirectXTex.inl 
	BC4BC5.cpp 
	BC6HBC7.cpp 
	DirectXTexCompress.cpp 
	DirectXTexCompressGPU.cpp 
	DirectXTexConvert.cpp 
	DirectXTexD3D11.cpp 
	DirectXTexDDS.cpp 
	DirectXTexFlipRotate.cpp 
	DirectXTexImage.cpp 
	DirectXTexMipmaps.cpp 
	DirectXTexMisc.cpp 
	DirectXTexNormalMaps.cpp 
	DirectXTexPMAlpha.cpp 
	DirectXTexResize.cpp 
	DirectXTexTGA.cpp 
	DirectXTexUtil.cpp 
	DirectXTexWIC.cpp)

include_directories(
	"C:/Program Files (x86)/Windows Kits/8.1/Include/shared"
	"C:/Program Files (x86)/Windows Kits/8.1/Include/um"
	"C:/Program Files (x86)/Windows Kits/8.1/Include/winrt")
	
link_directories("C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x64")

add_library(libDirectXTex ${Sources})
install(TARGETS libDirectXTex DESTINATION Lib)
install(FILES DirectXTex.h DirectXTex.inl  DESTINATION Include)