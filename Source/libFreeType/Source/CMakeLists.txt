cmake_minimum_required(VERSION 2.8)

project(libFreeType2)

add_definitions(-DFT2_BUILD_LIBRARY)
add_definitions("-DFT_CONFIG_MODULES_H=<ftmodule.h>")

include_directories("${PROJECT_SOURCE_DIR}/objs")
include_directories("${PROJECT_SOURCE_DIR}/builds/ansi")

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include/freetype")
include_directories("${PROJECT_SOURCE_DIR}/include/freetype/config")
include_directories("${PROJECT_BINARY_DIR}/include")

set(BASE_SRCS
    src/base/ftsystem.c
    src/base/ftdebug.c
    src/base/ftinit.c
    src/base/ftbbox.c
    src/base/ftbitmap.c
    src/base/ftcid.c
    src/base/ftadvanc.c
    src/base/ftcalc.c
    src/base/ftdbgmem.c
    src/base/ftgloadr.c
    src/base/ftobjs.c
    src/base/ftoutln.c
    src/base/ftrfork.c
    src/base/ftsnames.c
    src/base/ftstream.c
    src/base/fttrigon.c
    src/base/ftutil.c
    src/base/ftfstype.c
    src/base/ftgasp.c
    src/base/ftglyph.c
    src/base/ftgxval.c
    src/base/ftlcdfil.c
    src/base/ftmm.c
    src/base/ftotval.c
    src/base/ftpatent.c
    src/base/ftpfr.c
    src/base/ftstroke.c
    src/base/ftsynth.c
    src/base/fttype1.c
    src/base/ftwinfnt.c
    src/base/ftxf86.c
    src/truetype/truetype.c
    src/type1/type1.c
    src/cff/cff.c
    src/cid/type1cid.c
    src/pfr/pfr.c
    src/type42/type42.c
    src/winfonts/winfnt.c
    src/pcf/pcf.c
    src/bdf/bdf.c
    src/sfnt/sfnt.c
    src/autofit/autofit.c
    src/pshinter/pshinter.c
    src/raster/raster.c
    src/smooth/smooth.c
    src/cache/ftcache.c
    src/gzip/ftgzip.c
    src/lzw/ftlzw.c
    src/bzip2/ftbzip2.c
    src/psaux/psaux.c
    src/psnames/psmodule.c)

include_directories("src/truetype")
include_directories("src/sfnt")
include_directories("src/autofit")
include_directories("src/smooth")
include_directories("src/raster")
include_directories("src/psaux")
include_directories("src/psnames")

add_definitions(/Z7)

add_library(libFreeType STATIC ${BASE_SRCS})
set_target_properties(libFreeType PROPERTIES PREFIX "")

install(TARGETS libFreeType DESTINATION Lib)
install(DIRECTORY include/ DESTINATION Include)
