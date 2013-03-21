cmake_minimum_required(VERSION 2.8)

project (libGLEW)

include_directories(include)
add_definitions(-DGLEW_STATIC)

add_library(libGLEW 
	src/glew.c)

install(TARGETS libGLEW DESTINATION Lib)
install(FILES include/GL/glew.h include/GL/glxew.h include/GL/wglew.h DESTINATION Include/GL)
