cmake_minimum_required(VERSION 2.8)

project (libLua)

if (UNIX)
    set (LUA_DIRSEP "/")
else()
    set (LUA_DIRSEP "\\\\")
endif()

configure_file ( src/luaconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/luaconf.h )

include_directories(
    src
    ${CMAKE_CURRENT_BINARY_DIR})

set(Sources 
    src/lapi.c
    src/lcode.c
    src/lctype.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c 
    src/lfunc.c
    src/lgc.c
    src/llex.c
    src/lmem.c
    src/lobject.c
    src/lopcodes.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/ltable.c
    src/ltm.c
    src/lundump.c
    src/lvm.c
    src/lzio.c
    src/lauxlib.c
    src/lbaselib.c
    src/lbitlib.c
    src/lcorolib.c
    src/ldblib.c
    src/liolib.c
    src/lmathlib.c
    src/loslib.c
    src/lstrlib.c
    src/ltablib.c
    src/linit.c
    src/loadlib.c)

add_library(libLua ${Sources})

set_target_properties (libLua PROPERTIES OUTPUT_NAME lua CLEAN_DIRECT_OUTPUT 1)

install(TARGETS libLua DESTINATION Lib)
install(FILES src/lua.h src/lualib.h src/lauxlib.h ${CMAKE_CURRENT_BINARY_DIR}/luaconf.h DESTINATION Include)
