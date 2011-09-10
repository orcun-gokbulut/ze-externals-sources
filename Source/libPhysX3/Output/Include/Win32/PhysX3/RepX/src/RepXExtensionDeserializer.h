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
#ifndef REPX_EXTENSION_DESERIALIZER_H
#define REPX_EXTENSION_DESERIALIZER_H
#include "RepXMemoryAllocator.h"

namespace physx { namespace repx {

	//Property types not explicity picked up somehow are ignored.
	template<typename TObjType, typename TPropertyType>
	inline void readProperty( PxExtensionDeserializerBase&, TObjType*, TPropertyType& )
	{
	}

	template<typename TDeserializerType>
	struct PxExtensionDeserializer
	{
		PxExtensionDeserializerBase* mDeserializer;
		TDeserializerType* mObj;
		PxExtensionDeserializer( PxExtensionDeserializerBase* inBase, TDeserializerType* inObj)
			: mDeserializer( inBase )
			, mObj( inObj )
		{
		}
		//catch all for properties we know nothing about.
		template<typename TPropertyType>
		void operator()( const TPropertyType& inProp, PxU32 ) 
		{
			readProperty( *mDeserializer, mObj, inProp );
		}

		template<PxPropertyInfoName::Enum TKey, typename TObjType, typename TSetPropType, typename TGetPropType>
		inline void operator()( const PxPropertyInfo<TKey, TObjType, TSetPropType, TGetPropType> & inProp, PxU32 )
		{
			readFullDuplexProperty( *mDeserializer, static_cast< TObjType* >( mObj ), inProp );
		}

		template<PxPropertyInfoName::Enum TKey, typename TObjType, typename TPropertyType>
		inline void operator()( const PxRangePropertyInfo<TKey, TObjType, TPropertyType>& inProp, PxU32 )
		{
			readRangeProperty( *mDeserializer, static_cast< TObjType* >( mObj ), inProp );
		}

		template<PxPropertyInfoName::Enum TKey, typename TObjType, typename TIndexType, typename TPropertyType>
		inline void operator()( const PxIndexedPropertyInfo< TKey, TObjType, TIndexType, TPropertyType >& inProp, PxU32 )
		{
			readIndexedProperty( *mDeserializer, static_cast< TObjType* >( mObj ), inProp );
		}
	};

	template<typename TObjType>
	void readAllProperties( PxExtensionDeserializerBase& inBase, TObjType* inObj )
	{
		visitAllProperties<TObjType>( PxExtensionDeserializer<TObjType>( &inBase, inObj ) );
	}
}}


#endif