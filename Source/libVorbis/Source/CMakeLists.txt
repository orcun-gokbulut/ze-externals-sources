cmake_minimum_required (VERSION 2.8)

project(libVorbis)

add_definitions(/Z7)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include)

add_definitions(-D_USE_MATH_DEFINES)

add_library(libVorbis
	analysis.c
	backends.h
	barkmel.c
	bitrate.c
	bitrate.h
	block.c
	codebook.c
	codebook.h
	codec_internal.h
	envelope.c
	envelope.h
	floor0.c
	floor1.c
	highlevel.h
	info.c
	lookup.c
	lookup.h
	lookup_data.h
	lpc.c
	lpc.h
	lsp.c
	lsp.h
	mapping0.c
	masking.h
	mdct.c
	mdct.h
	misc.h
	os.h
	psy.c
	psy.h
	registry.c
	registry.h
	res0.c
	scales.h
	sharedbook.c
	smallft.c
	smallft.h
	synthesis.c
	tone.c
	vorbisenc.c
	vorbisfile.c
	window.c
	window.h)
	
set_target_properties(libVorbis PROPERTIES PREFIX "")

install(TARGETS libVorbis DESTINATION Lib)
install(FILES include/vorbis/codec.h include/vorbis/vorbisenc.h include/vorbis/vorbisfile.h DESTINATION Include/vorbis)