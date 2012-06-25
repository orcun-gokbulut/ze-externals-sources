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


#ifndef PX_PHYSICS_NX_SCENEQUERYREPORT
#define PX_PHYSICS_NX_SCENEQUERYREPORT
/** \addtogroup scenequery
@{
*/
#include "PxPhysX.h"
#include "PxVec3.h"
#include "PxFlags.h"

class PxShape;
class PxActor;

/**
\brief actor/shape output struct of scenequeries

@see PxBatchQuery
*/
struct PxSqHitObject
{
	PxActor*	actor;		//!< Hit actor
	PxShape*	shape;		//!< Hit shape, only nonzero if Actor contains Shapes.
};


/**
\brief Scene query specification.

A PxSceneQueryFlag parameter to a scene query call determines which optional fields in the output #PxSceneQueryHit are to be filled in.
In #PxSceneQueryHit it is used to inform which optional fields have been filled in.

The PxSceneQueryFlag::eINITIAL_OVERLAP controls the query behavior when the swept shape initially overlaps with a shape of the scene.
If the flag is set, additional overlap tests are performed to detect the initial overlaps. If you can guarantee your swept shape starts
from an empty portion of space, it is best (more efficient) to disable that flag. In case an initial overlap is detected, returned
distance is set to zero, and returned normal is set to the opposite of the sweep direction.

@see PxSceneQueryHit PxScene.raycastSingle PxScene.raycastMultiple, PxScene.sweepSingle, PxScene.sweepMultiple
*/
struct PxSceneQueryFlag
{
	enum Enum
	{
		eIMPACT				= (1<<0),	//!< "impact" member of #PxSceneQueryHit is valid
		eNORMAL				= (1<<1),	//!< "normal" member of #PxSceneQueryHit is valid
		eDISTANCE			= (1<<2),	//!< "distance" member of #PxSceneQueryHit is valid
		eUV					= (1<<3),	//!< "u" and "v" barycentric coordinates of #PxSceneQueryHit are valid. Note: Not applicable for sweep queries.
		eINITIAL_OVERLAP	= (1<<4),	//!< Enable/disable initial overlap tests in sweeps. Also mark returned hits as initially overlapping.
		eTOUCHING_HIT		= (1<<5),	//!< specified the hit object as a touching hit. See also #PxSceneQueryHitType
		eBLOCKING_HIT		= (1<<6),	//!< specified the hit object as a blocking hit. See also #PxSceneQueryHitType
	};
};

/**
\brief collection of set bits defined in PxSceneQueryFlag.

@see PxSceneQueryFlag
*/
typedef PxFlags<PxSceneQueryFlag::Enum> PxSceneQueryFlags;
PX_FLAGS_OPERATORS(PxSceneQueryFlag::Enum,PxU32);

/**
\brief Scene query hit information.

All members of the PxSceneQueryHit structure are not always available. For example when the query hits a sphere,
the faceIndex member is not computed.
*/
struct PxSceneQueryHit
{
	PxShape*			shape;		//!< Hit shape, only nonzero if Actor contains Shapes.
	PxActor*			actor;		//!< Hit actor
	PxU32				faceIndex;	//!< Face index of touched triangle, for triangle mesh and height field. Note: These are post cooking indices, use #PxTriangleMesh::getTrianglesRemap() to get the indices of the original mesh
	PxSceneQueryFlags	flags;		//!< Hit flags specifying which optional members are valid.
};

/**
\brief Scene query hit information for queries with impacts.

::PxSceneQueryFlag flags can be passed to scene query functions, as an optimization, to cause the SDK to
only generate specific members of this structure.
*/
struct PxSceneQueryImpactHit : public PxSceneQueryHit
{
	// the following fields are set in accordance with the #PxSceneQueryFlags

	PxVec3				impact;		//!< World-space impact point (flag: #PxSceneQueryFlag::eIMPACT)
	PxVec3				normal;		//!< World-space impact normal (flag: #PxSceneQueryFlag::eNORMAL)
	PxF32				distance;	//!< Distance to hit (flag: #PxSceneQueryFlag::eDISTANCE)
};

/**
\brief This structure captures results for a single sweep query.

See #PxBatchQuery and #PxScene for sweep methods.
*/
struct PxSweepHit : public PxSceneQueryImpactHit
{
	PxU32		sweepGeometryIndex;	//!< Only valid when using PxBatchQuery::linearCompoundGeometrySweepSingle() or PxScene::SweepAny/Single/Multiple() with compound of geometry objects. Index of the geometry object that hits the hitObject.
};

/**
\brief Raycast hit information.

::PxSceneQueryFlag flags can be passed to raycasting functions, as an optimization, to cause the SDK to
only generate specific members of this structure.

Some members like barycentric coordinates are currently only computed for triangle meshes and convexes, but next versions
might provide them in other cases. The client code should check #flags to make sure returned values are
relevant.

@see PxBatchQuery.raycastMultiple PxBatchQuery.raycastSingle PxScene.raycastAny PxScene.raycastSingle PxScene.raycastMultiple
*/
struct PxRaycastHit : public PxSceneQueryImpactHit
{
	// the following fields are set in accordance with the #PxSceneQueryFlags

	PxReal	u, v;			//!< barycentric coordinates of hit point, for triangle mesh and height field (flag: #PxSceneQueryFlag::eUV)

	PxU32	padding[3];		//!< Padding to make PxRaycastHit 16bytes aligned
};

/**
\brief Specifies the behaviour after a query result.

@see PxBatchQuery
*/
struct PxSceneQueryReportResult
{
	enum Enum
	{
		eCONTINUE,		//!< Continue reporting more results
		eABORT_QUERY,	//!< Stop reporting results for current query

		eFORCE_DWORD	=	0x7fffffff
	};
};

/** @} */
#endif