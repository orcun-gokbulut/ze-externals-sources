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
#ifndef REPX_EXTENSION_SERIALIZATION_BASE_H
#define REPX_EXTENSION_SERIALIZATION_BASE_H
#include "RepX.h"
#include "RepXMemoryAllocator.h"
#include "RepXCoreExtensions.h"
#include "RepXStringToType.h"
#include "RepXImpl.h"
#include "PsString.h"
#include "PsInlineArray.h"
#include "PxMeshScale.h"
#include "PxMetaDataObjects.h"
#include "PxHeightFieldSample.h"
#include "RepXWriter.h"
#include "RepXReader.h"
#include "PxStringTable.h"

namespace physx { namespace repx {


	struct PxExtensionSerializerBase
	{
		RepXWriter*				mWriter;
		MemoryBuffer*			mTempBuffer;
		RepXIdToRepXObjectMap*	mIdMap;
		PxExtensionSerializerBase( RepXWriter* inWriter, MemoryBuffer* inTempBuffer, RepXIdToRepXObjectMap* inMap )
			: mWriter( inWriter )
			, mTempBuffer( inTempBuffer )
			, mIdMap( inMap )
		{
		}
	protected:
		PxExtensionSerializerBase( const PxExtensionSerializerBase& inOther )
			: mWriter( inOther.mWriter )
			, mTempBuffer( inOther.mTempBuffer )
			, mIdMap( inOther.mIdMap )
		{
		}
	};

	struct PxExtensionDeserializerBase
	{
		RepXReader*				mReader;
		RepXMemoryAllocator*	mAllocator;
		RepXInstantiationArgs*	mArgs;
		RepXIdToRepXObjectMap*	mIdMap;

		PxExtensionDeserializerBase( RepXReader* inReader, RepXMemoryAllocator* inAllocator
									, RepXInstantiationArgs* inArgs, RepXIdToRepXObjectMap* inIdMap )
			: mReader( inReader )
			, mAllocator( inAllocator )
			, mArgs( inArgs )
			, mIdMap( inIdMap )
		{
		}
	protected:
		PxExtensionDeserializerBase( const PxExtensionDeserializerBase& inOther )
			: mReader( inOther.mReader )
			, mAllocator( inOther.mAllocator )
			, mArgs( inOther.mArgs )
			, mIdMap( inOther.mIdMap )
		{
		}
	};

	template<typename TDataType>
	inline void writeReference( PxExtensionSerializerBase& inSerializer, const char* inPropName, const TDataType* inDatatype )
	{
		inSerializer.mWriter->write( inPropName, createRepXObject( inDatatype, inSerializer.mIdMap->getIdForLiveObject( getBasePtr( inDatatype ) ) ) );
	}

	template<typename TCollectionType, typename TObjType, typename TPropertyType, typename TOperator>
	void writeCollectionProperty( const char* inGroupName
									, PxExtensionSerializerBase& inSerializer
									, const TObjType* inObj
									, TPropertyType& inProp
									, TOperator inCollectionMemberHandler )
	{
		PxU32 count( inProp.size( inObj ) );
		if ( count )
		{
			InlineArray<TCollectionType*,5> theData;
			theData.resize( count );
			inSerializer.mWriter->addAndGotoChild( inGroupName );
			inProp.get( inObj, theData.begin(), count );
			for( PxU32 idx =0; idx < count; ++idx )
				inCollectionMemberHandler( inSerializer, theData[idx] );
			inSerializer.mWriter->leaveChild();
		}
	}

	inline void writeProperty( RepXWriter& inWriter, MemoryBuffer& inBuffer, const char* inProp )
	{
		inBuffer.storeByte( 0 );
		inWriter.write( inProp, reinterpret_cast<const char*>( inBuffer.mBuffer ) );
		inBuffer.clear();
	}
	
	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const char* inProp )
	{
		writeProperty( *inSerializer.mWriter, *inSerializer.mTempBuffer, inProp );
	}

