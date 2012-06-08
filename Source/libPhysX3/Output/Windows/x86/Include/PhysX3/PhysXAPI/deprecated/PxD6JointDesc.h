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


#ifndef PX_D6JOINTDESC_H
#define PX_D6JOINTDESC_H

#include "common/PxTolerancesScale.h"
#include "deprecated/PxJointDesc.h"
#include "extensions/PxJointLimit.h"
#include "extensions/PxD6Joint.h"

#pragma warning(push)
#pragma warning(disable:4996)	// We have to use deprecated functionality, do not warn.

class PxD6JointDesc: public PxJointDesc
{
public:

	/**
	\brief The motion type around each axis.

	Each axis may independently specify that the degree of freedom is locked (blocking relative movement
	along or around this axis), limited by the corresponding limit, or free
	
	<b>Default</b> PxD6Motion::eLOCKED

	@see PxD6Motion
	*/

	PxD6Motion::Enum	motion[6]; 
	
	/**
	\brief the linear limit for the joint. 

	A single limit constraints all linear limited degrees of freedom, forming a linear, circular 
	or spherical constraint on motion depending on the number of limited degrees.

	@see PxJointLimit;
	*/

	PxJointLimit		linearLimit; 

	/**
	\brief Set the swing limit for the joint. 

	The swing limit is used if either swing degree of freedom is limited, but not both.
	
	The swing limit is parameterized by the rotation vector between the constraint frames. If SWING1 is limited
	the y-component of the rotation vector is constrained, and if SWING2 is limited, the z-component is constrained.

	@see PxJointLimit;
	*/

	PxJointLimitCone	swingLimit;	
	
	
	/**
	\brief the twist limit. 

	The twist limit controls the range of motion around the twist axis. Upper and lower values 
	may be specified.

	@see PxJointLimitPair;
	*/

	PxJointLimitPair	twistLimit;	

	/**
	\brief the drive type for the specified degree of freedom. 

	@see PxD6Drive
	*/
	PxD6JointDrive		drive[PxD6Drive::eCOUNT];

	/**
	\brief the target drive pose. 

	The drive target pose is specified relative to the actor[0] joint frame

	<b>Default</b> The identity transform
	*/
	PxTransform			drivePosition;

	/**
	\brief the target drive linear velocity. 

	Drive velocity vectors are specified relative to the actor[0] joint frame.

	<b>Default</b> the zero vector
	*/
	PxVec3				driveLinearVelocity;

	/**
	\brief the target drive angular velocity. 

	Drive velocity vectors are specified relative to the actor[0] joint frame.

	<b>Default</b> the zero vector
	*/
	PxVec3				driveAngularVelocity;

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
	\brief constructor sets to default.
	*/

				PxD6JointDesc(const PxTolerancesScale& scale);

	/**
	\brief (re)sets the structure to the default.	
	*/

	void		setToDefault(const PxTolerancesScale& scale);

	/**
	\brief Returns true if the descriptor is valid.

	\return True if the current settings are valid
	*/

	bool		isValid()		const;
};

PX_INLINE PxD6JointDesc::PxD6JointDesc(const PxTolerancesScale& scale)
  : PxJointDesc(PxJointType::eD6),
	drivePosition(PxTransform::createIdentity()),
	driveLinearVelocity(PxVec3(0)),
	driveAngularVelocity(PxVec3(0)),
	projectionLinearTolerance(1e10),
	projectionAngularTolerance(PxPi),
	swingLimit(PxPi/2, PxPi/2, 0.05f),
	twistLimit(-PxPi/2, PxPi/2, 0.05f),
	linearLimit(PX_MAX_F32, 0.05f*scale.length)
{
	for (PxU32 i = 0; i < 6; i++)
		motion[i] = PxD6Motion::eLOCKED;
}

#pragma warning(pop)

PX_INLINE void PxD6JointDesc::setToDefault(const PxTolerancesScale &scale)
{
	*this = PxD6JointDesc(scale);
}

PX_INLINE bool PxD6JointDesc::isValid() const
{
	if(!PxJointDesc::isValid())
		return false;

	if(!linearLimit.isValid())
		return false;

	if(!swingLimit.isValid())
		return false;

	if(!twistLimit.isValid())
		return false;

	for(PxU32 i=0;i<PxD6Drive::eCOUNT;i++)
	{
		if(!drive[i].isValid())
			return false;
	}

	if(!drivePosition.isValid())
		return false;

	if(!driveLinearVelocity.isFinite() || !driveAngularVelocity.isFinite())
		return false;

	if(!PxIsFinite(projectionLinearTolerance) || !PxIsFinite(projectionAngularTolerance))
		return false;

	if(PxAbs(projectionAngularTolerance)>PxPi)
		return false;

	return true;
}

#endif