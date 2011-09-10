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


#ifndef PX_PHYSICS_NX_PARTICLEFLUID
#define PX_PHYSICS_NX_PARTICLEFLUID
/** \addtogroup particles
  @{
*/

#include "particles/PxParticleFluidDesc.h"
#include "particles/PxParticleBase.h"

/**
\brief The particle fluid class represents the main module for particle based fluid simulation.
SPH (Smoothed Particle Hydrodynamics) is used to animate the particles.  This class inherits the properties 
of the PxParticleBase class and adds particle-particle interactions. 

There are two kinds of particle interaction forces which govern the behaviour of the fluid:
<ol>
<li>
	Pressure forces: These forces result from particle densities higher than the
	"rest density" of the fluid.  The rest density is given by specifying the inter-particle
	distance at which the fluid is in its relaxed state.  Particles which are closer than
	the rest spacing are pushed away from each other.
<li>
	Viscosity forces:  These forces act on neighboring particles depending on the difference
	of their velocities.  Particles drag other particles with them which is used to simulate the
	viscous behaviour of the fluid.
</ol>

For a good introduction to SPH fluid simulation,
see http://graphics.ethz.ch/~mattmuel/publications/sca03.pdf

@see PxParticleFluidDesc, PxParticleBaseDesc, PxParticleBase, PxParticleFluidReadData
*/
class PxParticleFluid : public PxParticleBase
	{
	protected:
// PX_SERIALIZATION
								PxParticleFluid(PxRefResolver& v) : PxParticleBase(v)		{}
//~PX_SERIALIZATION
	PX_INLINE					PxParticleFluid() : PxParticleBase() {}
	virtual						~PxParticleFluid() {}

	public:

/************************************************************************************************/

/** @name Particle Access and Manipulation
*/
//@{
	
	/**
	\brief Locks the particle data and provides the data descriptor for accessing the particles including fluid particle densities.
	@see PxParticleFluidReadData
	@see PxParticleBase::lockParticleReadData()
	*/
	virtual		PxParticleFluidReadData*		lockParticleFluidReadData()										= 0;

//@}
/************************************************************************************************/


/** @name Particle Fluid Parameters
*/
//@{

	/**
	\brief Returns the fluid stiffness.

	\return The fluid stiffness.

	@see PxParticleFluidDesc.stiffness
	*/
	virtual		PxReal							getStiffness()											const	= 0;

	/**
	\brief Sets the fluid stiffness (must be positive).

	\param stiffness The new fluid stiffness.

	@see PxParticleFluidDesc.stiffness
	*/
	virtual		void 							setStiffness(PxReal stiffness)									= 0;

	/**
	\brief Returns the fluid viscosity.

	\return The viscosity  of the fluid.

	@see PxParticleFluidDesc.viscosity
	*/
	virtual		PxReal							getViscosity()											const	= 0;

	/**
	\brief Sets the fluid viscosity (must be positive).

	\param viscosity The new viscosity of the fluid.

	@see PxParticleFluidDesc.viscosity
	*/
	virtual		void 							setViscosity(PxReal viscosity)									= 0;

//@}
/************************************************************************************************/

/** @name Particle Fluid Property Read Back
*/
//@{

	/**
	\brief Returns the typical distance of particles in the relaxed state of the fluid.

	\return Rest particle distance.

	@see PxParticleFluidDesc.restParticleDistance
	*/
	virtual		PxReal							getRestParticleDistance()								const	= 0;

//@}
/************************************************************************************************/

/** @name Particle Fluid Descriptor Operations
*/
//@{

	/**
	\brief Saves the particle system descriptor.

	\param[out] desc The descriptor used to retrieve the state of the object.

	@see PxParticleFluidDesc
	*/
	virtual		void							saveToDesc(PxParticleFluidDesc& desc)					const	= 0;

//@}
/************************************************************************************************/

	};
/** @} */
#endif
