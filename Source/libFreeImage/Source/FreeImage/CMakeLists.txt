list(APPEND Temp
	FreeImage/BitmapAccess.cpp
	FreeImage/CacheFile.cpp
	FreeImage/ColorLookup.cpp
	FreeImage/Conversion.cpp
	FreeImage/Conversion16_555.cpp
	FreeImage/Conversion16_565.cpp
	FreeImage/Conversion24.cpp
	FreeImage/Conversion32.cpp
	FreeImage/Conversion4.cpp
	FreeImage/Conversion8.cpp
	FreeImage/ConversionFloat.cpp
	FreeImage/ConversionRGB16.cpp
	FreeImage/ConversionRGBF.cpp
	FreeImage/ConversionType.cpp
	FreeImage/ConversionUINT16.cpp
	FreeImage/FreeImage.cpp
	FreeImage/FreeImageC.c
	FreeImage/FreeImageIO.cpp
	FreeImage/GetType.cpp
	FreeImage/Halftoning.cpp
	FreeImage/J2KHelper.cpp
	FreeImage/MemoryIO.cpp
	FreeImage/MultiPage.cpp
	FreeImage/NNQuantizer.cpp
	FreeImage/PixelAccess.cpp
	FreeImage/Plugin.cpp
	FreeImage/PluginBMP.cpp
	FreeImage/PluginCUT.cpp
	FreeImage/PluginDDS.cpp
	
	FreeImage/PluginG3.cpp
	FreeImage/PluginGIF.cpp
	FreeImage/PluginHDR.cpp
	FreeImage/PluginICO.cpp
	FreeImage/PluginIFF.cpp
	FreeImage/PluginJ2K.cpp
	FreeImage/PluginJP2.cpp
	FreeImage/PluginJPEG.cpp
	FreeImage/PluginKOALA.cpp
	
	FreeImage/PluginPCD.cpp
	FreeImage/PluginPCX.cpp
	FreeImage/PluginPFM.cpp
	FreeImage/PluginPICT.cpp
	FreeImage/PluginPNG.cpp
	FreeImage/PluginPNM.cpp
	FreeImage/PluginPSD.cpp
	FreeImage/PluginRAS.cpp
	
	FreeImage/PluginSGI.cpp
	FreeImage/PluginTARGA.cpp
	FreeImage/PluginTIFF.cpp
	FreeImage/PluginWBMP.cpp
	FreeImage/PluginXBM.cpp
	FreeImage/PluginXPM.cpp
	FreeImage/PSDParser.cpp
	FreeImage/PSDParser.h
	FreeImage/TIFFLogLuv.cpp
	FreeImage/tmoColorConvert.cpp
	FreeImage/tmoDrago03.cpp
	FreeImage/tmoFattal02.cpp
	FreeImage/tmoReinhard05.cpp
	FreeImage/ToneMapping.cpp
	FreeImage/WuQuantizer.cpp
	FreeImage/ZLibInterface.cpp
	
	FreeImage/Half/eLut.h
	FreeImage/Half/half.cpp
	FreeImage/Half/half.h
	FreeImage/Half/halfFunction.h
	FreeImage/Half/halfLimits.h
	FreeImage/Half/toFloat.h)

set(Sources ${Sources} ${Temp} PARENT_SCOPE)
