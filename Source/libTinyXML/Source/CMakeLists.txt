cmake_minimum_required(VERSION 2.6)

project(TinyXml)

set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include_directories(./)

file(
	GLOB_RECURSE
	source_files
	*.h
	*.cpp
)

add_library(
	tinyxml
	STATIC
	${source_files}
)
