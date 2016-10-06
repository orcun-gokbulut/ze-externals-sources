cmake_minimum_required (VERSION 2.8)

project(libRecast)

set(Sources
	Recast.cpp
	RecastAlloc.cpp
	RecastArea.cpp
	RecastContour.cpp
	RecastFilter.cpp
	RecastLayers.cpp
	RecastMesh.cpp
	RecastMeshDetail.cpp
	RecastRasterization.cpp
	RecastRegion.cpp)
	
set(Headers
	Recast.h
	RecastAssert.h
	RecastAlloc.h)
	
add_library(libRecast ${Sources} ${Headers})

set_target_properties(libRecast PROPERTIES PREFIX "")

install(TARGETS libRecast DESTINATION Lib)
install(FILES ${Headers} DESTINATION Include/Recast)