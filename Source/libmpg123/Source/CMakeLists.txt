cmake_minimum_required(VERSION 2.8)

project(libmpg123)

set(Sources
	compat.c
	dct64.c
	equalizer.c
	feature.c
	format.c
	frame.c
	icy.c
	icy2utf8.c
	id3.c
	index.c
	layer1.c
	layer2.c
	layer3.c
	libmpg123.c
	ntom.c
	optimize.c
	parse.c
	readers.c
	stringbuf.c
	synth.c
	synth_8bit.c
	synth_real.c
	synth_s32.c
	tabinit.c)

if (WIN32)
	set(AssemblySources
		dct36_3dnow.S
		dct36_3dnowext.S
		dct64_3dnow.S
		dct64_3dnowext.S
		dct64_mmx.S
		dct64_sse.S
		dct64_sse_float.S
		equalizer_3dnow.S
		getcpuflags.S
		synth_3dnow.S
		synth_3dnowext.S
		synth_i586.S
		synth_mmx.S
		synth_sse.S
		synth_sse_accurate.S
		synth_sse_float.S
		synth_sse_s32.S
		synth_stereo_sse_accurate.S
		synth_stereo_sse_float.S
		synth_stereo_sse_s32.S
		tabinit_mmx.S)
	set(Sources ${Sources} dct64_i386.c)
elseif (ARM)
	set (AssemblySources
		dct36_3dnow.S
		dct64_neon.S
		dct64_neon_float.S
		synth_arm.S
		synth_arm_accurate.S
		synth_neon.S
		synth_neon_accurate.S
		synth_neon_float.S
		synth_neon_s32.S
		synth_stereo_neon.S
		synth_stereo_neon_accurate.S
		synth_stereo_neon_float.S
		synth_stereo_neon_s32.S)
elseif (POWERPC)
	set(Sources ${Sources} dct64_altivec.c synth_altivec.c)
endif()


include_directories(${CMAKE_SOURCE_DIR})
if (WIN32)
	include_directories(${CMAKE_SOURCE_DIR}/platform/windows)
	set(Sources ${Sources} platform/windows/msvc.c)
	add_definitions(
		-D_CRT_SECURE_NO_WARNINGS
		-DBUILD_MPG123_DLL
		-DOPT_MULTI
		-DOPT_GENERIC
		-DOPT_I386
		-DOPT_I586
		-DOPT_MMX
		-DOPT_3DNOW
		-DOPT_3DNOWEXT
		-DOPT_SSE)
endif()

set(Sources ${Sources} ${AssemblySources})
add_library(libmpg123 SHARED ${Sources})

if (WIN32)
	foreach(ASSEMBLY ${AssemblySources})
		add_custom_command(
			TARGET libmpg123
			PRE_BUILD
			COMMAND "${CMAKE_C_COMPILER}"
			ARGS "/I \"${CMAKE_SOURCE_DIR}/platform/windows\" /I \"${CMAKE_CURRENT_SOURCE_DIR}\" /EP /TC \"${CMAKE_CURRENT_SOURCE_DIR}/${ASSEMBLY}\" /nologo > \"${CMAKE_CURRENT_BINARY_DIR}/${ASSEMBLY}.asm\""
			COMMAND "${CMAKE_SOURCE_DIR}/../Tools/yasm.exe" 
			ARGS "-a x86 -p gas -r raw -f win32 -g null -m x86 -o \"${CMAKE_CURRENT_BINARY_DIR}/${ASSEMBLY}.o\" \"${CMAKE_CURRENT_BINARY_DIR}/${ASSEMBLY}.asm\"")
		set(AssemblyBinaries "${AssemblyBinaries} \"${CMAKE_CURRENT_BINARY_DIR}/${ASSEMBLY}.o\"")
	endforeach()
	set_property(TARGET libmpg123 APPEND PROPERTY LINK_FLAGS ${AssemblyBinaries})
endif()

if (WIN32)
	install(TARGETS libmpg123 RUNTIME DESTINATION DLL)
	install(TARGETS libmpg123 ARCHIVE DESTINATION Lib)
	install(FILES makefile.h.in platform/windows/mpg123.h platform/windows/config.h DESTINATION Include)
else()
	install(TARGETS libmpg123 LIBRARY DESTINATION DLL)
	install(FILES mpg123.h DESTINATION Include)
endif()

