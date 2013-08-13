cmake_minimum_required (VERSION 2.8)

project(libISense)

add_definitions(/Z7)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(libISense
	isense.c
	isense.h
	types.h)

set_target_properties(libISense PROPERTIES PREFIX "")

install(TARGETS libISense DESTINATION Lib)
install(FILES isense.h types.h DESTINATION Include/isense)