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
#ifndef PVD_PVDCOMMLAYERTYPES_H
#define PVD_PVDCOMMLAYERTYPES_H

#include "PxSimpleTypes.h"
#include "PxAssert.h"
#include "PVDCommLayerErrors.h"
#include "PVDCommLayerDatatypes.h"
#include "PVDCommLayerValue.h"

namespace PVD //PhysX Visual Debugger
{	
	struct SDeleteOperator
	{
		template<typename TObject>
		inline void operator()( TObject* inObject ) { delete inObject; }
	};

	struct EInstanceUIFlags
	{
		enum
		{
			None = 0,
			/**
			 *	This instance can be a top level or root
			 *	instance in the object browser.
			 */
			TopLevel = 1 << 0,
			Hidden = 1 << 1, //Never shows up in the UI
		};
		PxU32 mFlags;
		PX_INLINE EInstanceUIFlags( PxU32 inFlags = None ) : mFlags( inFlags ) {}
		PX_INLINE bool operator==( const EInstanceUIFlags& inOther ) const { return mFlags == inOther.mFlags; }
		PX_INLINE bool operator!=( const EInstanceUIFlags& inOther ) const { return !(*this == inOther); }
		PX_INLINE bool hasFlag( PxU32 inFlag ) { return (mFlags & inFlag) != 0; }
	};
}

#endif