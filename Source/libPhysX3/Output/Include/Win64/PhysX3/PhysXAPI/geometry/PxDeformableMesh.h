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


#ifndef PX_PHYSICS_GEOMUTILS_NX_DEFORMABLEMESH
#define PX_PHYSICS_GEOMUTILS_NX_DEFORMABLEMESH
/** \addtogroup deformable
  @{
*/

#include "geometry/PxDeformableMeshDesc.h"

// PX_SERIALIZATION
#include "common/PxSerialFramework.h"
//~PX_SERIALIZATION

/**
\brief A deformable mesh.

Deformable meshes are suitable for soft body simulation.  They come in two 
flavors: triangle meshes, suitable for simulating a thin shell (such
as a sheet of rubber), and tetrahedron meshes, suitable for simulating 
a volume (such as a blob of jelly).

Instances can be created using PxPhysics.createDeformableMesh().
*/
class PxDeformableMesh
// PX_SERIALIZATION
	: public PxSerializable
//~PX_SERIALIZATION
{
public:
	/**
	\brief Releases the deformable mesh.

	Do not keep a reference to the deleted instance.

	\note This will decrease the reference count.

	@see PxPhysics.createDeformableMesh()
	*/
	virtual void release() = 0;

	/**
	\brief Saves the deformable descriptor. 

	A deformable mesh is created via the cooker. The cooker potentially changes the
	order of the arrays referenced by the pointers PxDeformableMeshDesc.vertices and 
	PxDeformableMeshDesc.primitives. Since saveToDesc returns the data of the cooked mesh, 
	this data might	differ from the originally provided data. Note that this is in contrast 
	to the meshData	member of PxDeformableDesc, which is guaranteed to provide data in the 
	same order as that used to create the mesh.
	
	\param desc The descriptor used to retrieve the state of the object.
	\return True on success.

	@see PxDeformableMeshDesc
	*/
	virtual	bool saveToDesc(PxDeformableMeshDesc& desc) const = 0;

	/**
	\brief Gets the number of deformable instances referencing this deformable mesh.

	@see PxDeformable
	*/
	virtual	PxU32 getReferenceCount() const = 0;

	/**
	\brief Returns the PxDeformablePrimitiveType used to create the deformable.

	@see PxDeformable
	*/
	virtual PxDeformablePrimitiveType::Enum getPrimitiveType() const = 0;

protected:
	// PX_SERIALIZATION
	PxDeformableMesh()										{}
	PxDeformableMesh(PxRefResolver& v)	: PxSerializable(v)	{}
	PX_DECLARE_SERIAL_RTTI(PxDeformableMesh, PxSerializable)
		//~PX_SERIALIZATION
		virtual						~PxDeformableMesh() {}
};
/** @} */
#endif
