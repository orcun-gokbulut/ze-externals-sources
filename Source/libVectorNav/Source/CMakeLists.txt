cmake_minimum_required (VERSION 2.8)

project(libVectorNav)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(Sources vn100.c vn200.c)
	
if (WIN32)
	set(Sources ${Sources} arch/win32/vncp_services.c)
else()
	set(Sources ${Sources} arch/linux/vncp_services.c)
endif()

add_definitions(/Z7)

add_library(libVectorNav ${Sources})

set_target_properties(libVectorNav PROPERTIES PREFIX "")

install(TARGETS libVectorNav DESTINATION Lib)
install(
	FILES
		include/vectornav.h
		include/vn100.h
		include/vn200.h
		include/vncp_services.h
		include/vn_common.h
		include/vn_errorCodes.h
		include/vn_kinematics.h
		include/vn_linearAlgebra.h
		include/vn_math.h
	DESTINATION Include)