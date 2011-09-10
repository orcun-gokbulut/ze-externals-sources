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


#ifndef PX_PARTICLE_BASE_FLAG
#define PX_PARTICLE_BASE_FLAG
/** \addtogroup particles
  @{
*/

/**
\brief ParticleBase flags
*/
struct PxParticleBaseFlag
{
	enum Enum
	{
		/**
		\brief Enable/disable two way collision of particles with the rigid body scene.
		In either case, particles are influenced by colliding rigid bodies.
		If eCOLLISION_TWOWAY is not set, rigid bodies are not influenced by 
		colliding particles. Use PxParticleBaseDesc.particleMass to
		control the strength of the feedback force on rigid bodies.
		
		\note Switching this flag while the particle system is part of a scene will fail. 

		@see PxParticleBaseDesc.particleMass
		*/
		eCOLLISION_TWOWAY					= (1<<0),

		/**
		\brief Enable/disable execution of particle simulation.
		*/
		eENABLED							= (1<<1),

		/**
		\brief Defines whether the particles of this particle system should be projected to a plane.
		This can be used to build 2D applications, for instance. The projection
		plane is defined by the parameter PxParticleBaseDesc.projectionPlaneNormal and PxParticleBaseDesc.projectionPlaneDistance.

		@see PxParticleBaseDesc.projectionPlaneNormal PxParticleBaseDesc.projectionPlaneDistance
		*/
		ePROJECT_TO_PLANE					= (1<<2),

		/**
		\brief Enable/disable per particle rest offsets.
		Per particle rest offsets can be used to support particles having different sizes with 
		respect to collision.
		
		\note This configuration cannot be changed after the particle system was created.

		@see PxParticleBaseDesc.restOffset
		*/
		ePER_PARTICLE_REST_OFFSET			= (1<<3),

		/**
		\brief Enable/disable particle indices debug mode.

		This can be used to help finding erroneous and unintended particle updates. Note that
		any operations on invalid particles have undefined effects. Warnings are issued in checked builds.
		However, particle indices get reused when releasing and creating new particles. Under these 
		circumstances erroneous particle updates might pass undetected. Switching on this flag and setting 
		PxParticleBaseDesc::maxParticles to a large value might help to debug bad index management.

		\note This flag can only be set in the particle system descriptor at creation time. Changing the flag later on is not supported.
		*/
		eMINIMIZE_INDEX_REUSE_DEBUG_MODE	= (1<<4),

		/**
		\brief Enable/disable GPU acceleration. 
		This configuration cannot be changed while the particle system is part of a scene.
		
		\note Switching this flag while the particle system is part of a scene will fail. 
		
		@see PxScene.removeActor() PxScene.addActor()
		*/
		eGPU								= (1<<5),
	};
};

/** @} */
#endif
