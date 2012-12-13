list(APPEND Temp
	Metadata/Exif.cpp
	Metadata/FIRational.cpp
	Metadata/FIRational.h
	Metadata/FreeImageTag.cpp
	Metadata/FreeImageTag.h
	Metadata/IPTC.cpp
	Metadata/TagConversion.cpp
	Metadata/TagLib.cpp
	Metadata/XTIFF.cpp)
	
set(Sources ${Sources} ${Temp} PARENT_SCOPE)