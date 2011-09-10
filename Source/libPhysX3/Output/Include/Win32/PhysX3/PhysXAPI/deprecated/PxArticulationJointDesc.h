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


#ifndef PX_PHYSICS_NX_ARTICULATION_JOINT_DESC
#define PX_PHYSICS_NX_ARTICULATION_JOINT_DESC
/** \addtogroup physics
@{
*/

#include "PxPhysX.h"
#include "PxBounds3.h"

/**
\brief Articulation joint descriptor. This structure is used to save and load the state of #PxArticulationJoint objects.

@see PxArticulationLinkDesc.joint
*/

class PxArticulationJointDesc
{
public:

	/**
	\brief

	@see PxArticulationJoint.getParentPose()

	<b>Default:</b> the identity matrix
	*/
	PxTransform					parentPose;

	/**
	\brief

	@see PxArticulationJoint.getChildPose()

	<b>Default:</b> the identity matrix
	*/
	PxTransform					childPose;

	/**
	\brief the target position for the joint drive, measured in the parent constraint frame

	@see PxArticulationJoint.setTargetOrientation() PxArticulationJoint.getTargetOrientation()
	*/
	PxQuat						targetPosition;

	/**
	\brief the target velocity for the joint drive, measured in the parent constraint frame

	@see PxArticulationJoint.setTargetVelocity() PxArticulationJoint.getTargetVelocity()
	*/
	PxVec3						targetVelocity;

	/**
	\brief the strength of the joint acceleration spring

	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 0.0
	*/
	PxReal						spring;

	/**
	\brief the damping for the joint acceleration spring
	
	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 0.0
	*/
	PxReal						damping;

	/**
	\brief resistance to internal forces of the articulation
	
	<b> Range: [0,1)</b>
	<b> Default:</b> 0.0
	*/
	PxReal						internalCompliance;

	/**
	\brief resistance to external forces

	<b> Range: [0,inf)</b>
	<b> Default:</b> 0.0
	*/
	PxReal						externalCompliance;

	/**
	\brief the extent of the cone limit rotation around the Y-axis

	<b> Range: [0,Pi)</b>
	<b> Default:</b> 0.0
	*/
	PxReal						swingLimitY;

	/**
	\brief the extent of the cone limit rotation around the Z-axis

	<b> Range: [0,Pi)</b>
	<b> Default:</b> 0.0
	*/
	PxReal						swingLimitZ;

	/**
	\brief whether the swing limit is enabled

	<b> Default:</b> false
	*/
	bool						swingLimitEnabled;

	/**
	\brief the lower extent of the twist limit

	<b> Range: [0,Pi)</b>
	<b> Default:</b> 0.0
	*/
	PxReal						twistLimitLow;

	/**
	\brief the upper extent of the twist limit

	<b> Range: [0,Pi)</b>
	<b> Default:</b> 0.0
	*/
	PxReal						twistLimitHigh;

	/**
	\brief whether the twist limit is enabled

	<b> Default:</b> false
	*/
	bool						twistLimitEnabled;

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
	PX_INLINE PxArticulationJointDesc(const PxTolerancesScale& scale);
};



PX_INLINE PxArticulationJointDesc::PxArticulationJointDesc(const PxTolerancesScale& scale)
{
	parentPose			= PxTransform::createIdentity();
	childPose			= PxTransform::createIdentity();

	targetPosition		= PxQuat::createIdentity();
	targetVelocity		= PxVec3(0);

	spring				= 0.0f;
	damping				= 0.0f;

	internalCompliance	= 1.0f;
	externalCompliance	= 1.0f;

	swingLimitY			= 0.0f;
	swingLimitZ			= 0.0f;
	swingLimitEnabled	= false;

	twistLimitLow		= 0.0f;
	twistLimitHigh		= 0.0f;
	twistLimitEnabled	= false;
}

PX_INLINE void PxArticulationJointDesc::setToDefault(const PxTolerancesScale& scale)
{
	*this = PxArticulationJointDesc(scale);
}

PX_INLINE bool PxArticulationJointDesc::isValid() const
{
	if (!parentPose.isValid())
		return false;

	if (!childPose.isValid())
		return false;

	if (!targetPosition.isValid())
		return false;

	if (spring < 0.0f)
		return false;

	if (damping < 0.0f)
		return false;

	return true;
}

/** @} */
#endif
