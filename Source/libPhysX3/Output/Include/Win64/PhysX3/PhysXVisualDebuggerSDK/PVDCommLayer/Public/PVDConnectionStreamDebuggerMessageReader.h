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
#ifndef PVD_PVDCONNECTIONSTREAMDEBUGGERMESSAGEREADER_H
#define PVD_PVDCONNECTIONSTREAMDEBUGGERMESSAGEREADER_H

#include "PVDCommByteStream.h"
#include "PvdDataStreamEventInStream.h"

namespace PVD
{
	template<typename TRetType, typename TOperator, typename TReaderType>
	struct PvdConnectionStreamDebuggerMessageReaderOp
	{
		TReaderType* mReader;
		TOperator mOperator;
		PvdConnectionStreamDebuggerMessageReaderOp( TReaderType* inReader, TOperator inOperator )
			: mReader( inReader )
			, mOperator( inOperator ) 
		{}
		template<typename TDataType>
		inline TRetType operator()( const TDataType& inEvent ) 
		{ 
			TRetType marker;
			return mReader->deserialize(marker, inEvent, mOperator );
		}
		inline TRetType operator()() { return mOperator(); }
	};

	template<typename TEventInStreamType
			, typename TDeleteOperator=SDeleteOperator>
	class PvdConnectionStreamDebuggerMessageReader
	{
		typedef PvdConnectionStreamDebuggerMessageReader<TEventInStreamType,TDeleteOperator> TThisType;
		TEventInStreamType* mInStream;
		PxU32 mEventCount;
		PxU32 mEventReadSize;
		PxU32 mEventSizeCheck;

		PvdDebugMessageType mEventType;

		PvdConnectionStreamDebuggerMessageReader( const PvdConnectionStreamDebuggerMessageReader& inOther );
		PvdConnectionStreamDebuggerMessageReader& operator=( const PvdConnectionStreamDebuggerMessageReader& inOther );
	public:
		PvdConnectionStreamDebuggerMessageReader()
			: mEventCount( 0 )
			, mEventType( PvdDebugMessageType::Unknown )
			, mEventReadSize( 0 )
			, mEventSizeCheck( 0 )
			, mInStream( NULL )
		{}

		inline void setup( const SEventHeader& inHeader,  TEventInStreamType* inStream )
		{
			mEventReadSize = 0;
			mEventSizeCheck = inHeader.mSize;
			mInStream = inStream;
			if ( inHeader.mEventType == PvdDebugMessageType::EventBatch )
			{
				mInStream->streamify( mEventCount );
				mEventReadSize += 4;
				mEventType = PvdDebugMessageType::EventBatch;
			}
		}
		
		void destroy() { TDeleteOperator()(this); }
		
		template<typename TRetType, typename TOperator>
		inline TRetType readNextBatchEvent(TOperator inOperator)
		{
			TRetType retval = TRetType();
			if ( mEventCount )
			{
				--mEventCount;
				PvdDebugMessageType theEventType;
				mInStream->streamify( theEventType.mType );	
				mEventReadSize += 1;
				retval = deserializeNextEvent<TRetType>( theEventType, inOperator );
			}
			return retval;
		}

		template<typename TRetType, typename TOperator>
		inline TRetType deserializeNextEvent( PvdDebugMessageType inType, TOperator inOperator )
		{
			PxU32 originalSize = mInStream->amountLeft();
			typedef PvdConnectionStreamDebuggerMessageReaderOp<TRetType, TOperator, TThisType > TReaderOpType;
			TReaderOpType theOperator( this, inOperator );
			TRetType retval( visitDebugMessage<TRetType>( inType, theOperator));
			PxU32 finalSize = mInStream->amountLeft();
			mEventReadSize += originalSize - finalSize;
			return retval;
		}

		template<typename TRetType, typename TDataType, typename TOperator>
		inline TRetType deserialize(const TRetType&, const TDataType&, TOperator inOperator)
		{
			TDataType theTemp;
			theTemp.streamify( *mInStream );
			return inOperator( theTemp );
		}
	};
}

#endif