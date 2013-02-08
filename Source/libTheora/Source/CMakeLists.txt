cmake_minimum_required (VERSION 2.8)

project(libTheora)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(libTheora STATIC
	analyze.c
	apiwrapper.c
	bitpack.c
	cpu.c
	decapiwrapper.c
	decinfo.c
	decode.c
	dequant.c
	encapiwrapper.c
	encfrag.c
	encinfo.c
	encode.c
	encoder_disabled.c
	enquant.c
	fdct.c
	fragment.c
	huffdec.c
	huffenc.c
	idct.c
	info.c
	internal.c
	mathops.c
	mcenc.c
	quant.c
	rate.c
	state.c
	tokenize.c
	x86/mmxencfrag.c
	x86/mmxfdct.c
	x86/mmxfrag.c
	x86/mmxidct.c
	x86/mmxstate.c
	x86/sse2fdct.c
	x86/x86enc.c
	x86/x86state.c
	x86_vc/mmxencfrag.c
	x86_vc/mmxfdct.c
	x86_vc/mmxfrag.c
	x86_vc/mmxidct.c
	x86_vc/mmxstate.c
	x86_vc/x86enc.c
	x86_vc/x86state.c)

set_target_properties(libTheora PROPERTIES PREFIX "")

install(TARGETS libTheora DESTINATION Lib)
install(FILES include/theora/codec.h include/theora/theora.h include/theora/theoraenc.h include/theora/theoradec.h DESTINATION Include/theora)

