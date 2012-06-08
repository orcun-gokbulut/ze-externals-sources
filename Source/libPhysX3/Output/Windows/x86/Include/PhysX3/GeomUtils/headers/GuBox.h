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


#ifndef PX_PHYSICS_GEOMUTILS_BOX
#define PX_PHYSICS_GEOMUTILS_BOX

/** \addtogroup geomutils
@{
*/

#include "CmMatrix34.h"
#include "PxMat33Legacy.h"
#include "PxTransform.h"
#include "CmPhysXCommon.h"


namespace Gu
{
	class Capsule;

	void computeOBBPoints(PxVec3* PX_RESTRICT pts, const PxVec3& center, const PxVec3& extents, const PxVec3& base0, const PxVec3& base1, const PxVec3& base2);

	/**
	\brief Represents an oriented bounding box. 

	As a center point, extents(radii) and a rotation. i.e. the center of the box is at the center point, 
	the box is rotated around this point with the rotation and it is 2*extents in width, height and depth.
	*/

	/**
	Box geometry

	The rot member describes the world space orientation of the box.
	The center member gives the world space position of the box.
	The extents give the local space coordinates of the box corner in the positive octant.
	Dimensions of the box are: 2*extent.
	Transformation to world space is: worldPoint = rot * localPoint + center
	Transformation to local space is: localPoint = T(rot) * (worldPoint - center)
	Where T(M) denotes the transpose of M.
	*/
	class Box
	{
	public:
		/**
		\brief Constructor
		*/
		PX_INLINE Box()
		{
		}

		/**
		\brief Constructor

		\param _center Center of the OBB
		\param _extents Extents/radii of the obb.
		\param _rot rotation to apply to the obb.
		*/
		PX_INLINE Box(const PxVec3& _center, const PxVec3& _extents, const PxMat33Legacy& _rot) : center(_center), extents(_extents)
		{
			_rot.getColumnMajor(&rot.column0.x);
		}

		//! Construct from center, extent and rotation
		PX_INLINE Box(const PxVec3& origin, const PxVec3& extent, const PxMat33& base) : rot(base), center(origin), extents(extent)
		{}

		//! construct from a matrix(center and rotation) + extent
		PX_INLINE Box(const Cm::Matrix34& mat, const PxVec3& extent) : rot(PxMat33(mat.base0, mat.base1, mat.base2)), center(mat.base3), extents(extent)
		{}

		//! Copy constructor
		PX_INLINE Box(const Box& other) : rot(other.rot), center(other.center), extents(other.extents)
		{}

		/**
		\brief Destructor
		*/
		PX_INLINE ~Box()
		{
		}

		//! Assignment operator
		PX_INLINE const Box& operator=(const Box& other)
		{
			rot		= other.rot;
			center	= other.center;
			extents	= other.extents;
			return *this;
		}

		/**
		\brief Setups an empty box.
		*/
		PX_INLINE void setEmpty()
		{
			center = PxVec3(0);
			extents = PxVec3(-PX_MAX_REAL, -PX_MAX_REAL, -PX_MAX_REAL);
			rot = PxMat33::createIdentity();
		}

		/**
		\brief Checks the box is valid.

		\return	true if the box is valid
		*/
		PX_INLINE bool isValid() const
		{
			// Consistency condition for (Center, Extents) boxes: Extents >= 0.0f
			if(extents.x < 0.0f)	return false;
			if(extents.y < 0.0f)	return false;
			if(extents.z < 0.0f)	return false;
			return true;
		}

/////////////
		PX_FORCE_INLINE	void	setAxes(const PxVec3& axis0, const PxVec3& axis1, const PxVec3& axis2)
		{
			rot.column0 = axis0;
			rot.column1 = axis1;
			rot.column2 = axis2;
		}

		PX_INLINE	PxMat33Legacy			getRotLegacy()	const
		{
			PxMat33Legacy legacy;
			legacy.setColumn(0, rot.column0);
			legacy.setColumn(1, rot.column1);
			legacy.setColumn(2, rot.column2);
			return legacy;			
		}

		PX_INLINE	void					setRotLegacy(const PxMat33Legacy& legacy)
		{
			legacy.getColumn(0, rot.column0);
			legacy.getColumn(1, rot.column1);
			legacy.getColumn(2, rot.column2);
		}

		PX_INLINE	PxVec3	rotate(const PxVec3& src)	const
		{
			return rot * src;
		}

		PX_INLINE	PxVec3	rotateInv(const PxVec3& src)	const
		{
			return rot.transformTranspose(src);
		}

		PX_INLINE	PxVec3	transform(const PxVec3& src)	const
		{
			return rot * src + center;
		}

		PX_INLINE	PxTransform getTransform()	const
		{
			return PxTransform(center, PxQuat(rot));
		}

		PX_INLINE PxVec3 computeAABBExtent() const
		{
			const PxReal a00 = PxAbs(rot[0][0]);
			const PxReal a01 = PxAbs(rot[0][1]);
			const PxReal a02 = PxAbs(rot[0][2]);

			const PxReal a10 = PxAbs(rot[1][0]);
			const PxReal a11 = PxAbs(rot[1][1]);
			const PxReal a12 = PxAbs(rot[1][2]);

			const PxReal a20 = PxAbs(rot[2][0]);
			const PxReal a21 = PxAbs(rot[2][1]);
			const PxReal a22 = PxAbs(rot[2][2]);

			const PxReal ex = extents.x;
			const PxReal ey = extents.y;
			const PxReal ez = extents.z;

			return PxVec3(	a00 * ex + a10 * ey + a20 * ez,
							a01 * ex + a11 * ey + a21 * ez,
							a02 * ex + a12 * ey + a22 * ez);
		}

		/**
		Computes the obb points.
		\param		pts	[out] 8 box points
		\return		true if success
		*/
		PX_INLINE void computeBoxPoints(PxVec3* PX_RESTRICT pts) const
		{
			Gu::computeOBBPoints(pts, center, extents, rot.column0, rot.column1, rot.column2);
		}

		/**
		\brief recomputes the OBB after an arbitrary transform by a 4x4 matrix.
		\param	mtx		[in] the transform matrix
		\param	obb		[out] the transformed OBB
		*/
		PX_INLINE	void rotate(const Cm::Matrix34& mtx, Box& obb)	const
		{
			// The extents remain constant
			obb.extents = extents;
			// The center gets x-formed
			obb.center = mtx.transform(center);
			// Combine rotations
			obb.rot = PxMat33(mtx.base0, mtx.base1, mtx.base2) * rot;
		}

		void create(const Gu::Capsule& capsule);

		/**
		\brief checks the OBB is inside another OBB.
		\param		box		[in] the other OBB
		*/
		physx::shdfnd3::IntBool isInside(const Box& box)	const;

		PxMat33	rot;
		PxVec3	center;
		PxVec3	extents;
	};
	PX_COMPILE_TIME_ASSERT(sizeof(Gu::Box) == 60);
}

/** @} */
#endif
