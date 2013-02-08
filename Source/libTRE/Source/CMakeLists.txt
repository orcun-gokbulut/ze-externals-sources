cmake_minimum_required (VERSION 2.8)

project(libTRE)

set(Sources
	config.h
	regcomp.c
	regerror.c
	regex.h 
	regexec.c
	tre-ast.c
	tre-ast.h
	tre-compile.c
	tre-compile.h
	tre-config.h
	tre-config.h.in
	tre-internal.h
	tre-match-approx.c
	tre-match-backtrack.c
	tre-match-parallel.c
	tre-match-utils.h
	tre-mem.c
	tre-mem.h
	tre-parse.c
	tre-parse.h
	tre-stack.c
	tre-stack.h
	tre.h
	xmalloc.c
	xmalloc.h)

include_directories(${CMAKE_SOURCE_DIR})
add_definitions(-DHAVE_CONFIG_H)

add_library(libTRE ${Sources})
set_target_properties(libTRE PROPERTIES PREFIX "")

install(TARGETS libTRE DESTINATION Lib)
install(FILES tre.h tre-config.h DESTINATION Include)