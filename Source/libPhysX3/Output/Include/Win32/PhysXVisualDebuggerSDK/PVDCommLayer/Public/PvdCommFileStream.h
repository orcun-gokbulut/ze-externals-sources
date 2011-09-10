// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
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
// Copyright (c) 2009 NVIDIA Corporation. All rights reserved.
#ifndef PVD_PVDCOMMFILESTREAM_H
#define PVD_PVDCOMMFILESTREAM_H
#include "PVDCommByteStream.h"
#include "PsUserAllocated.h"
#include <stdio.h>

#pragma warning (disable:4503)

namespace PVD
{
	class FStreamCommOutStream : public PvdCommOutStream, public UserAllocated
	{
		FILE* mFile;
		volatile bool mConnected;

	public:

		virtual ~FStreamCommOutStream() {}


		FStreamCommOutStream()
			: mFile( NULL )
		{
		}

		bool open( const char* inFname )
		{
			mFile = fopen( inFname, "wb" );
			if ( mFile != NULL )
			{
				mConnected = true;
				return true;
			}
			return false;
		}
		virtual PvdCommLayerError write( const PxU8* inBytes, PxU32 inLength )
		{
			if ( inBytes && inLength && mConnected )
				fwrite( inBytes, 1, inLength, mFile );
			return PvdCommLayerError::None;
		}
		virtual PvdCommLayerError flush() { fflush( mFile ); return PvdCommLayerError::None; }
		virtual bool isConnected() const { return mConnected; }
		virtual void disconnect() { mConnected = false; }
		virtual void destroy() { if ( mFile ) fclose( mFile ); mFile = NULL; PX_DELETE( this ); }
	};
}

#endif