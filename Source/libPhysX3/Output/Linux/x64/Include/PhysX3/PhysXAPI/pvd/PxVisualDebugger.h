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


#ifndef PX_VISUALDEBUGGER_H
#define PX_VISUALDEBUGGER_H


#ifdef PX_SUPPORT_VISUAL_DEBUGGER
#define PX_SUPPORT_VISUAL_DEBUGGER 1
#include "PxSimpleTypes.h"
#include "PxVec3.h"
#else
#define PX_SUPPORT_VISUAL_DEBUGGER 0
#endif

/** \addtogroup pvd
@{
*/

#include "PxTransform.h"

namespace PVD
{
	class PvdDataStream;
	class PvdConnection;
}

#ifndef PX_DOXYGEN
namespace physx
{
#endif

class PxScene;

/**
\brief PVD Flags.
*/
struct PxVisualDebuggerFlags
{
	enum Enum
	{
		eTRANSMIT_CONTACTS	= (1 << 0),	//! Transmits contact stream to PVD. Disabled by default.
	};
};


/**
\brief Class to communicate with the PhysX Visual Debugger.
*/
class PxVisualDebugger
{
public:

	/**
	Disconnects the SDK from the PhysX Visual Debugger application.
	If we are still connected, this will kill the entire debugger connection.
	*/
	virtual void disconnect() = 0;

	/**
	 *	Checks if the connect state is paused. If it is, then this method will not
	 *	return until the connection state changes or pvd disconnects.
	 */
	virtual void checkConnection() = 0;

	/**
	returns the PVD connection factory that was passed to the SDK.
	returns NULL if no connection is present.
	*/
	virtual PVD::PvdConnection* getPvdConnectionFactory() = 0;

	/**
	\param scene The scene of which the PVD connection should be returned.
	returns the PVD connection of a given scene.
	returns NULL if no connection is present.
	*/
	virtual PVD::PvdDataStream* getPvdConnection(const PxScene& scene) = 0;

	/**
	Sets the scaling for PVD's joint visualization.
	/param frameScale Scale of the joint's frame (basis) visualization.
	/param limitScale Scale of the joint's limit visualization.
	*/
	virtual void setJointVisualizationScale(PxReal frameScale, PxReal limitScale) = 0;

	/**
	Sets the PVD flags. See PxVisualDebuggerFlags.
	/param flag Flag to set.
	/param value value the flag gets set to.
	*/
	virtual void setVisualDebuggerFlag(PxVisualDebuggerFlags::Enum flag, bool value) = 0;

	/**
	Retrieves the PVD flags. See PxVisualDebuggerFlags.
	*/
	virtual PxU32 getVisualDebuggerFlags() = 0;

	/**
	Creates a camera in PVD
	\param instanceKey A unique ID for the camera. E.g. the application's this pointer.
	\param name The camera's name. Will be shown in PVD camera drop down menu.
	returns true if the camera was created successfully.
	*/
	virtual bool createCamera(PxU64 instanceKey, const char* name) = 0;

	/**
	Updates the pose of a PVD camera.
	\param instanceKey A unique ID for the camera. E.g. the application's this pointer.
	\param origin The origin of the camera.
	\param up The up vector of the camera.
	\param target The target vector of the camera.
	returns true if operation was successful.
	*/
	virtual bool updateCamera(PxU64 instanceKey, const PxVec3& origin, const PxVec3& up, const PxVec3& target) = 0;

protected:
	virtual ~PxVisualDebugger() {}
};

#ifndef PX_DOXYGEN
} // namespace physx
#endif

/** @} */
#endif // PX_VISUALDEBUGGER_H
