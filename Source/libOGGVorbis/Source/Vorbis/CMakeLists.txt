cmake_minimum_required (VERSION 2.8)

include_directories(../OGG)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(libVorbis
	analysis.c
	backends.h
	barkmel.c
	bitrate.c
	bitrate.h
	bitwise.c
	block.c
	books
	CMakeLists.txt
	codebook.c
	codebook.h
	codec_internal.h
	envelope.c
	envelope.h
	floor0.c
	floor1.c
	framing.c
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
	modes
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
	vorbisenc.c
	vorbisfile.c
	window.c
	window.h)

install(TARGETS libVorbis DESTINATION Lib)
install(FILES vorbis/codec.h vorbis/vorbisenc.h vorbis/vorbisfile.h DESTINATION Include/vorbis)