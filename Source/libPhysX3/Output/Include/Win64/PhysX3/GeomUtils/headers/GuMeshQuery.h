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


#ifndef PX_PHYSICS_GEOMUTILS_PX_MESH_QUERY
#define PX_PHYSICS_GEOMUTILS_PX_MESH_QUERY

/** \addtogroup geomutils
  @{
*/

#include "CmPhysXCommon.h"
#include "PxPhysXGeomUtils.h"
#include "PxTriangleFlags.h"

class PxGeometry;
class PxConvexMeshGeometry;
class PxTriangleMeshGeometry;
class PxHeightFieldGeometry;


namespace Gu
{
	class MeshQueryContext;
	class Triangle;

	class MeshQuery
	{
	public:

		/**
		\brief Creates MeshQueryContext object for performing mesh queries.

		\note Call #MeshQueryContext.release() to delete a created instance.

		\return Pointer to MeshQueryContext object.

		@see MeshQueryContext overlapTriangleMesh() overlapConvexMesh() overlapHeightField()
		*/
		static Gu::MeshQueryContext* createMeshQueryContext();


		/**
		\brief Retrieves triangle data from a triangle ID.

		This function can be used together with #overlapTriangleMesh() to retrieve triangle properties.

		\param[in] triGeom Geometry of the triangle mesh to extract the triangle from.
		\param[in] transform Transform for the triangle mesh
		\param[in] triangleIndex The index of the triangle to retrieve.
		\param[out] triangle Triangle points in world space.
		\param[out] edgeTri World space edge normals for triangle (NULL to not compute).
		\param[out] flags Flags which show if an edge is convex. See #PxTriangleFlags

		@see Triangle PxTriangleFlags PxTriangleID overlapTriangleMesh()
		*/
		static void getTriangle(const PxTriangleMeshGeometry& triGeom, const PxTransform& transform, PxTriangleID triangleIndex, Gu::Triangle& triangle, Gu::Triangle* edgeTri, PxU32* flags);


		/**
		\brief Retrieves triangle data from a triangle ID.

		This function can be used together with #overlapHeightField() to retrieve triangle properties.

		\param[in] hfGeom Geometry of the height field to extract the triangle from.
		\param[in] transform Transform for the height field.
		\param[in] triangleIndex The index of the triangle to retrieve.
		\param[out] triangle Triangle points in world space.
		\param[out] edgeTri World space edge normals for triangle (NULL to not compute).
		\param[out] flags Flags which show if an edge is convex. See #PxTriangleFlags

		@see Triangle PxTriangleFlags PxTriangleID overlapHeightField()
		*/
		static void getTriangle(const PxHeightFieldGeometry& hfGeom, const PxTransform& transform, PxTriangleID triangleIndex, Gu::Triangle& triangle, Gu::Triangle* edgeTri, PxU32* flags);


		/**
		\brief Find the mesh triangles which touch the specified geometry object.

		The triangle indices returned through the MeshQueryContext object can be used with #getTriangle() to retrieve the triangle properties.

		\param[in] geom0 The geometry object to test for mesh triangle overlaps. Supported geometries are #PxSphereGeometry, #PxCapsuleGeometry and #PxBoxGeometry
		\param[in] pose0 Pose of the geometry object
		\param[in] geom1 The triangle mesh geometry to check overlap against
		\param[in] pose1 Pose of the triangle mesh
		\param[in/out] context MeshQueryContext object to store the indices of the overlapping triangles
		\param[in] firstContact True to report boolean results and early exit as soon as there's a hit, false to return all touched triangles
		\return True if there is an overlap between the mesh and the other geometry object, else false

		@see PxTriangleMeshGeometry MeshQueryContext getTriangle()
		*/
		static bool overlapTriangleMesh(const PxGeometry& geom0,
										const PxTransform& pose0,
										const PxTriangleMeshGeometry& geom1,
										const PxTransform& pose1,
										Gu::MeshQueryContext& context,
										bool firstContact);


		/**
		\brief Find if the convex mesh touches the specified geometry object.

		\param[in] geom0 The geometry object to test for convex mesh overlaps. Supported geometries are #PxSphereGeometry, #PxCapsuleGeometry and #PxBoxGeometry
		\param[in] pose0 Pose of the geometry object
		\param[in] geom1 The convex mesh geometry to check overlap against
		\param[in] pose1 Pose of the convex mesh
		\return True if there is an overlap between the mesh and the other geometry object, else false

		@see PxConvexMeshGeometry 
		*/
		static bool overlapConvexMesh(const PxGeometry& geom0,
									  const PxTransform& pose0,
									  const PxConvexMeshGeometry& geom1,
									  const PxTransform& pose1);


		/**
		\brief Find the height field triangles which touch the specified geometry object.

		The triangle indices returned through the MeshQueryContext object can be used with #getTriangle() to retrieve the triangle properties.

		\param[in] geom0 The geometry object to test for height field overlaps. Supported geometries are #PxBoxGeometry
		\param[in] pose0 Pose of the geometry object
		\param[in] geom1 The height field geometry to check overlap against
		\param[in] pose1 Pose of the height field
		\param[in/out] context MeshQueryContext object to store the indices of the overlapping triangles
		\return True if there is an overlap between the height field and the other geometry object, else false

		@see PxHeightFieldGeometry MeshQueryContext getTriangle()
		*/
		static bool overlapHeightField(const PxGeometry& geom0,
									   const PxTransform& pose0,
									   const PxHeightFieldGeometry& geom1,
									   const PxTransform& pose1,
									   Gu::MeshQueryContext& context);
	};


}  // namespace Gu


/** @} */

#endif