	template<typename TDataType>
	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, TDataType inValue )
	{
		(*inSerializer.mTempBuffer) << inValue;
		writeProperty( inSerializer, inPropName );
	}
	
	template<typename TDataType>
	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, TDataType inValue, const PxUnknownClassInfo& )
	{
		writeProperty( inSerializer, inPropName, inValue );
	}

	template<typename TDataType, typename TInfoType>
	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, TDataType inValue, const TInfoType& )
	{
		inSerializer.mWriter->addAndGotoChild( inPropName );
		writeAllProperties( inSerializer, &inValue );
		inSerializer.mWriter->leaveChild();
	}
	
	template<PxU32 TNumBytes>
	inline void writeProperty( PxExtensionSerializerBase&, const char*, PxPadding<TNumBytes> )
	{
	}

	inline void writeProperty( PxExtensionSerializerBase&, const char*, PxStridedData )
	{
	}
	
	inline void writeFlagsProperty( PxExtensionSerializerBase& inSerializer, const char* inProp, PxU32 inFlags, const PxU32ToName* inTable )
	{
		if ( inTable )
		{
			PxU32 flagValue( inFlags );
			if ( flagValue )
			{
				for ( PxU32 idx =0; inTable[idx].mName != NULL; ++idx )
				{
					if ( (inTable[idx].mValue & flagValue) != 0 )
					{
						if ( inSerializer.mTempBuffer->mWriteOffset != 0 )
							(*inSerializer.mTempBuffer) << "|";
						(*inSerializer.mTempBuffer) << inTable[idx].mName;
					}
				}
				writeProperty( inSerializer, inProp );
			}
		}
	}

	template<typename TEnumType>
	inline void writeEnumProperty( PxExtensionSerializerBase& inSerializer, const char* inProp, TEnumType inDatatype, const PxU32ToName* inTable )
	{
		if ( inTable )
		{
			PxU32 theValue( static_cast<PxU32>( inDatatype ) );
			for ( PxU32 idx =0; inTable[idx].mName != NULL; ++idx )
			{
				if ( theValue == inTable[idx].mValue )
				{
					inSerializer.mWriter->write( inProp, inTable[idx].mName );
					break;
				}
			}
		}
	}

	template<typename TDataType, typename TTraitsType>
	inline void writePropertyDispatch( PxExtensionSerializerBase& inSerializer, const char* inPropName, TDataType inData, const TTraitsType& inTraits, bool )
	{
		writeProperty( inSerializer, inPropName, inData, inTraits );
	}
	
	template<typename TDataType>
	inline void writePropertyDispatch( PxExtensionSerializerBase& inSerializer, const char* inPropName, TDataType inData, const PxUnknownClassInfo& inTraits, const PxU32ToName* inNames )
	{
		writeEnumProperty( inSerializer, inPropName, inData, inNames );
	}

	template<typename TEnumType, typename TStorageType>
	inline void writePropertyDispatch( PxExtensionSerializerBase& inSerializer, const char* inPropName, PxFlags<TEnumType, TStorageType> inData, const PxUnknownClassInfo& inTraits, bool )
	{
		writeFlagsProperty( inSerializer, inPropName, inData, PxEnumTraits<TEnumType>().NameConversion );
	}

	template<typename TDataType>
	inline void writePropertyDispatch( PxExtensionSerializerBase& inSerializer, const char* inPropName, TDataType inData )
	{
		writePropertyDispatch( inSerializer, inPropName, inData, PxClassInfoTraits<TDataType>().Info, PxEnumTraits<TDataType>().NameConversion );
	}
	
	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TSetPropType, typename TGetPropType>
	inline void writeFullDuplexProperty( PxExtensionSerializerBase& inSerializer, const TInObjType* inObj, const PxPropertyInfo<TKey, TObjType, TSetPropType, TGetPropType> & inProp )
	{
		writePropertyDispatch( inSerializer, inProp.mName, inProp.get( inObj ), PxClassInfoTraits<TGetPropType>().Info, PxEnumTraits<TGetPropType>().NameConversion );
	}

	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TPropertyType>
	inline void writeRangeProperty( PxExtensionSerializerBase& inSerializer, const TInObjType* inObj, const PxRangePropertyInfo<TKey, TObjType, TPropertyType>& inProp )
	{
		TPropertyType lhs,rhs;
		inProp.get( inObj, lhs, rhs );
		inSerializer.mWriter->addAndGotoChild( inProp.mName );
		writeProperty( inSerializer, inProp.mArg0Name, lhs );
		writeProperty( inSerializer, inProp.mArg1Name, rhs );
		inSerializer.mWriter->leaveChild();
	}
	
	//If non of the index is not named we ignore the property
	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TIndexType, typename TPropertyType>
	inline void writeIndexedProperty( PxExtensionSerializerBase&, const TInObjType*, const PxIndexedPropertyInfo<TKey, TObjType, TIndexType, TPropertyType>&, bool )
	{
	}

	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TIndexType, typename TPropertyType>
	inline void writeIndexedProperty( PxExtensionSerializerBase& inSerializer, const TInObjType* inObj, const PxIndexedPropertyInfo<TKey, TObjType, TIndexType, TPropertyType>& inProp, const PxU32ToName* inIndexNames )
	{
		//OK, the index is named.
		//time to simplify the problem down a little bit.
		//run through each index in the name table and read a property from that index.
		inSerializer.mWriter->addAndGotoChild( inProp.mName );
		for ( const PxU32ToName* theName = inIndexNames; 
			theName->mName != NULL;
			++theName )
		{
			TIndexType theIndex( static_cast<TIndexType>( theName->mValue ) );
			TPropertyType theProp( inProp.get( inObj, theIndex ) );
			writePropertyDispatch( inSerializer, theName->mName, theProp, PxClassInfoTraits<TPropertyType>().Info, PxEnumTraits<TPropertyType>().NameConversion );
		}
		inSerializer.mWriter->leaveChild();
	}

	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TIndexType, typename TPropertyType>
	inline void writeIndexedProperty( PxExtensionSerializerBase& inDeserializer, const TInObjType* inObj, const PxIndexedPropertyInfo<TKey, TObjType, TIndexType, TPropertyType>& inProp )
	{
		writeIndexedProperty( inDeserializer, inObj, inProp, PxEnumTraits<TIndexType>().NameConversion );
	}

	inline void writePxVec3( MemoryBuffer& inBuffer, const PxVec3& inData ) { inBuffer << inData; }

	template<typename TObjType, typename TAccessOperator, typename TWriteOperator>
	inline void writeBuffer( RepXWriter& inWriter, MemoryBuffer& inTempBuffer
							, PxU32 inObjPerLine, const TObjType* inObjType, TAccessOperator inAccessOperator
							, PxU32 inBufSize, const char* inPropName, TWriteOperator inOperator )
	{
		if ( inBufSize )
		{
			for ( PxU32 idx = 0; idx < inBufSize; ++idx )
			{
				if ( idx && ( idx % inObjPerLine == 0 ) )
					inTempBuffer << "\n\t\t\t";
				else
					inTempBuffer << " ";
				inOperator( inTempBuffer, inAccessOperator( inObjType, idx ) );
			}
			writeProperty( inWriter, inTempBuffer, inPropName );
		}
	}

	template<typename TDataType>
	inline const TDataType& PtrAccess( const TDataType* inPtr, PxU32 inIndex )
	{
		return inPtr[inIndex];
	}

	inline void writeHeightFieldSample( MemoryBuffer& inStream, const PxHeightFieldSample& inSample )
	{
		PxU32 retval;
		PxU8* writePtr( reinterpret_cast< PxU8*>( &retval ) );
		const PxU8* inPtr( reinterpret_cast<const PxU8*>( &inSample ) );
		if ( isBigEndian() )
		{
			//Height field samples are a
			//16 bit integer followed by two bytes.
			//right now, data is 2 1 3 4
			//We need a 32 bit integer that 
			//when read in by a LE system is 4 3 2 1.
			//Thus, we need a BE integer that looks like:
			//4 3 2 1

			writePtr[0] = inPtr[3];
			writePtr[1] = inPtr[2];
			writePtr[2] = inPtr[0];
			writePtr[3] = inPtr[1];
		}
		else
		{
			writePtr[0] = inPtr[0];
			writePtr[1] = inPtr[1];
			writePtr[2] = inPtr[2];
			writePtr[3] = inPtr[3];
		}
		inStream << retval;
	}

	template<typename TDataType, typename TWriteOperator>
	inline void writeBuffer( RepXWriter& inWriter, MemoryBuffer& inTempBuffer
							, PxU32 inObjPerLine, const TDataType* inBuffer
							, PxU32 inBufSize, const char* inPropName, TWriteOperator inOperator )
	{
		writeBuffer( inWriter, inTempBuffer, inObjPerLine, inBuffer, PtrAccess<TDataType>, inBufSize, inPropName, inOperator );
	}
	
	inline PxU32 findEnumByName( const char* inName, const PxU32ToName* inTable )
	{
		for ( PxU32 idx = 0; inTable[idx].mName != NULL; ++idx )
		{
			if ( physx::string::stricmp( inTable[idx].mName, inName ) == 0 )
				return inTable[idx].mValue;
		}
		return 0;
	}


	template<typename TDataType>
	PX_INLINE void stringToFlagsType( const char* strData, PxExtensionDeserializerBase& inSerializer, TDataType& ioType, const PxU32ToName* inTable )
	{
		if ( inTable == NULL )
			return;
		ioType = 0;
		if ( strData && *strData)
		{
			//Destructively parse the string to get out the different flags.
			char* theValue = const_cast<char*>( copyStr( inSerializer.mAllocator, strData ) );
			char* theMarker = theValue;
			char* theNext = theValue;
			while( theNext && *theNext )
			{
				++theNext;
				if( *theNext == '|' )
				{
					*theNext = 0;
					++theNext;
					ioType += static_cast< TDataType > ( findEnumByName( theMarker, inTable ) );
					theMarker = theNext;
				}
			}
			if ( theMarker && *theMarker )
				ioType += static_cast< TDataType > ( findEnumByName( theMarker, inTable ) );
			inSerializer.mAllocator->deallocate( reinterpret_cast<PxU8*>( theValue ) );
		}
	}

	template<typename TDataType>
	PX_INLINE void stringToEnumType( const char* strData, TDataType& ioType, const PxU32ToName* inTable )
	{
		ioType = static_cast<TDataType>( findEnumByName( strData, inTable ) );
	}

	template<typename TDataType>
	PX_INLINE void stringToType( const char* inValue, TDataType& ioType )
	{
		char* theVal(const_cast<char*>(inValue));
		strto( ioType, ioType );
	}

	template<typename TSerializer, typename TDataType>
	PX_INLINE void stringToType( const char* inValue, PxExtensionDeserializerBase& inSerializer, const char*, TDataType& ioType )
	{
		stringToType( inValue, ioType );
	}
	
	template<typename TSerializer>
	PX_INLINE void stringToType( const char* inValue, PxExtensionDeserializerBase& inSerializer, const char*, const char*& ioType )
	{
		ioType = copyStr( inSerializer.mAllocator, inValue );
	}
	
	inline bool readProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, const char*& ioType )
	{
		const char* theValue = NULL;
		if ( inSerializer.mReader->read( inPropName, theValue ) )
		{
			//Use the string table to setup the name.
			if ( inSerializer.mArgs->mStringTable != NULL )
				ioType = inSerializer.mArgs->mStringTable->allocateStr( theValue );
			else
			//Use the base allocator type instead of the public one because
			//on deserialization we want the allocations to last beyond the lifetime
			//of the collection.  The memory allocated by the mAllocator only lasts
			//up to the lifetime of the collection.
				ioType = copyStr( inSerializer.mAllocator->getAllocator(), theValue );
			return true;
		}
		return false;
	}

	template<typename TDataType>
	inline bool readProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, TDataType& ioType )
	{
		const char* theValue = NULL;
		if ( inSerializer.mReader->read( inPropName, theValue ) )
		{
			stringToType( theValue, ioType );
			return true;
		}
		return false;
	}
	
	template<typename TDataType>
	inline bool readProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, TDataType& ioType, const PxUnknownClassInfo& )
	{
		return readProperty( inSerializer, inPropName, ioType );
	}
	
	template<typename TDataType, typename TInfoType>
	inline bool readProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, TDataType& ioType, const TInfoType& )
	{
		if ( inSerializer.mReader->gotoChild( inPropName ) )
		{
			readAllProperties( inSerializer, &ioType );
			inSerializer.mReader->leaveChild();
			return true;
		}
		return false;
	}
	
	template<PxU32 TNumBytes>
	inline bool readProperty( PxExtensionDeserializerBase&, const char*, PxPadding<TNumBytes>& )
	{
		return false;
	}

	inline bool readProperty( PxExtensionDeserializerBase&, const char*, PxStridedData& )
	{
		return false;
	}

	template<typename TDataType>
	PX_INLINE bool readFlagsProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, TDataType& ioFlags, const PxU32ToName* inTable )
	{
		const char* theValue = NULL;
		if ( inSerializer.mReader->read( inPropName, theValue ) )
		{
			stringToFlagsType( theValue, inSerializer, ioFlags, inTable );
			return true;
		}
		return false;
	}

	template<typename TEnumType, typename TStorageType>
	PX_INLINE bool readFlagsProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, PxFlags<TEnumType, TStorageType>& ioFlags, const PxU32ToName* inTable )
	{
		PxU32 theFlags(ioFlags);
		if ( readFlagsProperty( inSerializer, inPropName, theFlags, inTable ) )
		{
			ioFlags = PxFlags<TEnumType, TStorageType>( static_cast<TStorageType>( theFlags ) );
			return true;
		}
		return false;
	}
	
	template<typename TDataType>
	PX_INLINE bool readEnumProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, TDataType& ioFlags, const PxU32ToName* inTable )
	{
		const char* theValue = NULL;
		if ( inSerializer.mReader->read( inPropName, theValue ) )
		{
			stringToEnumType( theValue, ioFlags, inTable );
			return true;
		}
		return false;
	}

	//assumes the reference is the current node
	template<typename TObjType>
	inline TObjType* readReference( PxExtensionDeserializerBase& inSerializer )
	{
		void* theData;
		char* theValue = const_cast<char*>( inSerializer.mReader->getCurrentItemValue() );
		strto( theData, theValue );
		return reinterpret_cast<TObjType*>( const_cast<void*>( inSerializer.mIdMap->getLiveObjectFromId( theData ).mLiveObject ) );
	}
	//assumes the reference is the current node
	template<typename TObjType>
	inline TObjType* readReference( PxExtensionDeserializerBase& inSerializer, const char* inPropName )
	{
		void* theData;
		if ( inSerializer.mReader->read( inPropName, theData ) )
			return reinterpret_cast<TObjType*>( const_cast<void*>( inSerializer.mIdMap->getLiveObjectFromId( theData ).mLiveObject ) );
		return NULL;
	}
	
	template<typename TDataType, typename TTraitsType>
	inline bool readPropertyDispatch( PxExtensionDeserializerBase& inSerializer, const char* inPropName, TDataType& inData, const TTraitsType& inTraits, bool )
	{
		return readProperty( inSerializer, inPropName, inData, inTraits );
	}
	
	template<typename TEnumType>
	inline bool readPropertyDispatch( PxExtensionDeserializerBase& inDeserializer, const char* inPropName, TEnumType& inData, const PxUnknownClassInfo& inTraits, const PxU32ToName* inNames )
	{
		return readEnumProperty( inDeserializer, inPropName, inData, inNames );
	}

	template<typename TEnumType, typename TStorageType>
	inline bool readPropertyDispatch( PxExtensionDeserializerBase& inDeserializer, const char* inPropName, PxFlags<TEnumType, TStorageType>& inData, const PxUnknownClassInfo& inTraits, bool )
	{
		return readFlagsProperty( inDeserializer, inPropName, inData, PxEnumTraits<TEnumType>().NameConversion );
	}

	template<typename TDataType>
	inline bool readPropertyDispatch( PxExtensionDeserializerBase& inDeserializer, const char* inPropName, TDataType& inData )
	{
		return readPropertyDispatch( inDeserializer, inPropName, inData, PxClassInfoTraits<TDataType>().Info, PxEnumTraits<TDataType>().NameConversion );
	}

	//=============================================
	//Full access property types
	//=============================================
	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TSetPropType, typename TGetPropType>
	inline void readFullDuplexProperty( PxExtensionDeserializerBase& inDeserializer, TInObjType* inObj, const PxPropertyInfo<TKey, TObjType, TSetPropType, TGetPropType> & inProp )
	{
		TGetPropType theProp( inProp.get( inObj ) );
		if ( readPropertyDispatch( inDeserializer, inProp.mName, theProp, PxClassInfoTraits<TGetPropType>().Info, PxEnumTraits<TGetPropType>().NameConversion ) )
			inProp.set( inObj, theProp );
	}

	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TPropertyType>
	inline void readRangeProperty( PxExtensionDeserializerBase& inDeserializer, TInObjType* inObj, const PxRangePropertyInfo<TKey, TObjType, TPropertyType>& inProp )
	{
		TPropertyType lhs,rhs;
		inProp.get( inObj, lhs, rhs );

		if ( inDeserializer.mReader->gotoChild( inProp.mName ) )
		{
			readProperty( inDeserializer, inProp.mArg0Name, lhs );
			readProperty( inDeserializer, inProp.mArg1Name, rhs );
			inDeserializer.mReader->leaveChild();
			inProp.set( inObj, lhs, rhs );
		}
	}
	//If non of the index is not named we ignore the property
	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TIndexType, typename TPropertyType>
	inline void readIndexedProperty( PxExtensionDeserializerBase&, TInObjType*, const PxIndexedPropertyInfo<TKey, TObjType, TIndexType, TPropertyType>&, bool )
	{
	}

	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TIndexType, typename TPropertyType>
	inline void readIndexedProperty( PxExtensionDeserializerBase& inDeserializer, TInObjType* inObj, const PxIndexedPropertyInfo<TKey, TObjType, TIndexType, TPropertyType>& inProp, const PxU32ToName* inIndexNames )
	{
		//OK, the index is named.
		//time to simplify the problem down a little bit.
		//run through each index in the name table and read a property from that index.
		if ( inDeserializer.mReader->gotoChild( inProp.mName ) )
		{
			for ( const PxU32ToName* theName = inIndexNames; 
				theName->mName != NULL;
				++theName )
			{
				TIndexType theIndex( static_cast<TIndexType>( theName->mValue ) );
				TPropertyType theProp( inProp.get( inObj, theIndex ) );
				if ( readPropertyDispatch( inDeserializer, theName->mName, theProp, PxClassInfoTraits<TPropertyType>().Info, PxEnumTraits<TPropertyType>().NameConversion ) )
					inProp.set( inObj, theIndex, theProp );
			}
			inDeserializer.mReader->leaveChild();
		}
	}

	template<PxPropertyInfoName::Enum TKey, typename TInObjType, typename TObjType, typename TIndexType, typename TPropertyType>
	inline void readIndexedProperty( PxExtensionDeserializerBase& inDeserializer, TInObjType* inObj, const PxIndexedPropertyInfo<TKey, TObjType, TIndexType, TPropertyType>& inProp )
	{
		readIndexedProperty( inDeserializer, inObj, inProp, PxEnumTraits<TIndexType>().NameConversion );
	}

	//Functions defined here but not implemented.  Implementation is in RepXExtensionSerializater and RepXExtensionDeserializer.
	template<typename TObjType> inline void writeAllProperties(PxExtensionSerializerBase& inSerializer, const TObjType* inObj );
	template<typename TObjType> inline void readAllProperties(PxExtensionDeserializerBase& inSerializer, TObjType* inObj );
}}
#endif