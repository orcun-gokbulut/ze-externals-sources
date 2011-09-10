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


#ifndef PX_PHYSICS_NX_SHAPEDESC
#define PX_PHYSICS_NX_SHAPEDESC
/** \addtogroup physics
@{
*/

#include "PxPhysX.h"
#include "geometry/PxGeometry.h"
#include "PxFiltering.h"
#include "PxMaterial.h"
#include "PxTransform.h"
#include "PxPhysics.h"
#include "common/PxTolerancesScale.h"
#include "common/PxCoreUtilityTypes.h"
#include "PxShape.h"



/**
\brief Descriptor for #PxShape class. 

Used for saving and loading the shape state.

@see PxRigidActor.createShape() PxBoxGeometry PxSphereGeometry PxCapsuleGeometry PxPlaneGeometry PxConvexMeshGeometry
PxTriangleMeshGeometry PxHeightFieldGeometry
*/
class PxShapeDesc
{
public:

	/**
	\brief The geometry of the shape.

	@see PxShape.setGeometry PxBoxGeometry PxSphereGeometry PxCapsuleGeometry PxPlaneGeometry PxConvexMeshGeometry
	PxTriangleMeshGeometry PxHeightFieldGeometry
	*/
	PxGeometry*				geometry;

	/**
	\brief The pose of the shape in the coordinate frame of the owning actor.

	<b>Range:</b> rigid body transform<br>
	<b>Default:</b> Identity

	@see PxShape.setLocalPose()
	*/
	PxTransform				localPose;

	/**
	\brief User definable data for simulation related filtering (contact, trigger, CCD setup etc.).

	@see PxShape.setSimulationFilterData(), PxShape.getSimulationFilterData()
	*/
	PxFilterData			simulationFilterData;

	/**
	\brief User definable data for query filtering.

	@see PxShape.setQueryFilterData(), PxShape.getQueryFilterData()
	*/
	PxFilterData			queryFilterData;

	/**
	\brief A combination of ::PxShapeFlag values.

	<b>Default:</b> PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE

	@see PxShape.setFlag() PxShapeFlag
	*/
	PxShapeFlags			flags;

	/**
	\brief The material(s) of the shape.

	\note Basic geometry types only support one material. For mesh based geometry types which support per triangle material, a set of materials
	can be specified. The per triangle material indices of the mesh based geometry will point into the specified material table, hence, 
	make sure that the order of the materials matches. A triangle with material table index n will use the material specified in PxShapeDesc::materials[n].

	\note If more than one material is specified but the shape geometry does not support multiple materials (includes meshes and height fields with only one material),
	all the specified materials except the first one will be discarded.

	<b>Default:</b> NULL

	@see PxPhysics.createMaterial() PxShape.setMaterial()
	*/
	PxPtrArray<PxMaterial> materials;

	/**
	\brief Specifies a skin around the object within which contacts will be generated.

	Objects that come within contactOffset distance of this shape will count as being in contact.  In other words, two shapes generate contacts when they are less than their sum
	contactOffsets' apart.	The contactOffset has to be greater than zero and also be greater the shape's restOffset.
	Having a contactOffset is important so that when the contacting object moves further away than restDistance, it doesn't immediately lose contact, which would cause jitter.

	Units: distance. 

	<b>Range:</b> (0,inf)<br>
	<b>Default:</b> 0.02

	@see PxVisualizationParameter
	*/
	PxReal					contactOffset;

	/**
	\brief Specifies an offset relative to the shape's surface at which other objects will come to rest on it.

	Two shapes will come to rest on top of eachother at a distance equal to the sum of their restOffsets. restOffset must be smaller than contactOffset.

	If the restOffset is 0, they should converge to touching exactly.  Having a restOffset greater than zero is useful to have
	objects slide at a slight distance.  This way they will not get hung up on irregularities of eachothers' surface, and sliding will be smoother.

	\note: In SDK 2.x, the restOffset was specified by -skinWidth, and only negative restOffsets could be specified.  For legacy 2.x default behavior, use: -0.025 for restOffset.

	<b>Range:</b> (-inf,contactOffset)<br>
	<b>Default:</b> 0.0 
	*/
	PxReal					restOffset;

	/**
	\brief Will be copied to PxShape::userData.

	<b>Default:</b> NULL
	*/
	void*					userData;

	/**
	\brief Possible debug name. The string is not copied by the SDK, only the pointer is stored.

	<b>Default:</b> NULL
	*/
	const char*				name;


	/**
	constructor sets to default.
	*/
	PX_INLINE			PxShapeDesc(const PxTolerancesScale& scale);

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE 			void setToDefault(const PxTolerancesScale& scale);

	/**
	\brief Returns true if the descriptor is valid.

	\return true if the current settings are valid
	*/
	PX_INLINE virtual	bool isValid() const;

	/**
	\brief Set the descriptor material array and count
	*/
	PX_INLINE void setMaterials(PxMaterial*const* materials_, PxU32 materialCount_)
	{
		materials.set( materials_, materialCount_ );
	}

	/**
	\brief Set the descriptor material array and count
	*/
	PX_INLINE void setSingleMaterial(PxMaterial* material)
	{
		materials.setSingle( material );
	}

private:		
	void operator=(const PxShapeDesc &) {}
};

PX_INLINE PxShapeDesc::PxShapeDesc(const PxTolerancesScale& scale)
{
	geometry					= NULL;
	localPose					= PxTransform::createIdentity();
	simulationFilterData.setToDefault();
	queryFilterData.setToDefault();
	flags						= PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
	contactOffset				= 0.02f * scale.length;
	restOffset					= 0.0f;
	userData					= NULL;
	name						= NULL;
}

PX_INLINE void PxShapeDesc::setToDefault(const PxTolerancesScale& scale)
{
	*this = PxShapeDesc(scale);
}

PX_INLINE bool PxShapeDesc::isValid() const
{
	if (geometry == NULL)
		return false;
	if(!localPose.isFinite())
		return false;
	if (contactOffset < 0)
		return false;
	if (contactOffset <= restOffset)
		return false;
	if (!materials.isValid() || materials.getCount() == 0 )
		return false;
	if (materials.getCount() >= 0xffff)
		return false;

	return true;
}

/** @} */
#endif
