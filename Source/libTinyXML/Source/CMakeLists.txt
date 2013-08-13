cmake_minimum_required(VERSION 2.8)

project(libTinyXml)

set(Sources
	tinystr.cpp
	tinystr.h
	tinyxml.cpp
	tinyxml.h
	tinyxmlerror.cpp
	tinyxmlparser.cpp)

add_definitions(/Z7)

add_library(libTinyXML STATIC ${Sources})

set_target_properties(libTinyXML PROPERTIES PREFIX "")

install(TARGETS libTinyXML DESTINATION Lib)
install(FILES tinystr.h tinyxml.h DESTINATION Include)