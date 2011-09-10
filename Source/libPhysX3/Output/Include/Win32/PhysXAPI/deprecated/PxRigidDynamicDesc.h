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


#ifndef PX_PHYSICS_NX_RIGIDDYNAMICDESC
#define PX_PHYSICS_NX_RIGIDDYNAMICDESC
/** \addtogroup physics
@{
*/

#include "deprecated/PxRigidBodyDesc.h"
#include "PxRigidDynamic.h"
#include "PxPhysics.h"
#include "common/PxTolerancesScale.h"

/**
\brief Dynamic rigid body descriptor. This structure is used to save and load the state of #PxRigidDynamic objects.

@see PxRigidBodyDesc
*/

class PxRigidDynamicDesc : public PxRigidBodyDesc
{
public:

	/**
	\brief The pose of the actor in the world.

	<b>Range:</b> rigid body transform<br>
	<b>Default:</b> Identity transform

	@see PxRigidDynamic::setGlobalPose()
	*/
	PxTransform				globalPose;

	/**
	\brief Linear damping applied to the body

	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 0.0

	@see PxRigidDynamic.getLinearDamping() PxRigidDynamic.setLinearDamping() angularDamping
	*/
	PxReal					linearDamping;

	/**
	\brief Angular damping applied to the body

	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 0.05

	@see PxRigidDynamic.setAngularDamping() PxRigidDynamic.getAngularDamping() linearDamping
	*/
	PxReal					angularDamping;

	/**
	\brief Linear Velocity of the body

	<b>Range:</b> velocity vector<br>
	<b>Default:</b> Zero

	@see PxRigidDynamic.setLinearVelocity() PxRigidDynamic.getLinearVelocity() angularVelocity
	*/
	PxVec3					linearVelocity;

	/**
	\brief Angular velocity of the body

	<b>Range:</b> angular velocity vector<br>
	<b>Default:</b> Zero

	@see PxRigidDynamic.setAngularVelocity() PxRigidDynamic.getAngularVelocity() linearVelocity
	*/
	PxVec3					angularVelocity;

	/**
	\brief Maximum allowed angular velocity 

	<b>Range:</b> (0,inf)<br>

	Note: The angular velocity is clamped to the set value <i>before</i> the solver, which means that
	the limit may still be momentarily exceeded.

	@see PxRigidDynamic.setMaxAngularVelocity() PxRigidDynamic.getMaxAngularVelocity()
	*/
	PxReal					maxAngularVelocity;

	/**
	\brief Threshold for the energy-based sleeping algorithm.
	The threshold is scaled internally by the body's mass.

	<b>Range:</b> [0, inf)<br>
	<b>Default:</b> 0.005
	*/
	PxReal					sleepEnergyThreshold;

	/**
	\brief Damping factor for bodies that are about to sleep.

	<b>Range:</b> [0, inf)<br>
	<b>Default:</b> 0
	*/
	PxReal					sleepDamping;

	/**
	\brief The body's initial wake up counter.

	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 20.0f*0.02f

	@see PxRigidDynamic.wakeUp() PxRigidDynamic.putToSleep()
	*/
	PxReal					wakeUpCounter;

	/**
	\brief Minimum number of position iterations performed when processing joint/contacts connected to this body.

	Increase the number of position iterations if joint chains or stacks are unstable.

	<b>Range:</b> [1,255]<br>
	<b>Default:</b> 4

	@see PxRigidDynamic.setSolverIterationCounts() PxRigidDynamic.getSolverIterationCounts()
	*/
	PxU32					minPositionIterations;

	/**
	\brief Minimum number of velocity iterations performed when processing joint/contacts connected to this body.

	Increase the number of velocity iterations if intersecting objects are being depenetrated too violently.

	<b>Range:</b> [1,255]<br>
	<b>Default:</b> 1

	@see PxRigidDynamic.setSolverIterationCounts() PxRigidDynamic.getSolverIterationCounts()
	*/
	PxU32					minVelocityIterations;

	/**
	\brief The force threshold for contact reports.

	<b>Range:</b> (0,inf)<br>
	<b>Default:</b> PX_MAX_REAL

	@see PxRigidDynamic.setContactReportThreshold() PxRigidDynamic.getContactReportThreshold()
	*/
	PxReal					contactReportThreshold;

	/**
	\brief Combination of ::PxRigidDynamicFlag flags

	<b>Default:</b> 0

	@see PxRigidDynamicFlag PxRigidDynamic::setRigidDynamicFlag() PxRigidDynamic::getRigidDynamicFlags()
	*/
	PxRigidDynamicFlags		rigidDynamicFlags;

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void setToDefault(const PxTolerancesScale& scale);

	/**
	\brief Returns true if the descriptor is valid.

	\return True if the current settings are valid
	*/
	PX_INLINE bool isValid() const;

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE PxRigidDynamicDesc(const PxTolerancesScale& scale);
};



PX_INLINE PxRigidDynamicDesc::PxRigidDynamicDesc(const PxTolerancesScale& scale) 
    : PxRigidBodyDesc(PxActorType::eRIGID_DYNAMIC)
{
	globalPose				= PxTransform::createIdentity();
	linearDamping			= 0.0f;
	angularDamping			= 0.05f;
	linearVelocity			= PxVec3(0);
	angularVelocity			= PxVec3(0);
	maxAngularVelocity		= 7.0f;
	sleepEnergyThreshold	= 5e-5f * scale.speed * scale.speed;
	sleepDamping			= 0.0f;
	wakeUpCounter			= 20.0f*0.02f;
	minPositionIterations   = 4;
	minVelocityIterations	= 1;
	contactReportThreshold  = PX_MAX_REAL;
	rigidDynamicFlags		= PxRigidDynamicFlags();
}

PX_INLINE void PxRigidDynamicDesc::setToDefault(const PxTolerancesScale& scale)
{
	*this = PxRigidDynamicDesc(scale);
}

PX_INLINE bool PxRigidDynamicDesc::isValid() const
{
	if(!globalPose.isFinite())
		return false;
	if (linearDamping < 0.0f) //must be nonnegative
		return false;
	if (angularDamping < 0.0f) //must be nonnegative
		return false;
	if (wakeUpCounter < 0.0f) //must be nonnegative
		return false;
	if (minPositionIterations < 1) //must be positive
		return false;
	if (minPositionIterations > 255) 
		return false;
	if (minVelocityIterations < 1) //must be positive
		return false;
	if (minVelocityIterations > 255) 
		return false;
	if (contactReportThreshold < 0.0f) //must be nonnegative
		return false;

	if(maxAngularVelocity<0.0f)
		return false;
	if(sleepEnergyThreshold<0.0f)
		return false;


	return PxRigidBodyDesc::isValid();
}

/** @} */
#endif
