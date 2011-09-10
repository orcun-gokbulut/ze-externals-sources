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


#ifndef PX_PHYSICS_NX_RIGIDBODYDESC
#define PX_PHYSICS_NX_RIGIDBODYDESC
/** \addtogroup physics
@{
*/

#include "deprecated/PxRigidActorDesc.h"
#include "PxRigidBody.h"

/**
\brief Dynamic rigid body descriptor.

To simulate a dynamic rigid body, the SDK needs a mass and an inertia tensor. 
(The inertia tensor is the combination of PxRigidBodyDesc.massLocalPose and PxRigidBodyDesc.massSpaceInertia)

You can compute and set these properties by calling the PhysX extensions method #PxRigidBodyExt::updateMassAndInertia()
*/

class PxRigidBodyDesc : public PxRigidActorDesc
{
public:

	/**
	\brief position and orientation of the center of mass

	<b>Range:</b> rigid body transform<br>
	<b>Default:</b> Identity transform

	@see PxRigidBody.setCMassLocalPose() PxRigidBody.getCMassLocalPose() massSpaceInertia mass
	*/
	PxTransform				massLocalPose;

	/**
	\brief Diagonal mass space inertia tensor in bodies mass frame.

	Should not be zero.

	<b>Range:</b> inertia vector<br>
	<b>Default:</b> PxVec3(1.0f, 1.0f, 1.0f)

	@see PxRigidBody.setMassSpaceInertiaTensor() PxRigidBody.getMassSpaceInertiaTensor() mass massLocalPose
	*/
	PxVec3					massSpaceInertia;

	/**
	\brief Mass of body

	Should not be zero.

	<b>Range:</b> (0,inf)<br>
	<b>Default:</b> 1.0f

	@see PxRigidBody.setMass() PxRigidBody.getMass() massSpaceInertia
	*/
	PxReal					mass;

	/**
	\brief Returns true if the descriptor is valid.

	\return True if the current settings are valid
	*/
	PX_INLINE bool isValid() const;

protected:

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE PxRigidBodyDesc(PxActorType::Enum t);
};



PX_INLINE PxRigidBodyDesc::PxRigidBodyDesc(PxActorType::Enum t) : PxRigidActorDesc(t)
{
	massLocalPose			= PxTransform::createIdentity();
	mass					= 0.0f;
	massSpaceInertia		= PxVec3(0.0f, 0.0f, 0.0f);
}

PX_INLINE bool PxRigidBodyDesc::isValid() const
{
	if(!massLocalPose.isFinite())
		return false;

// DESCLESS
#if 0
	if(mass<=0.0f)		//must be positive
		return false;
	if(massSpaceInertia.isZero())	//must be non zero
		return false;
#endif
// DESCLESS

	return PxRigidActorDesc::isValid();
}

/** @} */
#endif
