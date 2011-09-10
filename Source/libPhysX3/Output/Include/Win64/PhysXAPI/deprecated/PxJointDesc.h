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


#ifndef PX_JOINTDESC_H
#define PX_JOINTDESC_H

#include "common/PxTolerancesScale.h"
#include "extensions/PxJoint.h"
#include "extensions/PxJointLimit.h"

class PxJointDesc
{
public:
	PxRigidActor*		actor[2];
	PxTransform			localPose[2];
	PxReal				breakForce;
	PxReal				breakTorque;
	PxConstraintFlags	constraintFlags;
	void*				userData;
	const char*			name;

	PxJointType::Enum	getType() const		{	return mType;	}

	bool isValid() const
	{
		if(actor[0] == 0 && actor[1] == 0)
			return false;
		if(breakForce<0 || breakTorque<0)
			return false;
		if(!localPose[0].isValid() || !localPose[1].isValid())
			return false;
		return true;
	}

	void setGlobalPose(const PxTransform& globalPose)
	{
		PX_ASSERT(globalPose.isValid());
		localPose[0] = actor[0] ? actor[0]->getGlobalPose().transformInv(globalPose) : globalPose;
		localPose[1] = actor[1] ? actor[1]->getGlobalPose().transformInv(globalPose) : globalPose;
	}

protected:

	PxJointDesc(PxJointType::Enum t):
	    breakForce(PX_MAX_REAL),
		breakTorque(PX_MAX_REAL),
		userData(0),
		name(0),
		mType(t)

	{
		actor[0] = 0;
		actor[1] = 0;
		localPose[0] = PxTransform::createIdentity();
		localPose[1] = PxTransform::createIdentity();
	}

	PxJointType::Enum		mType;
};

PxJoint*			PxJointCreate(PxPhysics& physics, const PxJointDesc& desc);

#endif