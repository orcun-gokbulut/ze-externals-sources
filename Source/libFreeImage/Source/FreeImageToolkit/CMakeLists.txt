list(APPEND Temp
	FreeImageToolkit/Background.cpp
	FreeImageToolkit/BSplineRotate.cpp
	FreeImageToolkit/Channels.cpp
	FreeImageToolkit/ClassicRotate.cpp
	FreeImageToolkit/CMakeLists.txt
	FreeImageToolkit/Colors.cpp
	FreeImageToolkit/CopyPaste.cpp
	FreeImageToolkit/Display.cpp
	FreeImageToolkit/Filters.h
	FreeImageToolkit/Flip.cpp
	FreeImageToolkit/JPEGTransform.cpp
	FreeImageToolkit/MultigridPoissonSolver.cpp
	FreeImageToolkit/Rescale.cpp
	FreeImageToolkit/Resize.cpp
	FreeImageToolkit/Resize.h)
	
set(Sources ${Sources} ${Temp} PARENT_SCOPE)