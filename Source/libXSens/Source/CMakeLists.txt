cmake_minimum_required (VERSION 2.8)

project(libXSens)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(libXSens
	cmt1.cpp
	cmt1.h
	cmt2.cpp
	cmt2.h
	cmt3.cpp
	cmt3.h
	cmtdef.h	
	cmtmessage.cpp
	cmtmessage.h
	cmtpacket.cpp
	cmtpacket.h
	cmtscan.cpp
	cmtscan.h
	pstdint.h
	xsens_exception.cpp
	xsens_exception.h
	xsens_fifoqueue.h
	xsens_file.h
	xsens_janitors.h
	xsens_list.h
	xsens_std.cpp
	xsens_std.h
	xsens_time.cpp
	xsens_time.h
)

set_target_properties(libXSens PROPERTIES PREFIX "")

install(TARGETS libXSens DESTINATION Lib)
install(
	FILES cmt1.h
		cmt2.h
		cmt3.h
		cmtdef.h	
		cmtmessage.h
		cmtpacket.h
		cmtscan.h
		pstdint.h
		xsens_exception.h
		xsens_fifoqueue.h
		xsens_file.h
		xsens_janitors.h
		xsens_list.h
		xsens_std.h
		xsens_time.h
	DESTINATION Include/xsens)