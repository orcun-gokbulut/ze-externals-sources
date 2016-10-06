cmake_minimum_required (VERSION 2.8)

project(libDebugUtils)

include_directories(../Recast ../Detour)

set(Sources
	DebugDraw.cpp
	DetourDebugDraw.cpp
	RecastDebugDraw.cpp
	RecastDump.cpp)
	
set(Headers
	DebugDraw.h
	DetourDebugDraw.h
	RecastDebugDraw.h
	RecastDump.h)
	
add_library(libDebugUtils ${Sources} ${Headers})

set_target_properties(libDebugUtils PROPERTIES PREFIX "")

install(TARGETS libDebugUtils DESTINATION Lib)
install(FILES ${Headers} DESTINATION Include/DebugUtils)