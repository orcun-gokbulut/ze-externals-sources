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


#ifndef PX_PHYSICS_NX_RIGIDACTORDESC
#define PX_PHYSICS_NX_RIGIDACTORDESC
/** \addtogroup physics
@{
*/

#include "PxActorDesc.h"
#include "deprecated/PxShapeDesc.h"

/**
\brief Base descriptor for rigid body type actors.
*/

class PxRigidActorDesc : public PxActorDesc
{
public:

	/**
	\brief Pointer to an array of shape descriptors from which to construct the actor.

	See the notes for #PxRigidStatic and #PxRigidDynamic for more details regarding shapes.

	@see PxRigidActor::createShape()
	*/
	PxPtrArray<const PxShapeDesc> shapes;

	/**
	\brief Returns true if the descriptor is valid.

	\return True if the current settings are valid
	*/
	PX_INLINE virtual bool isValid() const;

	/**
	\brief Set the descriptor shape array and count
	*/
	PX_INLINE void setShapes(const PxShapeDesc*const* shapes_, PxU32 shapeCount_)
	{
		shapes.set( shapes_, shapeCount_ );
	}

	/**
	\brief Set the descriptor shape array and count
	*/
	PX_INLINE void setSingleShape(const PxShapeDesc& shape)
	{
		shapes.setSingle( &shape );
	}

protected:

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE PxRigidActorDesc(PxActorType::Enum t);

};



PX_INLINE PxRigidActorDesc::PxRigidActorDesc(PxActorType::Enum t) : 
    PxActorDesc(t)
{
}

PX_INLINE bool PxRigidActorDesc::isValid() const
{
	if ( shapes.isValid() == false )
		return false;

	for (unsigned i = 0; i < shapes.getCount(); i++)
	{
		if (!shapes[i]->isValid())
			return false;
	}

	return PxActorDesc::isValid();
}

/** @} */
#endif
