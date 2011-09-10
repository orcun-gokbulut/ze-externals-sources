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


#ifndef PX_SPHERICALJOINTDESC_H
#define PX_SPHERICALJOINTDESC_H

#include "common/PxTolerancesScale.h"
#include "deprecated/PxJointDesc.h"
#include "extensions/PxJointLimit.h"

class PxSphericalJointDesc;

/**
 \brief A spherical joint constrains two points on two bodies to coincide. The
 point on each body is specified by the origin of that body's joint frame.

 \image html sphericalJoint.png

 <h3>Creation</h3>

 The bodies may be projected together if the joint distance exceeds a given threshold.
 Projection is activated by setting the ePROJECTION flag.

 @see PxJoint
*/


class PxSphericalJointDesc: public PxJointDesc
{
public:
    /** \brief the limit cone for the joint

	If enabled, the limit cone will constrain the angular movement of the joint to lie
	within an elliptical cone.

	@see PxJointLimitCone
	*/

	PxJointLimitCone		coneLimit;

	/**
	\brief the linear tolerance above which projection occurs

	If the joint separates by more than this distance along its locked degrees of freedom, the solver 
	will move the bodies to close the distance.

	Setting a very small tolerance may result in simulation jitter or other artifacts.

	Sometimes it is not possible to project (for example when the joints form a cycle).

	<b>Range:</b> [0,inf)<br>
	<b>Default:</b> 1e10f
	*/

	PxReal					projectionLinearTolerance;	

	PxSphericalJointFlags	sphericalJointFlags;

	/**
	\brief constructor sets to default.
	*/
			PxSphericalJointDesc(const PxTolerancesScale &scale);

	/**
	\brief (re)sets the structure to the default.	
	*/

	void	setToDefault(const PxTolerancesScale &scale);

	/**
	\brief Returns true if the descriptor is valid.

	\return True if the current settings are valid
	*/

	bool	isValid()	const;
};

PX_INLINE PxSphericalJointDesc::PxSphericalJointDesc(const PxTolerancesScale &scale): 
    PxJointDesc(PxJointType::eSPHERICAL),
    projectionLinearTolerance(1e10f),
	coneLimit(PxPi/2, PxPi/2, 0.05f)
{
}

PX_INLINE void PxSphericalJointDesc::setToDefault(const PxTolerancesScale &scale)
{
	*this = PxSphericalJointDesc(scale);
}

PX_INLINE bool PxSphericalJointDesc::isValid() const
{
	if(!PxJointDesc::isValid())
		return false;

	if(!coneLimit.isValid())
		return false;

	if(projectionLinearTolerance < 0)
		return false;

	return true;
}
#endif