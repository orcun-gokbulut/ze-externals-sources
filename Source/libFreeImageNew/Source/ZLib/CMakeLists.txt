list(APPEND Temp
	ZLib/adler32.c
	ZLib/compress.c
	ZLib/crc32.c
	ZLib/crc32.h
	ZLib/deflate.c
	ZLib/deflate.h
	ZLib/gzclose.c
	ZLib/gzguts.h
	ZLib/gzlib.c
	ZLib/gzread.c
	ZLib/gzwrite.c
	ZLib/infback.c
	ZLib/inffast.c
	ZLib/inffast.h
	ZLib/inffixed.h
	ZLib/inflate.c
	ZLib/inflate.h
	ZLib/inftrees.c
	ZLib/inftrees.h
	ZLib/trees.c
	ZLib/trees.h
	ZLib/uncompr.c
	ZLib/zconf.h
	ZLib/zlib.h
	ZLib/zutil.c
	ZLib/zutil.h)
	
set(Sources ${Sources} ${Temp} PARENT_SCOPE)