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


#ifndef PX_PHYSICS_NX_ARTICULATION_LINK_DESC
#define PX_PHYSICS_NX_ARTICULATION_LINK_DESC
/** \addtogroup physics
@{
*/

#include "PxPhysX.h"
#include "deprecated/PxArticulationJointDesc.h"
#include "deprecated/PxRigidBodyDesc.h"

/**
\brief Articulation link descriptor. This structure is used to save and load the state of #PxArticulationLink objects.

@see PxArticulation.createLink()
*/

class PxArticulationLinkDesc : public PxRigidBodyDesc
{
public:

	/**
	\brief world pose of this articulation link

	@see PxArticulationJointDesc PxArticulationLink.getGlobalPose() PxArticulationLink.setGlobalPose()
	*/
	PxTransform					globalPose;

	/**
	\brief linear velocity

	@see PxArticulationJointDesc PxArticulationLink.getLinearVelocity() PxArticulationLink.setLinearVelocity()
	*/
	PxVec3						linearVelocity;

	/**
	\brief linear velocity

	@see PxArticulationJointDesc PxArticulationLink.getAngularVelocity() PxArticulationLink.setAngularVelocity()
	*/
	PxVec3						angularVelocity;



	/**
	\brief Joint connecting this articulation link to its parent

	@see PxArticulationJointDesc PxArticulationLink.getInboundJoint()
	*/
	PxArticulationJointDesc*	joint;

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
	PX_INLINE PxArticulationLinkDesc(const PxTolerancesScale& scale);
};



PX_INLINE PxArticulationLinkDesc::PxArticulationLinkDesc(const PxTolerancesScale& scale) : 
	PxRigidBodyDesc(PxActorType::eARTICULATION_LINK),
	globalPose(PxTransform::createIdentity()),
	linearVelocity(PxVec3(0)),
	angularVelocity(PxVec3(0))
{
	joint				= 0;
}

PX_INLINE void PxArticulationLinkDesc::setToDefault(const PxTolerancesScale& scale)
{
	*this = PxArticulationLinkDesc(scale);
}

PX_INLINE bool PxArticulationLinkDesc::isValid() const
{
	if (joint && !joint->isValid())
		return false;

	if(!globalPose.isValid())
		return false;

	return PxRigidBodyDesc::isValid();
}

/** @} */
#endif
