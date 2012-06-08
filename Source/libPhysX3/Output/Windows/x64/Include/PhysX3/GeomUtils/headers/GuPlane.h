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


#ifndef PX_PHYSICS_GEOMUTILS_PX_PLANE
#define PX_PHYSICS_GEOMUTILS_PX_PLANE
/** \addtogroup geomutils
@{
*/

#include "PxVec3.h"
#include "CmPhysXCommon.h"

/**
\brief Representation of a plane.

Plane equation used: a*x + b*y + c*z + d = 0
*/
namespace Gu
{

	class Plane
	{
	public:
		/**
		\brief Constructor
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane()
		{
		}

		/**
		\brief Constructor from a normal and a distance
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane(PxF32 nx, PxF32 ny, PxF32 nz, PxF32 _d)
		{
			set(nx, ny, nz, _d);
		}

		/**
		\brief Constructor from a point on the plane and a normal
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane(const PxVec3& p, const PxVec3& n)
		{
			set(p, n);
		}

		/**
		\brief Constructor from three points
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane(const PxVec3& p0, const PxVec3& p1, const PxVec3& p2)
		{
			set(p0, p1, p2);
		}

		/**
		\brief Constructor from a normal and a distance
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane(const PxVec3& _n, PxF32 _d) : normal(_n), d(_d)
		{
		}

		/**
		\brief Copy constructor
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane(const Plane& plane) : normal(plane.normal), d(plane.d)
		{
		}

		/**
		\brief Destructor
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE ~Plane()
		{
		}

		/**
		\brief Sets plane to zero.
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane& setZero()
		{
			normal = PxVec3(0);
			d = 0.0f;
			return *this;
		}

		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane& set(PxF32 nx, PxF32 ny, PxF32 nz, PxF32 _d)
		{
			normal = PxVec3(nx, ny, nz);
			d = _d;
			return *this;
		}

		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane& set(const PxVec3& _normal, PxF32 _d)
		{
			normal = _normal;
			d = _d;
			return *this;
		}

		PX_CUDA_CALLABLE PX_FORCE_INLINE Plane& set(const PxVec3& p, const PxVec3& _n)
		{
			normal = _n;
			// Plane equation: a*x + b*y + c*z + d = 0
			// p belongs to plane so:
			//     a*p.x + b*p.y + c*p.z + d = 0
			// <=> (n|p) + d = 0
			// <=> d = - (n|p)
			d = - p.dot(_n);
			return *this;
		}

		/**
		\brief Computes the plane equation from 3 points.
		*/
		PX_CUDA_CALLABLE Plane& set(const PxVec3& p0, const PxVec3& p1, const PxVec3& p2)
		{
			const PxVec3 Edge0 = p1 - p0;
			const PxVec3 Edge1 = p2 - p0;

			normal = Edge0.cross(Edge1);
			normal.normalize();

			// See comments in set() for computation of d
			d = -p0.dot(normal);

			return	*this;
		}

		/***
		\brief Computes distance, assuming plane is normalized
		\sa normalize
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE PxF32 distance(const PxVec3& p) const
		{
			// Valid for plane equation a*x + b*y + c*z + d = 0
			return p.dot(normal) + d;
		}

		PX_CUDA_CALLABLE PX_FORCE_INLINE bool belongs(const PxVec3& p) const
		{
			return PxAbs(distance(p)) < (1.0e-7f);
		}

		/**
		\brief projects p into the plane
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE PxVec3 project(const PxVec3& p) const
		{
			// Pretend p is on positive side of plane, i.e. plane.distance(p)>0.
			// To project the point we have to go in a direction opposed to plane's normal, i.e.:
			return p - normal * distance(p);
//			return p + normal * distance(p);
		}

		/**
		\brief find an arbitrary point in the plane
		*/
		PX_CUDA_CALLABLE PX_FORCE_INLINE PxVec3 pointInPlane() const
		{
			// Project origin (0,0,0) to plane:
			// (0) - normal * distance(0) = - normal * ((p|(0)) + d) = -normal*d
			return - normal * d;
//			return normal * d;
		}

		PX_CUDA_CALLABLE PX_FORCE_INLINE void normalize()
		{
			const PxF32 Denom = 1.0f / normal.magnitude();
			normal.x	*= Denom;
			normal.y	*= Denom;
			normal.z	*= Denom;
			d			*= Denom;
		}

		PX_CUDA_CALLABLE PX_FORCE_INLINE operator PxVec3() const
		{
			return normal;
		}

		PxVec3	normal;		//!< The normal to the plane
		PxF32	d;			//!< The distance from the origin
	};
}

/** @} */
#endif
