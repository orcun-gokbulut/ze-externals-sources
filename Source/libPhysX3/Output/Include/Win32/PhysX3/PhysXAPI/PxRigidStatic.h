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


#ifndef PX_PHYSICS_NX_RIGIDSTATIC
#define PX_PHYSICS_NX_RIGIDSTATIC
/** \addtogroup physics
@{
*/

#include "PxPhysX.h"
#include "PxRigidActor.h"


/**
\brief PxRigidStatic represents a static rigid body simulation object in the physics SDK.

PxRigidStatic objects are static rigid physics entities. They shall be used to define solid objects which are fixed in the world.

<h3>Creation</h3>
Instances of this class are created by calling #PxPhysics::createRigidStatic() and deleted with #release().

<h3>Visualizations</h3>
\li #PxVisualizationParameter::eACTOR_AXES

@see PxRigidActor  PxPhysics.createRigidStatic  release()
*/

class PxRigidStatic : public PxRigidActor
{
	protected:
// PX_SERIALIZATION
								PxRigidStatic(PxRefResolver& v) : PxRigidActor(v)		{}
								PX_DECLARE_SERIAL_RTTI(PxRigidStatic, PxRigidActor)
//~PX_SERIALIZATION
	PX_INLINE					PxRigidStatic() : PxRigidActor() {}
	virtual						~PxRigidStatic()	{}

	public:
	/**
	\brief Deletes the static rigid body.
	
	Note: deleting a static rigid body will not wake up any sleeping objects that were
	sitting on it. Use a kinematic rigid body instead to get this behavior.

	Calls #PxRigidActor::release() so you might want to check the documentation of that method as well.

	@see PxPhysics::createRigidStatic() PxScene::createRigidStatic() PxRigidActor::release()
	*/
	virtual		void			release() = 0;

	// Runtime modifications


/************************************************************************************************/
/** @name Global Pose Manipulation
*/


};

/** @} */
#endif