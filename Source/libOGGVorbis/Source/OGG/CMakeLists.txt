cmake_minimum_required (VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(libOGG
	bitwise.c
	framing.c
	ogg/ogg.h
	ogg/os_types.h)

install(TARGETS libOGG DESTINATION Lib)
install(FILES ogg/ogg.h ogg/os_types.h DESTINATION Include/ogg)