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


#ifndef PX_PHYSICS_NX_RIGIDSTATICDESC
#define PX_PHYSICS_NX_RIGIDSTATICDESC
/** \addtogroup physics
@{
*/

#include "common/PxTolerancesScale.h"
#include "deprecated/PxRigidActorDesc.h"

/**
\brief Static rigid body descriptor. This structure is used to save and load the state of #PxRigidStatic objects.

\note Shapes should	be specified for the static actor when it is created.

\note If you want to create a temporarily static actor that can be made dynamic at runtime, create a dynamic body
instead (see #PxRigidDynamic) and use PxRigidDynamicFlag::eKINEMATIC in its descriptor.
*/

class PxRigidStaticDesc : public PxRigidActorDesc
{
public:

	/**
	\brief The pose of the actor in the world.

	<b>Range:</b> rigid body transform<br>
	<b>Default:</b> Identity transform

	@see PxRigidStatic::setGlobalPose()
	*/
	PxTransform					globalPose; 

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE virtual void setToDefault(const PxTolerancesScale& scale);

	/**
	\brief Returns true if the descriptor is valid.

	\return True if the current settings are valid
	*/
	PX_INLINE virtual bool isValid() const;

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE PxRigidStaticDesc(const PxTolerancesScale& scale);
};



PX_INLINE PxRigidStaticDesc::PxRigidStaticDesc(const PxTolerancesScale& scale) : PxRigidActorDesc(PxActorType::eRIGID_STATIC)
{
	globalPose = PxTransform::createIdentity();
}

PX_INLINE void PxRigidStaticDesc::setToDefault(const PxTolerancesScale& scale)
{
	*this = PxRigidStaticDesc(scale);
}

PX_INLINE bool PxRigidStaticDesc::isValid() const
{
	if(!globalPose.isFinite())
		return false;

	return PxRigidActorDesc::isValid();
}

/** @} */
#endif
