list(APPEND Temp
	LibPNG/png.c
	LibPNG/png.h
	LibPNG/pngconf.h
	LibPNG/pngdebug.h
	LibPNG/pngerror.c
	LibPNG/pngget.c
	LibPNG/pnginfo.h
	LibPNG/pnglibconf.h
	LibPNG/pngmem.c
	LibPNG/pngpread.c
	LibPNG/pngpriv.h
	LibPNG/pngread.c
	LibPNG/pngrio.c
	LibPNG/pngrtran.c
	LibPNG/pngrutil.c
	LibPNG/pngset.c
	LibPNG/pngstruct.h
	LibPNG/pngtrans.c
	LibPNG/pngwio.c
	LibPNG/pngwrite.c
	LibPNG/pngwtran.c
	LibPNG/pngwutil.c)
	
set(Sources ${Sources} ${Temp} PARENT_SCOPE)