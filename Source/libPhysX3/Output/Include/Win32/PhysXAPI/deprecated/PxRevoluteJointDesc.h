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


#ifndef PX_REVOLUTEJOINTDESC_H
#define PX_REVOLUTEJOINTDESC_H

#include "common/PxTolerancesScale.h"
#include "deprecated/PxJointDesc.h"
#include "extensions/PxJointLimit.h"
#include "extensions/PxRevoluteJoint.h"

class PxRevoluteJointDesc: public PxJointDesc
{
public:

	/**
	\brief the limit parameters for the joint

	@see PxJointLimitPair
	*/

	PxJointLimitPair	limit;

	/**
	\brief the linear tolerance above which projection occurs

	If the joint separates by more than this distance along its locked degrees of freedom, the solver 
	will move the bodies to close the distance.

	Setting a very small tolerance may result in simulation jitter or other artifacts.

	Sometimes it is not possible to project (for example when the joints form a cycle).

	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 1e10f

	*/

	PxReal				projectionLinearTolerance;

	/**
	\brief the angular tolerance above which projection occurs

	If the joint deviates by more than this angle around its locked angular degrees of freedom, 
	the solver will move the bodies to close the angle.
	
	Setting a very small tolerance may result in simulation jitter or other artifacts.

	Sometimes it is not possible to project (for example when the joints form a cycle).

	<b>Range:</b> [0,Pi] <br>
	<b>Default:</b> Pi
	*/

	PxReal				projectionAngularTolerance;
	
	/**
	\brief the target velocity

	The motor will only be able to reach this velocity if the maxForce is sufficiently large.
	If the joint is spinning faster than this velocity, the motor will actually try to brake
	(see PxRevoluteFlags::eDRIVE_FREESPIN.)

	If you set this to infinity then the motor will keep speeding up, unless there is some sort 
	of resistance on the attached bodies. The sign of this variable determines the rotation direction,
	with positive values going the same way as positive joint angles.

	<b>Range:</b> [0,inf]<br>
	<b>Default:</b> PX_MAX_F32

	@see PxRevoluteFlags::eDRIVE_FREESPIN
	*/

	PxReal				driveVelocity;

	/**
	\brief The maximum force (or torque) the drive can exert.
	
	Zero disables the motor.
	Default is 0, should be >= 0. Setting this to a very large value if velTarget is also 
	very large may cause unexpected results.

	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 0.0
	*/

	PxReal				driveForceLimit;

	/**
	\brief The gear ratio of the drive.
	
	This determines ratio between the two actors: for purposes of the drive the velocity of the first actor is scaled by this value,
	and its response to drive torque is scaled down.

	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 1.0
	*/

	PxReal				driveGearRatio;	

	/**
	\brief the flags specific to the revolute joint.

	@see PxRevoluteJointFlag
	*/

	PxRevoluteJointFlags revoluteJointFlags;



	/**
	\brief constructor sets to default.
	*/

	PxRevoluteJointDesc(const PxTolerancesScale &scale);

	/**
	\brief (re)sets the structure to the default.	
	*/

	void	setToDefault(const PxTolerancesScale &scale);

	/**
	\brief Returns true if the descriptor is valid.

	\return True if the current settings are valid
	*/

	bool	isValid() const;



};


PX_INLINE PxRevoluteJointDesc::PxRevoluteJointDesc(const PxTolerancesScale &scale): 
	PxJointDesc(PxJointType::eREVOLUTE),
	projectionLinearTolerance(1e10f),
	projectionAngularTolerance(PxPi),
	driveVelocity(0),
	driveForceLimit(PX_MAX_F32),
	driveGearRatio(1.0f),
	limit(-PxPi/2, PxPi/2, 0.05f)
{
}

PX_INLINE void PxRevoluteJointDesc::setToDefault(const PxTolerancesScale &scale)
{
	*this = PxRevoluteJointDesc(scale);
}

PX_INLINE bool PxRevoluteJointDesc::isValid() const
{
	if(!PxJointDesc::isValid())
		return false;

	return limit.isValid() && 
		   PxIsFinite(projectionLinearTolerance) &&
		   PxIsFinite(projectionAngularTolerance) &&
		   PxIsFinite(driveVelocity) &&
		   driveForceLimit >= 0 &&
		   driveGearRatio >= 0 &&
		   PxAbs(projectionAngularTolerance)<=PxPi;
}

#endif