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
#ifndef REPX_CORE_EXTENSION_SERIALIZER_H
#define REPX_CORE_EXTENSION_SERIALIZER_H
#include "RepXExtensionSerializationBase.h"

namespace physx { namespace repx {
	using namespace physx::shdfnd2;

	inline void writeShapeMaterial( PxExtensionSerializerBase& inSerializer, const PxMaterial* inMaterial )
	{
		writeReference( inSerializer, "PxMaterialRef", inMaterial );
	}

	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const PxShape* inObj, const PxShapeMaterialsProperty& inProp )
	{
		writeCollectionProperty<PxMaterial>( "Materials", inSerializer, inObj, inProp, writeShapeMaterial );
	}

	inline void writeActorShape( PxExtensionSerializerBase& inSerializer, const PxShape* inShape )
	{
		inSerializer.mWriter->addAndGotoChild( "PxShape" );
		writeAllProperties( inSerializer, inShape );
		inSerializer.mWriter->leaveChild();
	}

	template<typename TObjType>
	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const TObjType* inObj, const PxRigidActorShapeCollection& inProp )
	{
		writeCollectionProperty<PxShape>( "Shapes", inSerializer, inObj, inProp, writeActorShape );
	}

	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, const PxConvexMesh* inDatatype )
	{
		writeReference( inSerializer, inPropName, inDatatype );
	}

	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, const PxTriangleMesh* inDatatype )
	{
		writeReference( inSerializer, inPropName, inDatatype );
	}

	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, const PxHeightField* inDatatype )
	{
		writeReference( inSerializer, inPropName, inDatatype );
	}

	template<typename GeometryType>
	inline void writeGeometryProperty( PxExtensionSerializerBase& inSerializer, const PxShape* inObj, const PxShapeGeometryProperty& inProp, const char* inTypeName )
	{
		inSerializer.mWriter->addAndGotoChild( "Geometry" );
		inSerializer.mWriter->addAndGotoChild( inTypeName );
		GeometryType theType;
		inProp.getGeometry( inObj, theType );
		writeAllProperties( inSerializer, &theType );
		inSerializer.mWriter->leaveChild();
		inSerializer.mWriter->leaveChild();
	}

	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const PxShape* inObj, const PxShapeGeometryProperty& inProp )
	{
		switch( inObj->getGeometryType() )
		{
			case PxGeometryType::eSPHERE: writeGeometryProperty<PxSphereGeometry>( inSerializer, inObj, inProp, "PxSphereGeometry" ); break;
			case PxGeometryType::ePLANE: writeGeometryProperty<PxPlaneGeometry>( inSerializer, inObj, inProp, "PxPlaneGeometry" ); break;
			case PxGeometryType::eCAPSULE: writeGeometryProperty<PxCapsuleGeometry>( inSerializer, inObj, inProp, "PxCapsuleGeometry" ); break;
			case PxGeometryType::eBOX: writeGeometryProperty<PxBoxGeometry>( inSerializer, inObj, inProp, "PxBoxGeometry" ); break;
			case PxGeometryType::eCONVEXMESH: writeGeometryProperty<PxConvexMeshGeometry>( inSerializer, inObj, inProp, "PxConvexMeshGeometry" ); break;
			case PxGeometryType::eTRIANGLEMESH: writeGeometryProperty<PxTriangleMeshGeometry>( inSerializer, inObj, inProp, "PxTriangleMeshGeometry" ); break;
			case PxGeometryType::eHEIGHTFIELD: writeGeometryProperty<PxHeightFieldGeometry>( inSerializer, inObj, inProp, "PxHeightFieldGeometry" ); break;
			default: PX_ASSERT( false );
		}
	}
	
	inline PxStream& operator<<( PxStream& inStream, const PxHeightFieldSample& inSample )
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
		return inStream;
	}

	template<typename TDataType, typename TWriteOperator>
	inline void writeStridedBufferProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, const void* inData, PxU32 inStride, PxU32 inCount, PxU32 inItemsPerLine, TWriteOperator inOperator )
	{
		PX_ASSERT( inStride == 0 || inStride == sizeof( TDataType ) );
		writeBuffer( *inSerializer.mWriter, *inSerializer.mTempBuffer
					, inItemsPerLine, reinterpret_cast<const TDataType*>( inData )
					, inCount, inPropName, inOperator );
	}

	template<typename TDataType, typename TWriteOperator>
	inline void writeStridedBufferProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, const PxStridedData& inData, PxU32 inCount, PxU32 inItemsPerLine, TWriteOperator inOperator )
	{
		writeStridedBufferProperty<TDataType>( inSerializer, inPropName, inData.data, inData.stride, inCount, inItemsPerLine, inOperator );
	}
	
	template<typename TDataType, typename TWriteOperator>
	inline void writeStridedBufferProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, const PxTypedStridedData<TDataType>& inData, PxU32 inCount, PxU32 inItemsPerLine, TWriteOperator inOperator )
	{
		writeStridedBufferProperty<TDataType>( inSerializer, inPropName, inData.data, inData.stride, inCount, inItemsPerLine, inOperator );
	}
	
	template<typename TDataType, typename TWriteOperator>
	inline void writeStridedBufferProperty( PxExtensionSerializerBase& inSerializer, const char* inPropName, const PxBoundedData& inData, PxU32 inItemsPerLine, TWriteOperator inWriteOperator )
	{
		writeStridedBufferProperty<TDataType>( inSerializer, inPropName, inData, inData.count, inItemsPerLine, inWriteOperator );
	}
	
	struct PxArticulationSerializer : public PxExtensionSerializerBase
	{
	private:
		PxArticulationSerializer( const PxArticulationSerializer& inOther );
		PxArticulationSerializer& operator=( const PxArticulationSerializer& inOther );
	public:

		TArticulationLinkLinkMap mArticulationLinkParents;

		PxArticulationSerializer( const PxArticulation* inArticulation, const PxExtensionSerializerBase& inBase )
			: PxExtensionSerializerBase( inBase )
			, mArticulationLinkParents( inBase.mTempBuffer->mManager->getWrapper() )
		{
			InlineArray<PxArticulationLink*, 64, WrapperReflectionAllocator<PxArticulationLink*> > linkList( WrapperReflectionAllocator<PxArticulationLink*>( inBase.mTempBuffer->mManager->getWrapper() ) );
			PxU32 numLinks = inArticulation->getNbLinks();
			linkList.resize( numLinks );
			inArticulation->getLinks( linkList.begin(), numLinks );
			for ( PxU32 idx = 0; idx < numLinks; ++idx )
			{
				const PxArticulationLink* theLink( linkList[idx] );
				InlineArray<PxArticulationLink*, 64> theChildList;
				PxU32 numChildren = theLink->getNbChildren();
				theChildList.resize( numChildren );
				theLink->getChildren( theChildList.begin(), numChildren );
				for ( PxU32 childIdx = 0; childIdx < numChildren; ++childIdx )
					mArticulationLinkParents.insert( theChildList[childIdx], theLink );
			}
		}
	};

	//Usually we don't deal with read-only properties.
	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const PxArticulationLink* inObj, const TIncomingJointPropType& inProp )
	{
		const PxArticulationJoint* theJoint( inProp.get( inObj ) );
		if ( theJoint )
		{
			inSerializer.mWriter->addAndGotoChild( "Joint" );
			writeAllProperties( inSerializer, theJoint );
			inSerializer.mWriter->leaveChild();
		}
	}

	inline void writeArticulationLink( PxExtensionSerializerBase& inSerializer, const PxArticulationLink* inLink )
	{
		inSerializer.mWriter->addAndGotoChild( "PxArticulationLink" );
		PxArticulationSerializer& theSerializer( static_cast<PxArticulationSerializer&>( inSerializer ) );
		const TArticulationLinkLinkMap::Entry* theParentPtr = theSerializer.mArticulationLinkParents.find( inLink );
		if ( theParentPtr != NULL )
			writeProperty( theSerializer, "Parent", reinterpret_cast<const void*>( theParentPtr->second ) );
		writeProperty( theSerializer, "Id", reinterpret_cast<const void*>( inLink ) );

		writeAllProperties( inSerializer, inLink );
		//We guarantee this by overriding the serializer in the writeAllProperties function for
		//the pxarticulation link.
		inSerializer.mWriter->leaveChild();
	}

	inline void recurseAddLinkAndChildren( const PxArticulationLink* inLink, InlineArray<const PxArticulationLink*, 64>& ioLinks )
	{
		ioLinks.pushBack( inLink );
		InlineArray<PxArticulationLink*, 8> theChildren;
		PxU32 childCount( inLink->getNbChildren() );
		theChildren.resize( childCount );
		inLink->getChildren( theChildren.begin(), childCount );
		for ( PxU32 idx = 0; idx < childCount; ++idx )
			recurseAddLinkAndChildren( theChildren[idx], ioLinks );
	}

	inline void writeProperty( PxExtensionSerializerBase& inSerializer, const PxArticulation* inObj, const PxArticulationLinkCollectionProp& inProp )
	{
		//topologically sort the links as per my discussion with Dilip because
		//links aren't guaranteed to have the parents before the children in the
		//overall link list and it is unlikely to be done by beta 1.
		PxU32 count( inProp.size( inObj ) );
		if ( count )
		{
			InlineArray<PxArticulationLink*, 64> theLinks;
			theLinks.resize( count );
			inProp.get( inObj, theLinks.begin(), count );
			inSerializer.mWriter->addAndGotoChild( "Links" );
			
			InlineArray<const PxArticulationLink*, 64> theSortedLinks;
			PxArticulationSerializer& theRealSerializer( static_cast<PxArticulationSerializer&>( inSerializer ) );
			for ( PxU32 idx = 0; idx < count; ++idx )
			{
				const PxArticulationLink* theLink( theLinks[idx] );
				if ( theRealSerializer.mArticulationLinkParents.find( theLink ) == NULL )
					recurseAddLinkAndChildren( theLink, theSortedLinks );
			}	
			PX_ASSERT( theSortedLinks.size() == count );
			for ( PxU32 idx = 0; idx < count; ++idx )
				writeArticulationLink( inSerializer, theSortedLinks[idx] );

			inSerializer.mWriter->leaveChild();
		}
	}
	
	inline void writeAllProperties( PxExtensionSerializerBase& inSerializer, const PxArticulation* inObj )
	{
		PxArticulationSerializer theSerializer( inObj, inSerializer );
		writeAllProperties<PxArticulation>( theSerializer, inObj );
	}
}}
#endif
