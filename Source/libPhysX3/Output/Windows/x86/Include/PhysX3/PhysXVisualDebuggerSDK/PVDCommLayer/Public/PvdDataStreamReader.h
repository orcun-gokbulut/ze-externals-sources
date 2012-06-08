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
#ifndef PVD_PVDCONNECTIONSTREAMREADER_H
#define PVD_PVDCONNECTIONSTREAMREADER_H

#include "PVDConnectionEvents.h"
#include "PvdDataStreamEventInStream.h"

namespace PVD
{
	template<typename TReaderType>
	struct PvdConnectionStreamReaderOp
	{
		TReaderType* mReader;
		PvdConnectionStreamReaderOp( TReaderType* inReader )
			: mReader( inReader ) {}
		template<typename TDataType>
		inline PvdConnectionEventData operator()( const TDataType& inEvent ) { return mReader->operator()( inEvent ); }
		inline PvdConnectionEventData operator()() { return PvdConnectionEventData(); }
	};

	/**
	 *	This class is not an exact analogue of the stream writer
	 *	due to the way it is intended to be used.  Currently PVD2 
	 *	reads data from a socket in another thread than the one that
	 *	does the stream parsing.  So the thread that does the stream
	 *	parsing always has just data pointers; it never actually
	 *	reads from the network.  Also, because they have been
	 *	read from the network already this class doesn't need to
	 *	worry about an event that is only partially sent; that would
	 *	be caught and taken care of by the network thread.
	 *	
	 *	This class must be parameterized based on if at least
	 *	if the stream is big or little endian.
	 */
	template<typename TEventInStreamType
			, typename TAllocator=ClientAllocator<char>
			, typename TDeleteOperator=SDeleteOperator>
	struct PvdDataStreamReaderImpl
	{
		
		typedef AbstractInStreamDatatypeHandler<TEventInStreamType> TDatatypeHandler;

		//Data needed for the multiple object sets.
		physx::shdfnd::Array<PvdCommLayerDatatype,TAllocator >	mDatatypes;
		physx::shdfnd::Array<TDatatypeHandler*,TAllocator >	mDatatypeHandlers;

		PxU32								mPropertyCount;

		//Data needed meta event types
		PvdConnectionEventType				mEventType;
		PxU32								mEventCount;
		PvdConnectionEventType				mEventSubtype;
		PxU32								mEventSizeCheck;
		PxU32								mEventReadSize;

		//Stream used to get the data. 
		TEventInStreamType					mInStream;
		PxU32								mStreamVersion;
		PxU64								mCurrentStreamId;
		PxU64								mCurrentTimestamp;


		PvdDataStreamReaderImpl( PxU32 inStreamVersion = SStreamInitialization::sCurrentStreamVersion )
			: mPropertyCount( 0 )
			, mEventCount( 0 )
			, mEventSizeCheck( 0 )
			, mEventReadSize( 0 )
			, mEventType( PvdConnectionEventType::Unknown )
			, mEventSubtype( PvdConnectionEventType::Unknown )
			, mStreamVersion( inStreamVersion )
			, mCurrentStreamId( 0 )
			, mCurrentTimestamp( 0 )
		{}
		
		void setData( const PxU8* inBeginPtr, const PxU8* inEndPtr )
		{
			mInStream.setup( inBeginPtr, inEndPtr, mStreamVersion );
		}
		bool hasMoreData() const { return mInStream.hasMoreData(); }

		void setStreamVersion( PxU32 inVersion ) { mStreamVersion = inVersion; }
		PxU32 getStreamVersion() const { return mStreamVersion; }
		PxU64 getCurrentStreamId() const { return mCurrentStreamId; }

		PvdConnectionEventData nextEvent()
		{
			if ( mEventCount )
			{
				if ( mEventType == PvdConnectionEventType::MultipleEvent )
					return ReadNextMultipleEvent();
				else if ( mEventType == PvdConnectionEventType::EventBatch )
					return readNextBatchEvent();
			}
			//Ensure the size parameter on the previous header was correct
			PX_ASSERT( mEventSizeCheck == mEventReadSize );
			PvdConnectionEventType theEventType;
			if ( mStreamVersion == 1 )
			{
				SEventHeader theHeader;
				theHeader.streamify( mInStream );
				theEventType = theHeader.mEventType;
				mEventSizeCheck = theHeader.mSize;
			}
			else if ( mStreamVersion < 4 )
			{
				SEventHeader2 theHeader;
				theHeader.streamify( mInStream );
				theEventType = theHeader.mEventType;
				mCurrentStreamId = theHeader.mStreamId;
				mEventSizeCheck = theHeader.mSize;
			}
			else
			{
				SEventHeader3 theHeader;
				theHeader.streamify( mInStream );
				theEventType = theHeader.mEventType;
				mCurrentStreamId = theHeader.mStreamId;
				mCurrentTimestamp = theHeader.mTimestamp;
				mEventSizeCheck = theHeader.mSize;
			}
			mEventReadSize = 0;
			if ( theEventType == PvdConnectionEventType::MultipleEvent )
			{
				mInStream.streamify( mEventCount );
				mInStream.streamify( mEventSubtype.mEventType );
				mEventReadSize += 5;
				mEventType = PvdConnectionEventType::MultipleEvent;
				return nextEvent();
			}
			else if ( theEventType == PvdConnectionEventType::EventBatch )
			{
				mInStream.streamify( mEventCount );
				mEventReadSize += 4;
				mEventType = PvdConnectionEventType::EventBatch;
				return nextEvent();
			}
			else //Who knows what happened, but we got an invalid event.
				return PvdConnectionEventData();
		}
		void destroy() { TDeleteOperator()(this); }
		


