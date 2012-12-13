list(APPEND Temp
	Freeimage/BitmapAccess.cpp
	Freeimage/CacheFile.cpp
	Freeimage/ColorLookup.cpp
	Freeimage/Conversion.cpp
	Freeimage/Conversion16_555.cpp
	Freeimage/Conversion16_565.cpp
	Freeimage/Conversion24.cpp
	Freeimage/Conversion32.cpp
	Freeimage/Conversion4.cpp
	Freeimage/Conversion8.cpp
	Freeimage/ConversionFloat.cpp
	Freeimage/ConversionRGB16.cpp
	Freeimage/ConversionRGBF.cpp
	Freeimage/ConversionType.cpp
	Freeimage/ConversionUINT16.cpp
	Freeimage/FreeImage.cpp
	Freeimage/FreeImageC.c
	Freeimage/FreeImageIO.cpp
	Freeimage/GetType.cpp
	Freeimage/Halftoning.cpp
	Freeimage/J2KHelper.cpp
	Freeimage/MemoryIO.cpp
	Freeimage/MultiPage.cpp
	Freeimage/NNQuantizer.cpp
	Freeimage/PixelAccess.cpp
	Freeimage/Plugin.cpp
	Freeimage/PluginBMP.cpp
	Freeimage/PluginCUT.cpp
	Freeimage/PluginDDS.cpp
	
	Freeimage/PluginG3.cpp
	Freeimage/PluginGIF.cpp
	Freeimage/PluginHDR.cpp
	Freeimage/PluginICO.cpp
	Freeimage/PluginIFF.cpp
	Freeimage/PluginJ2K.cpp
	Freeimage/PluginJP2.cpp
	Freeimage/PluginJPEG.cpp
	Freeimage/PluginKOALA.cpp
	
	Freeimage/PluginPCD.cpp
	Freeimage/PluginPCX.cpp
	Freeimage/PluginPFM.cpp
	Freeimage/PluginPICT.cpp
	Freeimage/PluginPNG.cpp
	Freeimage/PluginPNM.cpp
	Freeimage/PluginPSD.cpp
	Freeimage/PluginRAS.cpp
	
	Freeimage/PluginSGI.cpp
	Freeimage/PluginTARGA.cpp
	Freeimage/PluginTIFF.cpp
	Freeimage/PluginWBMP.cpp
	Freeimage/PluginXBM.cpp
	Freeimage/PluginXPM.cpp
	Freeimage/PSDParser.cpp
	Freeimage/PSDParser.h
	Freeimage/TIFFLogLuv.cpp
	Freeimage/tmoColorConvert.cpp
	Freeimage/tmoDrago03.cpp
	Freeimage/tmoFattal02.cpp
	Freeimage/tmoReinhard05.cpp
	Freeimage/ToneMapping.cpp
	Freeimage/WuQuantizer.cpp
	Freeimage/ZLibInterface.cpp
	
	Freeimage/Half/eLut.h
	Freeimage/Half/half.cpp
	Freeimage/Half/half.h
	Freeimage/Half/halfFunction.h
	Freeimage/Half/halfLimits.h
	Freeimage/Half/toFloat.h)

set(Sources ${Sources} ${Temp} PARENT_SCOPE)