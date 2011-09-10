// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2011 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  
#ifndef REPX_STRINGTOTYPE_H
#define REPX_STRINGTOTYPE_H
#include <stdio.h>
#include <ctype.h>
#include "PsString.h"
#include "PxCoreUtilityTypes.h"
#include "PxFiltering.h"

//Remapping function name for gcc-based systems.
#ifndef _MSC_VER
#define _strtoui64 strtoull
#endif


namespace physx { namespace repx {
	
	//Id's (void ptrs) are written to file as unsigned
	//64 bit integers, so this method gets called more
	//often than one might think.
	inline void strto( PxU64& ioDatatype, char*& ioData )
	{
		ioDatatype = _strtoui64( ioData, &ioData, 10 );
	}

	inline void strto( PxU32& ioDatatype, char*& ioData )
	{
		ioDatatype = static_cast<PxU32>( strtoul( ioData, &ioData, 10 ) );
	}

	inline void strto( PxU16& ioDatatype, char*& ioData )
	{
		ioDatatype = static_cast<PxU16>( strtoul( ioData, &ioData, 10 ) );
	}

	inline void eatwhite( char*& ioData )
	{
		if ( ioData )
		{
			while( isspace( *ioData ) )
				++ioData;
		}
	}

	inline void nullTerminateWhite( char*& ioData )
	{
		if ( ioData )
		{
			while( *ioData && !isspace( *ioData ) )
				++ioData;
			if ( *ioData )
			{
				ioData[0] = 0;
				++ioData;
			}
		}
	}
	
	inline void strto( PxF32& ioDatatype, char*& ioData )
	{
		//strtod on windows is fatally flawed.  It calls strlen on
		//the buffer.  Thus if you have a large buffer of space-delimited
		//strings you are taking an n! hit trying to parse it.  On top of
		//just parsing it.

		//So, we have to read in the string.  If it null-terminates, we are fine.
		//if we hit a whitespace, we null terminate it.

		//Hence the char data is not const; this is a destructive parse.

		//Eat the whitespace.
		//This only works for space-delimited number strings.  ANything
		//else will fail
		eatwhite( ioData );
		char* target = ioData;
		nullTerminateWhite( ioData );
		ioDatatype = static_cast<PxF32>( strtod( target, NULL ) );
	}

	inline void strto( void*& ioDatatype, char*& ioData )
	{
		PxU64 theData;
		strto( theData, ioData );
		ioDatatype = reinterpret_cast<void*>( theData );
	}
	
	inline void strto( physx::pubfnd3::PxVec3& ioDatatype, char*& ioData )
	{
		strto( ioDatatype[0], ioData );
		strto( ioDatatype[1], ioData );
		strto( ioDatatype[2], ioData );
	}

	PX_INLINE void stringToType( const char* inValue, PxReal& ioType)
	{
		ioType = static_cast<PxReal>( strtod( inValue, NULL ) );
	}

	PX_INLINE void stringToType( const char* inValue, PxU32& ioType)
	{
		ioType = static_cast<PxU32>( strtoul( inValue, NULL, 10 ) );
	}

	
	PX_INLINE void stringToType( const char* inValue, PxU16& ioType)
	{
		ioType = static_cast<PxU16>( strtoul( inValue, NULL, 10 ) );
	}
	
	PX_INLINE void stringToType( const char* inValue, PxU8& ioType)
	{
		ioType = static_cast<PxU8>( strtoul( inValue, NULL, 10 ) );
	}
	
	inline void strto( PxU8* ioDatatype, char*& ioData )
	{
	}

	PX_INLINE void stringToType( const char* inValue, bool& ioType)
	{
		ioType = physx::string::stricmp( inValue, "true" ) == 0 ? true : false;
	}

	PX_INLINE void stringToType( const char* inValue, PxFilterData& ioType)
	{
		char* thePtr( const_cast<char*>( inValue ) );
		ioType.word0 = static_cast<PxU32>( strtoul( thePtr, &thePtr, 10 ) );
		ioType.word1 = static_cast<PxU32>( strtoul( thePtr, &thePtr, 10 ) );
		ioType.word2 = static_cast<PxU32>( strtoul( thePtr, &thePtr, 10 ) );
		ioType.word3 = static_cast<PxU32>( strtoul( thePtr, NULL, 10 ) );
	}

	PX_INLINE void stringToType( const char* inValue, PxTypedStridedData<PxMaterialTableIndex>& inPtr )
	{
	}

	PX_INLINE void stringToType( const char* inValue, void*& inPtr )
	{
		char* theData = const_cast<char*>( inValue );
		strto( inPtr, theData );
	}

	PX_INLINE void stringToType( const char* inValue, PxVec3& ioType)
	{
		if ( inValue && *inValue )
		{
			char* nextPtr;
			ioType[0] = static_cast<PxReal>( strtod( inValue, &nextPtr ) );
			ioType[1] = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
			ioType[2] = static_cast<PxReal>( strtod( nextPtr, NULL ) );
		}
	}
	PX_INLINE void stringToType( const char* inValue, PxQuat& ioType )
	{
		char* nextPtr;
		ioType.x = static_cast<PxReal>( strtod( inValue, &nextPtr ) );
		ioType.y = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
		ioType.z = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
		ioType.w = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
	}

	PX_INLINE void stringToType( const char* inValue, PxTransform& ioType)
	{
		if ( inValue && *inValue )
		{
			char* nextPtr;
			ioType.q.x = static_cast<PxReal>( strtod( inValue, &nextPtr ) );
			ioType.q.y = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
			ioType.q.z = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
			ioType.q.w = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );

			ioType.p[0] = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
			ioType.p[1] = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
			ioType.p[2] = static_cast<PxReal>( strtod( nextPtr, NULL ) );
		}
	}
	PX_INLINE void stringToType( const char* inValue, PxBounds3& ioType)
	{
		if ( inValue && *inValue )
		{
			char* nextPtr;
			ioType.minimum[0] = static_cast<PxReal>( strtod( inValue, &nextPtr ) );
			ioType.minimum[1] = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
			ioType.minimum[2] = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );

			ioType.maximum[0] = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
			ioType.maximum[1] = static_cast<PxReal>( strtod( nextPtr, &nextPtr ) );
			ioType.maximum[2] = static_cast<PxReal>( strtod( nextPtr, NULL ) );
		}
	}
}}

#endif