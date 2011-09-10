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

#ifndef PX_DISTANCEJOINTDESC_H
#define PX_DISTANCEJOINTDESC_H

#include "common/PxTolerancesScale.h"
#include "deprecated/PxJointDesc.h"
#include "extensions/PxDistanceJoint.h"

class PxDistanceJointDesc;



class PxDistanceJointDesc: public PxJointDesc
{
public:
   	/**
	\brief the minimum distance allowed between the origins of the joint frames. The minimum
	distance must be no more than the maximum distance

	<b>Default</b> 0.0f
	<b>Range</b> [0, +inf)

	@see maxDistance PxDistanceJointFlag::eMIN_ENABLED
	*/

  	PxReal					minDistance;

   	/**
	\brief the maximum distance allowed between the origins of the joint frames. The maximum
	distance must be no less than the minimum distance. 

	<b>Default</b> 0.0f
	<b>Range</b> [0, +inf)

	@see minDistance PxDistanceJointFlag::eMAX_ENABLED
	*/

	PxReal					maxDistance;

   	/**
	\brief spring strength of the joint if the distance exceeds the [min, max] range and the spring
	is enabled

	<b>Default</b> 0.0f
	<b>Range</b> [0, +inf)

	@see PxDistanceJointFlag::eSPRING_ENABLED tolerance
	*/
	PxReal					spring;


   	/**
	\brief spring strength of the joint if the distance exceeds the [min, max] range

	<b>Default</b> 0.0f
	<b>Range</b> [0, +inf)

	@see PxDistanceJointFlag::eSPRING_ENABLED tolerance
	*/
	PxReal					damping;


   	/**
	\brief the distance beyond the joint's [min, max] range before the joint becomes active.

	<b>Default</b> 0.25f * PxTolerancesScale::length
	<b>Range</b> (0, +inf)
	*/
	PxReal					tolerance;

   	/**
	\brief the flags specific to the distance joint.

	<b>Default</b> PxDistanceJointFlag::eMAX_DISTANCE_ENABLED

	@see PxDistanceJointFlag
	*/

	PxDistanceJointFlags	jointFlags;

	/**
	\brief constructor sets to default.
	*/
			PxDistanceJointDesc(const PxTolerancesScale &scale);

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

PX_INLINE PxDistanceJointDesc::PxDistanceJointDesc(const PxTolerancesScale &scale): 
  PxJointDesc(PxJointType::eDISTANCE),
  minDistance(0),
  maxDistance(0),
  spring(0),
  damping(0),
  tolerance(0.025f * scale.length),
  jointFlags(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED)
  { }

PX_INLINE void PxDistanceJointDesc::setToDefault(const PxTolerancesScale &scale)
{
	*this = PxDistanceJointDesc(scale);
}

PX_INLINE bool PxDistanceJointDesc::isValid() const
{
	if(!PxJointDesc::isValid())
		return false;
	if(minDistance<0 || maxDistance<0)
		return false;
	if(spring<0 || damping <0)
		return false;
	if(tolerance <= 0)
		return false;
	return true;
}


#endif