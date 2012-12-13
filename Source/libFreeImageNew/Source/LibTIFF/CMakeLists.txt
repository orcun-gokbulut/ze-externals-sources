list(APPEND Temp
	LibTIFF/t4.h
	LibTIFF/tiff.h
	LibTIFF/tiffconf.vc.h
	LibTIFF/tiffconf.wince.h
	LibTIFF/tiffio.h
	LibTIFF/tiffio.hxx
	LibTIFF/tiffiop.h
	LibTIFF/tiffvers.h
	LibTIFF/tiffvers.h.in
	LibTIFF/tif_aux.c
	LibTIFF/tif_close.c
	LibTIFF/tif_codec.c
	LibTIFF/tif_color.c
	LibTIFF/tif_compress.c
	LibTIFF/tif_config.h
	LibTIFF/tif_config.vc.h
	LibTIFF/tif_config.wince.h
	LibTIFF/tif_dir.c
	LibTIFF/tif_dir.h
	LibTIFF/tif_dirinfo.c
	LibTIFF/tif_dirread.c
	LibTIFF/tif_dirwrite.c
	LibTIFF/tif_dumpmode.c
	LibTIFF/tif_error.c
	LibTIFF/tif_extension.c
	LibTIFF/tif_fax3.c
	LibTIFF/tif_fax3.h
	LibTIFF/tif_fax3sm.c
	LibTIFF/tif_flush.c
	LibTIFF/tif_getimage.c
	LibTIFF/tif_jbig.c
	LibTIFF/tif_jpeg.c
	LibTIFF/tif_luv.c
	LibTIFF/tif_lzw.c
	LibTIFF/tif_next.c
	LibTIFF/tif_ojpeg.c
	LibTIFF/tif_open.c
	LibTIFF/tif_packbits.c
	LibTIFF/tif_pixarlog.c
	LibTIFF/tif_predict.c
	LibTIFF/tif_predict.h
	LibTIFF/tif_print.c
	LibTIFF/tif_read.c
	LibTIFF/tif_stream.cxx
	LibTIFF/tif_strip.c
	LibTIFF/tif_swab.c
	LibTIFF/tif_thunder.c
	LibTIFF/tif_tile.c
	LibTIFF/tif_version.c
	LibTIFF/tif_warning.c
	LibTIFF/tif_write.c
	LibTIFF/tif_zip.c
	LibTIFF/uvcode.h)

#if (WIN32)
#	list(APPEND Temp LibTIFF/tif_win32.c)
#else(UNIX)
#	list(APPEND Temp LibTIFF/tif_unix.c)
#endif()
	
set(Sources ${Sources} ${Temp} PARENT_SCOPE)