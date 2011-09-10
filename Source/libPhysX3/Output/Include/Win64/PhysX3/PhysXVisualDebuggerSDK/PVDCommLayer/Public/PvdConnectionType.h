// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
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
// Copyright (c) 2009 NVIDIA Corporation. All rights reserved.
#ifndef PVD_PVDCONNECTIONTYPE_H
#define PVD_PVDCONNECTIONTYPE_H
#include "PxFlags.h"
namespace PVD 
{
	using namespace physx::pubfnd;

	struct PvdConnectionType
	{
		enum Enum
		{
			Unknown = 0,
			Debug = 1,
			Profile = 1 << 1,
			Memory = 1 << 2,
		};
	};

	typedef PxFlags<PvdConnectionType::Enum,PxU32> TConnectionFlagsType; 
	PX_FLAGS_OPERATORS(PvdConnectionType::Enum, PxU32 );
	
	static inline TConnectionFlagsType defaultConnectionFlags() { return TConnectionFlagsType( PvdConnectionType::Debug | PvdConnectionType::Profile | PvdConnectionType::Memory ); }
}
#endif