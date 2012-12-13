cmake_minimum_required(VERSION 2.8)

project(libFreeImage)

add_definitions(-DOPJ_STATIC
	-DFREEIMAGE_LIB)

set(Sources
	CacheFile.h
	FreeImage.h
	FreeImageIO.h
	Plugin.h
	Quantizers.h
	ToneMapping.h
	Utilities.h)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/FreeImage 
	${CMAKE_SOURCE_DIR}/LibJPEG 
	${CMAKE_SOURCE_DIR}/LibOpenJPEG 
	${CMAKE_SOURCE_DIR}/LibPNG 
	${CMAKE_SOURCE_DIR}/LibTIFF
	${CMAKE_SOURCE_DIR}/ZLib)

add_subdirectory(FreeImage)
add_subdirectory(LibJPEG)
add_subdirectory(LibOpenJPEG)
add_subdirectory(LibPNG)
add_subdirectory(LibTIFF)
add_subdirectory(Metadata)
add_subdirectory(ZLib)
add_subdirectory(FreeImageToolkit)

add_library(libFreeImage ${Sources})

install(TARGETS libFreeImage DESTINATION Lib)
install(FILES Freeimage.h DESTINATION Include)