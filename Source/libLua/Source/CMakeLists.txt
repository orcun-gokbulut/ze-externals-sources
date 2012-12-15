cmake_minimum_required(VERSION 2.8)

project (libLua)

include_directories(
    src
    ${CMAKE_CURRENT_BINARY_DIR})

if (WIN32)

elseif (APPLE)
	add_definitions(-DLUA_USE_MACOSX)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions("-DLUA_USE_LINUX")
elseif (UNIX)
	add_definitions(-DLUA_USE_POSIX)
endif()

set(Sources 
	src/lapi.c
	src/lapi.h
	src/lauxlib.c
	src/lauxlib.h
	src/lbaselib.c
	src/lbitlib.c
	src/lcode.c
	src/lcode.h
	src/lcorolib.c
	src/lctype.c
	src/lctype.h
	src/ldblib.c
	src/ldebug.c
	src/ldebug.h
	src/ldo.c
	src/ldo.h
	src/ldump.c
	src/lfunc.c
	src/lfunc.h
	src/lgc.c
	src/lgc.h
	src/linit.c
	src/liolib.c
	src/llex.c
	src/llex.h
	src/llimits.h
	src/lmathlib.c
	src/lmem.c
	src/lmem.h
	src/loadlib.c
	src/lobject.c
	src/lobject.h
	src/lopcodes.c
	src/lopcodes.h
	src/loslib.c
	src/lparser.c
	src/lparser.h
	src/lstate.c
	src/lstate.h
	src/lstring.c
	src/lstring.h
	src/lstrlib.c
	src/ltable.c
	src/ltable.h
	src/ltablib.c
	src/ltm.c
	src/ltm.h
	src/lua.c
	src/lua.h
	src/lua.hpp
	src/luaconf.h
	src/lualib.h
	src/lundump.c
	src/lundump.h
	src/lvm.c
	src/lvm.h
	src/lzio.c
	src/lzio.h)

add_library(libLua ${Sources})

set_target_properties (libLua PROPERTIES OUTPUT_NAME lua CLEAN_DIRECT_OUTPUT 1)

install(TARGETS libLua DESTINATION Lib)
install(FILES src/lua.h src/lualib.h src/lauxlib.h src/luaconf.h DESTINATION Include)
