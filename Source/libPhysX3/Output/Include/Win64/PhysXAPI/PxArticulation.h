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


#ifndef PX_PHYSICS_NX_ARTICULATION
#define PX_PHYSICS_NX_ARTICULATION
/** \addtogroup physics 
@{ */

class PxArticulationLink;

#include "PxPhysX.h"
#include "common/PxSerialFramework.h"

class PxArticulation
// PX_SERIALIZATION
	: public PxSerializable
//~PX_SERIALIZATION
{
// PX_SERIALIZATION
	protected:
								PxArticulation(PxRefResolver& v) : PxSerializable(v)	{}
								PX_DECLARE_SERIAL_RTTI(PxArticulation, PxSerializable)
//~PX_SERIALIZATION
	PX_INLINE					PxArticulation() {}
	virtual						~PxArticulation()	{}

	public:
	/**
	\brief Deletes the articulation.
	
	Do not keep a reference to the deleted instance.

	@see PxScene::createArticulation()
	*/
	virtual		void			release() = 0;

	/**
	\brief Retrieves the scene which this articulation belongs to.

	\return Owner Scene. NULL if not part of a scene.

	@see PxScene
	*/
	virtual		PxScene*		getScene()	const = 0;


	/**
	\brief sets maxProjectionIterations.

	This is the maximum number of iterations to run projection on the articulation to bring
	the links back together if the separation  tolerance is exceeded.


	\param[in] iterations the maximum number of projection iterations 
	<b>Default:</b> 4

	@see getMaxProjectionIterations()
	*/
	virtual		void			setMaxProjectionIterations(PxU32 iterations) = 0;

	/**
	\brief gets maxProjectionIterations.

	\return the maximum number of projection iterations

	@see setMaxProjectionIterations()
	*/

	virtual		PxU32			getMaxProjectionIterations() const = 0;

	/**
	\brief sets separationTolerance.

	This is the maximum allowed separation of any joint in the articulation before projection is used

	<b>Default: 0.1f, scaled by the tolerance scale </b>

	\param[in] tolerance the separation tolerance for the articulation

	@see getSeparationTolerance()
	*/
	virtual		void			setSeparationTolerance(PxReal tolerance) = 0;

	/**
	\brief gets separationTolerance.

	\return the separation tolerance

	@see setSeparationTolerance()
	*/

	virtual		PxReal			getSeparationTolerance() const = 0;


	/**
	\brief sets the number of iterations used to compute the drive response to internal forces

	The drive model uses an iterative algorithm to determine the load on each joint of the articulation. 
	This is the number of iterations to use when computing response of the drive to internal forces.

	\param[in] iterations the number of iterations used to compute the drive response to internal forces.
	
	<b>Default:</b> 4

	@see getInternalDriveIterations()
	*/
	virtual		void			setInternalDriveIterations(PxU32 iterations) = 0;

	/**
	\brief gets internal driveIterations.

	\return the number of iterations used to compute the drive response to internal forces

	@see setInternalDriveIterations()
	*/

	virtual		PxU32			getInternalDriveIterations() const = 0;


	/**
	\brief sets the number of iterations for drive response to external forces.

	The drive model uses an iterative algorithm to determine the load on each joint of the articulation. 
	This is the number of iterations to use when computing response of the drive to external forces.

	\param[in] iterations the number of iterations used to compute the drive response to external forces.

	<b>Default:</b> 4

	@see getExternalDriveIterations()
	*/

	virtual		void			setExternalDriveIterations(PxU32 iterations) = 0;

	/**
	\brief gets externalDriveIterations.

	\return the number of iterations used to compute the drive response to external forces

	@see setExternalDriveIterations()
	*/

	virtual		PxU32			getExternalDriveIterations() const = 0;


	/**
	\brief adds a link to the articulation with default attribute values.

	\param[in] parent the parent link of the articulation. Should be NULL if (and only if) this is the root link
	\param[in] pose the pose of the new link

	\return the new link, or NULL if the link cannot be created because the articulation has reached
	its maximum link count
	
	@see PxsArticulationLink
	*/

	virtual			PxArticulationLink*			createLink(PxArticulationLink* parent, const PxTransform& pose) = 0;


	/**
	\brief returns the number of links in the articulation
	*/

	virtual		PxU32			getNbLinks() const = 0;

	/**
	\brief returns the set of links in the articulation

	\param[in] userBuffer buffer into which to write an array of articulation link pointers
	\param[in] bufferSize the size of the buffer. If this is not large enough to contain all the pointers to links,
	only as many as will fit are written.

	\return the number of links written into the buffer.

	@see PxsArticulationLink
	*/

	virtual		PxU32			getLinks(PxArticulationLink** userBuffer, PxU32 bufferSize) const = 0;

	/**
	\brief Sets a name string for the object that can be retrieved with getName().
	
	This is for debugging and is not used by the SDK. The string is not copied by the SDK, 
	only the pointer is stored.

	\param[in] name String to set the objects name to.

	@see getName()
	*/
	virtual		void			setName(const char* name)		= 0;

	/**
	\brief Retrieves the name string set with setName().

	\return Name string associated with object.

	@see setName()
	*/
	virtual		const char*		getName()			const	= 0;

	/**
	\brief Retrieves the axis aligned bounding box enclosing the articulation.

	\return The articulation's bounding box.

	@see PxBounds3
	*/
	virtual		PxBounds3		getWorldBounds() const = 0;

	/**
	\brief Retrieves the aggregate the articulation might be a part of.

	\return The aggregate the articulation is a part of, or NULL if the articulation does not belong to an aggregate.

	@see PxAggregate
	*/
	virtual		PxAggregate*	getAggregate() const = 0;

	//public variables:
				void*			userData;	//!< user can assign this to whatever, usually to create a 1:1 relationship with a user object.
};

/** @} */
#endif
