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


#ifndef PX_PHYSICS_NX_ARTICULATIONDESC
#define PX_PHYSICS_NX_ARTICULATIONDESC
/** \addtogroup physics
@{
*/

#include "PxPhysX.h"
#include "deprecated/PxArticulationLinkDesc.h"
#include "PxPhysics.h"
#include "common/PxTolerancesScale.h"
#include "PxTransform.h"

/**
\brief Articulation descriptor. This structure is used to save and load the state of #PxArticulation objects.

@see PxScene.createArticulation()
*/

class PxArticulationDesc
{
public:

	/**
	\brief Number of iterations to use when computing response to external forces

	<b>Default:</b> 4

	@see PxArticulation.setInternalDriveIterations() PxArticulation.getInternalDriveIterations()
	*/

	PxU32									internalDriveIterations;


	/**
	\brief Number of iterations to use when computing response to external forces

	<b>Default:</b> 4

	@see PxArticulation.setInternalDriveIterations() PxArticulation.getExternalDriveIterations()
	*/

	PxU32									externalDriveIterations;


	/**
	\brief Maximum number of iterations to run projection for if separation exceeds tolerance

	<b>Default:</b> 4

	@see PxArticulation.setMaxProjectionIterations() PxArticulation.getMaxProjectionIterations()
	*/
	PxU32									maxProjectionIterations;

	/**
	\brief Maximum separation of any joint in the articulation before projection kicks in

	<b>Default:</b> 4

	@see PxArticulation.setSeparationTolerance() PxArticulation.getSeparationTolerance()
	*/
	PxReal									separationTolerance;


	/**
	\brief Will be copied to PxArticulation::userData

	<b>Default:</b> NULL

	@see PxArticulation.userData
	*/
	void*									userData; 

	/**
	\brief Possible debug name. The string is not copied by the SDK, only the pointer is stored.

	<b>Default:</b> NULL
	*/
	const char*								name;

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
	PX_INLINE PxArticulationDesc(const PxTolerancesScale& scale);
};




PX_INLINE PxArticulationDesc::PxArticulationDesc(const PxTolerancesScale& scale)
{
	internalDriveIterations = 4;
	externalDriveIterations = 4;
	maxProjectionIterations = 4;
	separationTolerance		= 0.1f * scale.length;

	userData			= NULL;
	name				= NULL;
}

PX_INLINE void PxArticulationDesc::setToDefault(const PxTolerancesScale& scale)
{
	*this = PxArticulationDesc(scale);
}

PX_INLINE bool PxArticulationDesc::isValid() const
{
	return true;
}

/** @} */
#endif
