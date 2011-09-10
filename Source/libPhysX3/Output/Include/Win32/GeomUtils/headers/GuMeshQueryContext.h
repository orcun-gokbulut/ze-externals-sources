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


#ifndef PX_PHYSICS_GEOMUTILS_PX_MESHQUERYCONTEXT
#define PX_PHYSICS_GEOMUTILS_PX_MESHQUERYCONTEXT

/** \addtogroup geomutils
  @{
*/

#include "CmPhysXCommon.h"


namespace Gu
{

	/**
	\brief MeshQueryContext is used to extract the indices of overlapping triangles when calling the mesh overlap queries
	#overlapTriangleMesh(), #overlapConvexMesh(), #overlapConvexMesh().

	@see MeshQuery MeshQuery.createMeshQueryContext()
	*/
	class MeshQueryContext
	{
	protected:
		PX_INLINE			MeshQueryContext() {}
		virtual				~MeshQueryContext()	{}

	public:
		/**
		\brief Release this instance.

		@see MeshQuery.createMeshQueryContext()
		*/
		virtual void		release() = 0;

		/**
		\brief Get index array of overlapping triangles of the last processed mesh query.

		@see MeshQuery.overlapTriangleMesh() MeshQuery.overlapConvexMesh() MeshQuery.overlapHeightField()
		*/
		virtual PxU32*		getTriangleIndices() = 0;

		/**
		\brief Get number of overlapping triangles of the last processed mesh query.

		@see getTriangleIndices()
		*/
		virtual PxU32		getNbTriangleIndices() = 0;

		/**
		\brief Release memory of triangle index buffer.

		If the triangle index buffer gets too large, use this method to free the memory

		@see getTriangleIndices()
		*/
		virtual void		freeIndexBuffer() = 0;

		/**
		\brief Reserve space for the index buffer.
		
		\note Invalidates existing buffer content.

		\param[in] triGeom Geometry of the triangle mesh to extract the triangle from.
		\return True if requested space could be allocated, else false

		@see getTriangleIndices()
		*/
		virtual bool		reserveIndexBuffer(PxU32 size) = 0;

		/**
		\brief Get capacity of index buffer.
		
		\return Maximum number of indices that can be stored in the buffer

		@see getTriangleIndices()
		*/
		virtual PxU32		getIndexBufferCapacity() = 0;
	};


}  // namespace Gu


/** @} */

#endif
