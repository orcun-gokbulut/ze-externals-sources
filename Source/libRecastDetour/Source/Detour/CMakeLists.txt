cmake_minimum_required (VERSION 2.8)

project(libDetour)

set(Sources
	DetourAlloc.cpp
	DetourCommon.cpp
	DetourCrowd.cpp
	DetourLocalBoundary.cpp
	DetourNavMesh.cpp
	DetourNavMeshBuilder.cpp
	DetourNavMeshQuery.cpp
	DetourNode.cpp
	DetourObstacleAvoidance.cpp
	DetourPathCorridor.cpp
	DetourPathQueue.cpp
	DetourProximityGrid.cpp
	DetourTileCache.cpp
	DetourTileCacheBuilder.cpp)
	
set(Headers
	DetourAlloc.h
	DetourAssert.h
	DetourCommon.h
	DetourCrowd.h
	DetourLocalBoundary.h
	DetourMath.h
	DetourNavMesh.h
	DetourNavMeshBuilder.h
	DetourNavMeshQuery.h
	DetourNode.h
	DetourObstacleAvoidance.h
	DetourPathCorridor.h
	DetourPathQueue.h
	DetourProximityGrid.h
	DetourStatus.h
	DetourTileCache.h
	DetourTileCacheBuilder.h)
	
add_library(libDetour ${Sources} ${Headers})

set_target_properties(libDetour PROPERTIES PREFIX "")

install(TARGETS libDetour DESTINATION Lib)
install(FILES ${Headers} DESTINATION Include/Detour)