		////////////////////////////////////////////////////////////
		// Implementation.  Public because it is called from an 
		// parameterized operator().
		////////////////////////////////////////////////////////////


		inline PvdConnectionEventData deserializeNextEvent( PvdConnectionEventType inType )
		{
			PxU32 originalSize = mInStream.amountLeft();
			PvdConnectionEventData retval( EventTypeBasedOperation<PvdConnectionEventData>( inType, PvdConnectionStreamReaderOp<PvdDataStreamReaderImpl<TEventInStreamType> >( this )) );
			PxU32 finalSize = mInStream.amountLeft();
			mEventReadSize += originalSize - finalSize;
			return retval;
		}

		//Implementation
		inline PvdConnectionEventData ReadNextMultipleEvent()
		{
			PvdConnectionEventData retval;
			if ( mEventCount )
			{
				--mEventCount;
				return deserializeNextEvent( mEventSubtype );
			}
			return retval;
		}

		inline PvdConnectionEventData readNextBatchEvent()
		{
			PvdConnectionEventData retval;
			if ( mEventCount )
			{
				--mEventCount;
				PvdConnectionEventType theEventType;
				mInStream.streamify( theEventType.mEventType );
				mEventReadSize += 1;
				retval = deserializeNextEvent( theEventType );
			}
			return retval;
		}

		template<typename TDataType>
		inline TDataType deserialize()
		{
			TDataType theTemp;
			theTemp.streamify( mInStream );
			return theTemp;
		}
		template<typename TDataType>
		inline void beginBlock( const TDataType& inEvent)
		{
			mDatatypes.clear();
			mDatatypeHandlers.clear();
			mPropertyCount = inEvent.mPropertyCount;
			mInStream.setDatatypeHandlers( NULL );
			for ( PxU32 idx =0; idx < inEvent.mPropertyCount; ++idx )
			{
				mDatatypes.pushBack( inEvent.mDatatypes[idx] );
				mDatatypeHandlers.pushBack( mInStream.findHandlerForType( inEvent.mDatatypes[idx] ) );
			}
			if ( inEvent.mPropertyCount )
				mInStream.setDatatypeHandlers( mDatatypeHandlers.begin() );
		}
		//The blocks require special handling
		inline PvdConnectionEventData operator()( const SBeginPropertyBlock& )
		{
			SBeginPropertyBlock theBlock( deserialize<SBeginPropertyBlock>() );
			beginBlock( theBlock );
			return theBlock;
		}
		inline PvdConnectionEventData operator()( const SSendPropertyBlock& )
		{
			SSendPropertyBlock theBlock = createSendPropertyBlock( mDatatypes.begin(), mPropertyCount );
			theBlock.streamify( mInStream );
			return theBlock;
		}
		inline PvdConnectionEventData operator()( const SEndPropertyBlock& )
		{
			mInStream.setDatatypeHandlers( NULL );
			return createEndPropertyBlock();
		}
		inline PvdConnectionEventData operator()( const SBeginArrayBlock& )
		{
			SBeginArrayBlock theBlock( deserialize<SBeginArrayBlock>() );
			beginBlock( theBlock);
			return theBlock;
		}
		
		inline PvdConnectionEventData operator()( const SBeginArrayPropertyBlock& )
		{
			SBeginArrayPropertyBlock theBlock( deserialize<SBeginArrayPropertyBlock>() );
			beginBlock( theBlock );
			return theBlock;
		}

		inline PvdConnectionEventData operator()( const SArrayObject&)
		{
			SArrayObject theObject = createArrayObject( mDatatypes.begin(), mPropertyCount );
			theObject.streamify( mInStream );
			return theObject;
		}
		
		inline PvdConnectionEventData operator()( const SArrayObjects&)
		{
			SArrayObjects theObject = createArrayObjects( mDatatypes.begin(), mPropertyCount );
			theObject.streamify( mInStream );
			return theObject;
		}

		inline PvdConnectionEventData operator()( const SEndArrayBlock& )
		{
			mInStream.setDatatypeHandlers( NULL );
			return createEndArrayBlock();
		}
		
		inline PvdConnectionEventData operator()( const SEndArrayPropertyBlock& )
		{
			mInStream.setDatatypeHandlers( NULL );
			return createEndArrayPropertyBlock();
		}
		
		//Most events can serialize/deserialize trivially from the connection.
		template<typename TDataType>
		inline PvdConnectionEventData operator()( const TDataType& )
		{
			return deserialize<TDataType>();
		}
	};
}

#endif