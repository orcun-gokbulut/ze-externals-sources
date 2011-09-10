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
#ifndef REPX_CORE_EXTENSION_DESERIALIZER_H
#define REPX_CORE_EXTENSION_DESERIALIZER_H
#include "RepXExtensionSerializationBase.h"

namespace physx { namespace repx {

	inline bool readProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, PxConvexMesh*& outProp )
	{
		outProp = readReference<PxConvexMesh>( inSerializer, inPropName );
		return true;
	}
	
	inline bool readProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, PxTriangleMesh*& outProp )
	{
		outProp = readReference<PxTriangleMesh>( inSerializer, inPropName );
		return true;
	}
	
	inline bool readProperty( PxExtensionDeserializerBase& inSerializer, const char* inPropName, PxHeightField*& outProp )
	{
		outProp = readReference<PxHeightField>( inSerializer, inPropName );
		return true;
	}

	template<typename TGeomType>
	inline PxGeometry* parseGeometry( PxExtensionDeserializerBase& inSerializer, TGeomType& inGeom )
	{
		readAllProperties( inSerializer, &inGeom );
		return &inGeom;
	}

	template<typename TObjType>
	inline void readProperty( PxExtensionDeserializerBase& inSerializer, TObjType* inObj, const PxRigidActorShapeCollection& inProp )
	{
		if ( inSerializer.mReader->gotoChild( "Shapes" ) )
		{
			//uggh working around the shape collection api.
			//read out materials and geometry
			for ( bool success = inSerializer.mReader->gotoFirstChild(); success; 
					success = inSerializer.mReader->gotoNextSibling() )
			{
				InlineArray<PxMaterial*, 5> materials;
				PxGeometry* result = NULL;
				if ( inSerializer.mReader->gotoChild( "Materials" ) )
				{
					for( bool matSuccess = inSerializer.mReader->gotoFirstChild(); matSuccess;
						matSuccess = inSerializer.mReader->gotoNextSibling() )
					{
						materials.pushBack( readReference<PxMaterial>( inSerializer ) );
					}
					if ( materials.size() )
						inSerializer.mReader->leaveChild(); //a materialref child
					inSerializer.mReader->leaveChild(); //the materials child.
				}
				if ( inSerializer.mReader->gotoChild( "Geometry" ) )
				{
					if ( inSerializer.mReader->gotoFirstChild() )
					{
						const char* geomTypeName = inSerializer.mReader->getCurrentItemName();
						PxPlaneGeometry			plane;
						PxHeightFieldGeometry	heightField;
						PxSphereGeometry		sphere;
						PxTriangleMeshGeometry	mesh;
						PxConvexMeshGeometry	convex;
						PxBoxGeometry			box;
						PxCapsuleGeometry		capsule;
						if ( physx::string::stricmp( geomTypeName, "PxSphereGeometry" ) == 0 ) result = parseGeometry(inSerializer, sphere);
						else if ( physx::string::stricmp( geomTypeName, "PxPlaneGeometry" ) == 0 ) result = parseGeometry(inSerializer, plane);
						else if ( physx::string::stricmp( geomTypeName, "PxCapsuleGeometry" ) == 0 ) result = parseGeometry(inSerializer, capsule);
						else if ( physx::string::stricmp( geomTypeName, "PxBoxGeometry" ) == 0 ) result = parseGeometry(inSerializer, box);
						else if ( physx::string::stricmp( geomTypeName, "PxConvexMeshGeometry" ) == 0 ) result = parseGeometry(inSerializer, convex);
						else if ( physx::string::stricmp( geomTypeName, "PxTriangleMeshGeometry" ) == 0 ) result = parseGeometry(inSerializer, mesh);
						else if ( physx::string::stricmp( geomTypeName, "PxHeightFieldGeometry" ) == 0 ) result = parseGeometry(inSerializer, heightField);
						else
							PX_ASSERT( false );
						inSerializer.mReader->leaveChild(); //specific geometry
					}
					inSerializer.mReader->leaveChild(); //geometry tag
				}
				if ( result && materials.size() )
				{
					PxShape* theShape( inProp.createShape( inObj, *result, materials.begin(), materials.size(), PxTransform::createIdentity() ) );
					if ( theShape )
						readAllProperties( inSerializer, theShape );
				}
			}
			if ( inProp.size( inObj ) )
				inSerializer.mReader->leaveChild(); //PxShape
			inSerializer.mReader->leaveChild(); //Shapes
		}
	}

	struct RepXMemoryAllocateMemoryPoolAllocator
	{
		RepXMemoryAllocator* mAllocator;
		RepXMemoryAllocateMemoryPoolAllocator( RepXMemoryAllocator* inAlloc ) : mAllocator( inAlloc ) {}

		PxU8* allocate( PxU32 inSize ) { return mAllocator->allocate( inSize ); }
		void deallocate( PxU8* inMem ) { mAllocator->deallocate( inMem ); }
	};

	inline void strto( Triangle<PxU32>& ioDatatype, char*& ioData )
	{
		strto( ioDatatype.mIdx0, ioData );
		strto( ioDatatype.mIdx1, ioData );
		strto( ioDatatype.mIdx2, ioData );
	}

	inline void strto( PxHeightFieldSample& ioDatatype, char*& ioData )
	{
		PxU32 tempData;
		strto( tempData, ioData );
		if ( isBigEndian() )
		{
			PxU32& theItem(tempData);
			PxU32 theDest;
			PxU8* theReadPtr( reinterpret_cast< PxU8* >( &theItem ) );
			PxU8* theWritePtr( reinterpret_cast< PxU8* >( &theDest ) );
			//A height field sample is a 16 bit number
			//followed by two bytes.

			//We write this out as a 32 bit integer, LE.
			//Thus, on a big endian, we need to move the bytes
			//around a bit.
			//LE - 1 2 3 4
			//BE - 4 3 2 1 - after convert from xml number
			//Correct BE - 2 1 3 4, just like LE but with the 16 number swapped
			theWritePtr[0] = theReadPtr[2];
			theWritePtr[1] = theReadPtr[3];
			theWritePtr[2] = theReadPtr[1];
			theWritePtr[3] = theReadPtr[0];
			theItem = theDest;
		}
		ioDatatype = *reinterpret_cast<PxHeightFieldSample*>( &tempData );
	}

	template<typename TDataType>
	inline void readStridedBufferProperty( RepXReader& ioReader, const char* inPropName, void*& outData, PxU32& outStride, PxU32& outCount, RepXMemoryAllocator& inAllocator)
	{
		const char* theSrcData;
		if ( ioReader.read( inPropName, theSrcData ) )
		{
			RepXMemoryAllocateMemoryPoolAllocator tempAllocator( &inAllocator );
			MemoryBufferBase<RepXMemoryAllocateMemoryPoolAllocator> tempBuffer( &tempAllocator );

			char* theData = const_cast<char*>( theSrcData );
			if ( theData )
			{
				PxU32 theLen = strLen( theData );
				char* theEndData = theData + theLen;
				while( theData < theEndData )
				{
					//These buffers are whitespace delimited.
					TDataType theType;
					strto( theType, theData );
					tempBuffer.store( theType );
				}
				outData = reinterpret_cast< TDataType* >( tempBuffer.mBuffer );
				outStride = sizeof( TDataType );
				outCount = tempBuffer.mWriteOffset / sizeof( TDataType );
			}
			tempBuffer.releaseBuffer();
		}
	}
	
	template<typename TDataType>
	inline void readStridedBufferProperty( RepXReader& ioReader, const char* inPropName, PxStridedData& ioData, PxU32& outCount, RepXMemoryAllocator& inAllocator)
	{
		void* tempData = NULL;
		readStridedBufferProperty<TDataType>( ioReader, inPropName, tempData, ioData.stride, outCount, inAllocator ); 
		ioData.data = tempData;
	}
	
	template<typename TDataType>
	inline void readStridedBufferProperty( RepXReader& ioReader, const char* inPropName, PxTypedStridedData<TDataType>& ioData, PxU32& outCount, RepXMemoryAllocator& inAllocator)
	{
		void* tempData = NULL;
		readStridedBufferProperty<TDataType>( ioReader, inPropName, tempData, ioData.stride, outCount, inAllocator );
		ioData.data = reinterpret_cast<PxMaterialTableIndex*>( tempData );
	}

	template<typename TDataType>
	inline void readStridedBufferProperty( RepXReader& ioReader, const char* inPropName, PxBoundedData& ioData, RepXMemoryAllocator& inAllocator)
	{
		return readStridedBufferProperty<TDataType>( ioReader, inPropName, ioData, ioData.count, inAllocator );
	}

	inline void readProperty( PxExtensionDeserializerBase& inSerializer, PxArticulationLink* inObj, const TIncomingJointPropType& inProp )
	{
		if ( inSerializer.mReader->gotoChild( "joint" ) )
		{
			PxArticulationJoint* theJoint( inObj->getInboundJoint() );
			if ( theJoint )
				readAllProperties( inSerializer, theJoint );
			inSerializer.mReader->leaveChild();
		}
	}

	inline void readProperty( PxExtensionDeserializerBase& inSerializer, PxArticulation* inObj, const PxArticulationLinkCollectionProp& inProp )
	{
		FoundationWrapper theWrapper( inSerializer.mAllocator->getAllocator() );
		TArticulationLinkLinkMap linkRemapMap( theWrapper );
		if( inSerializer.mReader->gotoChild( "links" ) )
		{
			for ( bool links = inSerializer.mReader->gotoFirstChild();
				links != false;
				links = inSerializer.mReader->gotoNextSibling() )
			{
				//Need enough information to create the link...
				void* theParentPtr = NULL;
				const PxArticulationLink* theParentLink = NULL;
				if ( inSerializer.mReader->read( "Parent", theParentPtr ) )
				{
					const TArticulationLinkLinkMap::Entry* theRemappedParent( linkRemapMap.find( theParentPtr ) );
					//If we have a valid at write time, we had better have a valid parent at read time.
					PX_ASSERT( theRemappedParent );
					theParentLink = theRemappedParent->second;
				}
				PxArticulationLink* newLink = inObj->createLink( const_cast<PxArticulationLink*>( theParentLink ), PxTransform::createIdentity() );
				void* theIdPtr = NULL;
				inSerializer.mReader->read( "Id", theIdPtr );
				linkRemapMap.insert( theIdPtr, newLink );
				readAllProperties( inSerializer, newLink );
			}
			if ( inObj->getNbLinks() ) //PxArticulationLink
				inSerializer.mReader->leaveChild();
			inSerializer.mReader->leaveChild(); //links
		}
		
	}

}}

#endif
