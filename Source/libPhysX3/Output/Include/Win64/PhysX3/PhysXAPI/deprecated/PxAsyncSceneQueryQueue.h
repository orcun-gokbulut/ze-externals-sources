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


#ifndef PX_PHYSICS_EXTENSIONS_ASYNCSCENEQUERYQUEUE_H
#define PX_PHYSICS_EXTENSIONS_ASYNCSCENEQUERYQUEUE_H

#include "Px.h"

class PxBatchQuery;
class PxSceneQueryManager;

/**
\brief Descriptor for the PxAsyncQueryQueue
*/
class PxAsyncSceneQueryQueueDesc
{
public:
	PX_INLINE	PxAsyncSceneQueryQueueDesc();
	PX_INLINE	void setToDefault();

	/**
	\brief Sets maximum size of the queue

	<b>Default:</b> 64
	*/
	PxU32					maxQueueSize;


	/**
	\brief Allows the user to specify which (logical) processor to allocate threads to.

	The SDK will allocate threads to processors corresponding to bits which are set. Starting from
	the least significant bit.

	This flag is ignored for platforms which do not associate threads exclusively to specific processors.

	\note The XBox 360 associates threads with specific processors.
	\note The special value of 0 lets the SDK determine the thread affinity.
	<b>Default:</b> 0
	*/
	PxU32					threadMask;

	/**
	\brief Allows the user to specify the stack size for the threads created by the SDK.

	The value is specified in bytes and rounded to an appropriate size by the operating system.

	NOTE: If you increase the stack size for all threads that call the SDK, you may want to call

	Specifying a value of zero will cause the SDK to choose a platform specific default value.
	
	\li PC SW - OS default
	\li PS3 - OS default
	\li XBox 360 - OS default
	*/
	PxU32					threadStackSize;
};

PX_INLINE PxAsyncSceneQueryQueueDesc::PxAsyncSceneQueryQueueDesc()	//constructor sets to default
{
	maxQueueSize	= 64;
	threadMask		= 0;
	threadStackSize	= 0;
}

PX_INLINE void PxAsyncSceneQueryQueueDesc::setToDefault()
{
	*this = PxAsyncSceneQueryQueueDesc();
}

/**
\brief A helper class to support asynchronous queries
@see PxBatchQuery
*/
class PxAsyncSceneQueryQueue
{
public:
	/**
	\brief Adds a scene query batch object to the queue.

	Call will block if queue is full and blocking is set to true.
	Returns true if non-blocking add was successful. 

	@see PxBatchQuery
	*/
	virtual	bool	addBatchQuery(PxBatchQuery&, bool block = true) = 0;
    
	/**
	\brief Check if all scene queries in the queue have finished
	*/
	virtual	bool	isQueueFinished(bool block = true) = 0;

	/**
	\brief Returns the current number tasks in the queue.

	Use stallQueue to access the value in a thread safe manner.
	
	@see stallQueue() resumeQueue()
	*/
	virtual	PxU32	getSize() = 0;
    
	/**
	\brief Stall the queue such that the user can modify the SDK without queries running in the background.

	Returns true if non-blocking call was successful. 
	*/
	virtual	bool	stallQueue(bool block = true) = 0;

	/**
	\brief Resume a stalled queue
	*/
	virtual	void	resumeQueue() = 0;

	/**
	\brief Waits till all jobs are finished, and then releases the threads and the allocated memory.
	*/
	virtual	void	release() = 0;

protected:
	virtual	~PxAsyncSceneQueryQueue() {}
};


/*PX_PHYSX_CORE_API*/ PxAsyncSceneQueryQueue*	PxAsyncSceneQueryQueueCreate(const PxAsyncSceneQueryQueueDesc& desc);

#endif
