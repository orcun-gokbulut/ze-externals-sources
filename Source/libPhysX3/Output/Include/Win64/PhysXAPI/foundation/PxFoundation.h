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


#ifndef PX_FOUNDATION_PXFOUNDATION_H
#define PX_FOUNDATION_PXFOUNDATION_H

/** \addtogroup foundation
  @{
*/

#include "PxErrors.h"

#ifndef PX_NAMESPACELESS
namespace physx
{
namespace pubfnd3
{
#endif

class PxErrorCallback;
class PxAllocatorCallback;
class PxProfilingZone;
class PxBroadcastingAllocator;

/**
\brief Foundation SDK singleton class.

You need to have an instance of this class to instance the higher level SDKs.
*/
class PxFoundation
{
public:
	/**
	retrieves error callback
	*/
	virtual PxErrorCallback& getErrorCallback() const = 0;

	/**
	Sets mask of errors to report.
	*/
	virtual void setErrorLevel(PxErrorCode::Enum mask = PxErrorCode::Enum(~0)) = 0;

	/**
	Retrieves mask of errors to be reported.
	*/
	virtual PxErrorCode::Enum getErrorLevel() const = 0;

	/**
	retrieves the current allocator.
	*/
	virtual PxBroadcastingAllocator& getAllocator() const = 0;
	
	/**
	Retrieves the allocator this object was created with.
	*/
	virtual PxAllocatorCallback& getAllocatorCallback() const = 0;

protected:
	virtual ~PxFoundation() {}
};

#ifndef PX_NAMESPACELESS
} // namespace pubfnd3
} // end namespace physx
#endif

/**
The constant PX_PUBLIC_FOUNDATION_VERSION is used when creating the PxFoundation object, 
which is an internally created object. This is to ensure that the application is using 
the same header version as the library was built with.
*/
#define PX_PUBLIC_FOUNDATION_VERSION ((PX_PUBLIC_FOUNDATION_VERSION_MAJOR   <<24)		\
									 +(PX_PUBLIC_FOUNDATION_VERSION_MINOR   <<16)		\
									 +(PX_PUBLIC_FOUNDATION_VERSION_BUGFIX  <<8) + 0)

 /** @} */
#endif
