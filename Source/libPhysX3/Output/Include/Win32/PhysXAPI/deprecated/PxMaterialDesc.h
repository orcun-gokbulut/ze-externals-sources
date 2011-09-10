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


#ifndef PX_PHYSICS_NXMATERIALDESC
#define PX_PHYSICS_NXMATERIALDESC
/** \addtogroup physics
@{
*/

#include "PxPhysX.h"
#include "PxVec3.h"
#include "PxFlags.h"
#include "common/PxCoreUtilityTypes.h"
#include "PxMaterial.h"

/**
\brief Descriptor of #PxMaterial.

@see PxMaterial PxPhysics.createMaterial()
*/
class PxMaterialDesc
{
public:
	/**
	coefficient of dynamic friction -- should be in [0, +inf]. If set to greater than staticFriction, the effective value of staticFriction will be increased to match.
	if flags & PxMaterialFlag::eANISOTROPIC is set, then this value is used for the primary direction of anisotropy (U axis)

	<b>Range:</b> [0,inf]<br>
	<b>Default:</b> 0.0

	@see flags frictionCombineMode
	*/
	PxReal	dynamicFriction;

	/**
	coefficient of static friction -- should be in [0, +inf]
	if flags & PxMaterialFlag::eANISOTROPIC is set, then this value is used for the primary direction of anisotropy (U axis)

	<b>Range:</b> [0,inf]<br>
	<b>Default:</b> 0.0

	@see flags frictionCombineMode
	*/
	PxReal	staticFriction;

	/**
	coefficient of restitution --  0 makes the object bounce as little as possible, higher values up to 1.0 result in more bounce.
	Note that values close to or above 1 may cause stability problems and/or increasing energy.
	<b>Range:</b> [0,1]<br>
	<b>Default:</b> 0.0

	@see flags restitutionCombineMode
	*/
	PxReal	restitution;

	/**
	anisotropic dynamic friction coefficient for along the secondary (V) axis of anisotropy. 
	This is only used if flags & PxMaterialFlag::eANISOTROPIC is set.

	<b>Range:</b> [0,inf]<br>
	<b>Default:</b> 0.0

	@see flags dynamicFriction
	*/
	PxReal dynamicFrictionV;

	/**
	anisotropic static  friction coefficient for along the secondary (V) axis of anisotropy. 
	This is only used if flags & PxMaterialFlag::eANISOTROPIC is set.

	<b>Range:</b> [0,inf]<br>
	<b>Default:</b> 0.0

	@see flags staticFriction
	*/
	PxReal staticFrictionV;

	/**
	shape space direction (unit vector) of anisotropy.
	This is only used if flags & PxMaterialFlag::eANISOTROPIC is set.

	<b>Range:</b> direction vector<br>
	<b>Default:</b> 1.0f,0.0f,0.0f

	@see flags staticFrictionV dynamicFrictionV
	*/
	PxVec3 dirOfAnisotropy;

	/**
	Friction combine mode. See the enum ::PxCombineMode .

	<b>Default:</b> PxCombineMode::eAVERAGE

	@see PxCombineMode staticFriction dynamicFriction
	*/
	PxCombineMode::Enum frictionCombineMode;

	/**
	Restitution combine mode. See the enum ::PxCombineMode .

	<b>Default:</b> PxCombineMode::eAVERAGE

	@see PxCombineMode restitution
	*/
	PxCombineMode::Enum restitutionCombineMode;

	/**
	Flags, a combination of the bits defined by the enum ::PxMaterialFlag . 

	<b>Default:</b> 0

	@see PxMaterialFlag PxMaterialFlags
	*/
	PxMaterialFlags flags;
	PxPadding<2>	paddingFromFlags;		// because flags are 16bits

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE PxMaterialDesc(const PxTolerancesScale& scale);	
	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void setToDefault(const PxTolerancesScale& scale);
	/**
	\brief Returns true if the descriptor is valid.

	\return true if the current settings are valid
	*/
	PX_INLINE bool isValid() const;
};

PX_INLINE PxMaterialDesc::PxMaterialDesc(const PxTolerancesScale& scale)
{
	dynamicFriction	= 0.0f;
	staticFriction	= 0.0f;
	restitution		= 0.0f;


	dynamicFrictionV= 0.0f;
	staticFrictionV = 0.0f;

	dirOfAnisotropy = PxVec3(1,0,0);
	flags = PxMaterialFlags();
	frictionCombineMode = PxCombineMode::eAVERAGE;
	restitutionCombineMode = PxCombineMode::eAVERAGE;
}

PX_INLINE	void PxMaterialDesc::setToDefault(const PxTolerancesScale& scale)
{
	*this = PxMaterialDesc(scale);
}

PX_INLINE	bool PxMaterialDesc::isValid()	const
{
	if(dynamicFriction < 0.0f) 
		return false;
	if(staticFriction < 0.0f) 
		return false;
	if(restitution < 0.0f || restitution > 1.0f) 
		return false;


	if (flags & PxMaterialFlag::eANISOTROPIC)
	{
		if(dynamicFrictionV < 0.0f) 
			return false;
		if(staticFrictionV < 0.0f) 
			return false;
		if(!dirOfAnisotropy.isNormalized())
			return false;
	}

	if (frictionCombineMode >= PxCombineMode::eN_VALUES)
		return false;
	if (restitutionCombineMode >= PxCombineMode::eN_VALUES)
		return false;

	return true;
}

/** @} */
#endif
