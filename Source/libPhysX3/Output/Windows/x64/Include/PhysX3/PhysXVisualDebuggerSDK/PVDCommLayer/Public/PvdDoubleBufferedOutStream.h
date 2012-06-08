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
#ifndef PVD_DOUBLE_BUFFERED_OUT_STREAM_H
#define PVD_DOUBLE_BUFFERED_OUT_STREAM_H
#include "PVDCommByteStream.h"
#include "PvdDoubleBufferedWriteBuffer.h"
#include "PsThread.h"

namespace PVD
{

	class DoubleBufferedOutStream : public PvdCommOutStream, public Thread
	{
		typedef PvdDoubleBufferedWriteBuffer<PxU8, ReflectionAllocator<DoubleBufferedOutStream> > TBufferType;
		typedef TBufferType::ReadResult TReadResult;

		TBufferType			mBuffer;
		PvdCommOutStream*	mOutStream;

	public:
		DoubleBufferedOutStream( PxU32 inCapacity, PvdCommOutStream* inOutStream )
			: mBuffer( inCapacity )
			, mOutStream( inOutStream )
		{
			//16K of stack space.  I have no idea how little I could allocate
			//at this point.
			start( 0x4000 ); 
		}
		virtual ~DoubleBufferedOutStream()
		{
			mOutStream->disconnect();
			waitForQuit();
			PvdCommOutStream* tempOutStream = mOutStream;
			mOutStream = NULL;
			tempOutStream->destroy();
		}

		virtual PvdCommLayerError write( const PxU8* inBytes, PxU32 inLength )
		{
			if ( mOutStream == NULL )
				return PvdCommLayerError::NetworkError;

			bool connected = mOutStream->isConnected();
			while( inLength && connected )
			{
				PxU32 written = mBuffer.write( inBytes, inLength );
				inLength -= written;
				if ( inLength )
				{
					inBytes += written;
					mBuffer.waitTillReadyToWrite(1);
					connected = mOutStream->isConnected();
				}
			}
			return connected ? PvdCommLayerError( PvdCommLayerError::None ) : PvdCommLayerError( PvdCommLayerError::NetworkError );
		}
		
		/**
		 *	Return true if this stream is still connected.
		 */
		virtual bool isConnected() const { if ( mOutStream ) return mOutStream->isConnected();  return false; }
		/**
		 *	Close the in stream.
		 */
		virtual void disconnect() 
		{ 
			if ( mOutStream )
				mOutStream->disconnect(); 
		}
		/**
		 *	release any resources related to this stream.
		 */
		virtual void destroy()
		{
			PX_DELETE( this );
		}
		
		//The thread execute fn.
		virtual void execute()
		{
			while( mOutStream->isConnected() )
			{
				TReadResult theResult = mBuffer.read();
				if ( theResult.mLen )
					mOutStream->write( theResult.mPtr, theResult.mLen );
				else
					mBuffer.waitTillReadyToRead( 1 );
			}
			quit();
		}
	};
}
#endif