cmake_minimum_required (VERSION 2.8)

project(libOGG)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(libOGG
	bitwise.c
	framing.c
	include/ogg/ogg.h
	include/ogg/os_types.h)

set_target_properties(libOGG PROPERTIES PREFIX "")

install(TARGETS libOGG DESTINATION Lib)
install(FILES include/ogg/ogg.h include/ogg/os_types.h DESTINATION Include/ogg)