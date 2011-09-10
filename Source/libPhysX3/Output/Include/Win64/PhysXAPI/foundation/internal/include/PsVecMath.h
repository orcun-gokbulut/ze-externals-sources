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


#ifndef PX_PHYSICS_COMMON_VECMATH
#define PX_PHYSICS_COMMON_VECMATH

#include "Ps.h"
#include "PsIntrinsics.h"
#include "PxVec3.h"
#include "PxVec4.h"
#include "PxMat33.h"

//We can activate asserts in vectorised functions for testing.
//NEVER submit with asserts activated.  
//Only activate asserts for local testing.
#define AOS_ASSERTS_ON 0

//We can opt to use the scalar version of vectorised functions.
//This can catch type safety issues and might even work out more optimal on pc.
//It will also be useful for benchmarking and testing.
//NEVER submit with vector intrinsics deactivated without good reason.
//AM: deactivating SIMD for debug win64 just so autobuild will also exercise 
//non-SIMD path, until a dedicated non-SIMD platform sich as Arm comes online.
//TODO: dima: reference all platforms with SIMD support here,
//all unknown/experimental cases should better default to NO SIMD.
#if defined(PX_X86) || (defined(PX_X64) && !defined(_DEBUG)) || defined(PX_PS3) || defined(PX_X360) || (defined(PX_LINUX) && (defined(PX_X86) || defined(PX_X64)))
#define COMPILE_VECTOR_INTRINSICS 1 // use SIMD
#else
#define COMPILE_VECTOR_INTRINSICS 0 // do not use SIMD
#endif

//////////////////////////
//-----------------------
//ATTENTION!
//We're currently experimenting with a less verbose interface 
//so you'll see two interfaces to the vectorised functions.  
//The first verbose interface uses namespaces (eg FAdd(a,b) or  V3Dot(a,b)) while
//the second less verbose interface uses pre-fixes (eg FAdd(a,b) or V3Dot(a,b))
//This is a temporary confusion until we decide on the final interface. 
//Perhaps neither of these interfaces will be used in the end because neither supports operator overloading.
//Until the debate is complete be aware that everything in this file is subject to change!
/////////////////////////


#if AOS_ASSERTS_ON
#define VECMATHAOS_ASSERT PX_ASSERT 
#else
#define VECMATHAOS_ASSERT(x) {}
#endif

#if defined(PX_VC) && defined(PX_X64)
#pragma warning(disable: 4744) // http://www.popularusenetgroups.com/showthread.php?t=471583
#endif

namespace physx
{
namespace shdfnd3
{
namespace aos
{

//Basic AoS types are 
//FloatV	- 16-byte aligned representation of float.
//Vec3V		- 16-byte aligned representation of PxVec3 stored as (x y z 0).
//Vec4V		- 16-byte aligned representation of vector of 4 floats stored as (x y z w).
//BoolV		- 16-byte aligned representation of vector of 4 bools stored as (x y z w).
//Mat33V	- 16-byte aligned representation of any 3x3 matrix.
//Mat34V	- 16-byte aligned representation of transformation matrix (rotation in col1,col2,col3 and translation in col4).
//Mat44V	- 16-byte aligned representation of any 4x4 matrix.

//Cross-platform typedef declarations of 16-byte aligned types (win32/360/ppu/spu etc).
//#ifdef PX_WINDOWS
//#include "windows/CmCacheAlign.h"
//#endif
//#ifdef PX_X360
//#include "xbox360/CmCacheAlign.h"
//#endif
//#ifdef PX_PS3
//#include "ps3/CmCacheAlign.h"
//#endif
//#if defined PX_LINUX || defined PX_APPLE
//#include "linux/CmCacheAlign.h"
//#endif

#if COMPILE_VECTOR_INTRINSICS
#include "PsAoS.h"
#else // #if COMPILE_VECTOR_INTRINSICS
#include "PsVecMathAoSScalar.h"
#endif // #if !COMPILE_VECTOR_INTRINSICS


//Construct a 16-byte aligned type from a scalar type.
PX_FORCE_INLINE FloatV FloatV_From_F32(const PxF32 f);
PX_FORCE_INLINE Vec3V Vec3V_From_F32(const PxF32 f);			
PX_FORCE_INLINE Vec4V Vec4V_From_F32(const PxF32 f);			
PX_FORCE_INLINE Vec4V Vec4V_From_XYZW(PxF32 x, PxF32 y, PxF32 z, PxF32 w);
PX_FORCE_INLINE Vec3V Vec3V_From_Vec4V(Vec4V v)	;		
PX_FORCE_INLINE Vec4V Vec4V_From_Vec3V(Vec3V f);
PX_FORCE_INLINE VecU32V VecU32V_From_XYZW(PxU32 x, PxU32 y, PxU32 z, PxU32 w);
PX_FORCE_INLINE BoolV BoolV_From_Bool32(const bool f);	
PX_FORCE_INLINE Vec3V Vec3V_From_PxVec3_Aligned(const PxVec3& f);		
PX_FORCE_INLINE Vec3V Vec3V_From_PxVec3(const PxVec3& f);		
PX_FORCE_INLINE Vec4V Vec4V_From_F32Array_Aligned(const PxF32* const f);
PX_FORCE_INLINE void F32Array_Aligned_From_Vec4V(const Vec4V a, PxF32* f);
PX_FORCE_INLINE Vec4V Vec4V_From_F32Array(const PxF32* const f);	
PX_FORCE_INLINE BoolV BoolV_From_Bool32Array(const bool* const f);

//Convert back from 16-byte aligned type to scalar type.
PX_FORCE_INLINE PxF32 PxF32_From_FloatV(const FloatV a);	
PX_FORCE_INLINE void PxVec3Aligned_From_Vec3V(const Vec3V a, PxVec3& f);
PX_FORCE_INLINE void PxVec3_From_Vec3V(const Vec3V a, PxVec3& f);


//Tests that all elements of two 16-byte types are completely equivalent.
//Use these tests for unit testing and asserts only.
namespace _VecMathTests
{
	PX_FORCE_INLINE bool allElementsEqualFloatV(const FloatV a, const FloatV b);
	PX_FORCE_INLINE bool allElementsEqualVec3V(const Vec3V a, const Vec3V b);
	PX_FORCE_INLINE bool allElementsEqualVec4V(const Vec4V a, const Vec4V b);
	PX_FORCE_INLINE bool allElementsEqualBoolV(const BoolV a, const BoolV b);
	PX_FORCE_INLINE bool allElementsEqualMat33V(const Mat33V& a, const Mat33V& b)
	{
		return
		(
		allElementsEqualVec3V(a.col0,b.col0) && 
		allElementsEqualVec3V(a.col1,b.col1) && 
		allElementsEqualVec3V(a.col2,b.col2)
		);
	}
	PX_FORCE_INLINE bool allElementsEqualMat34V(const Mat34V& a, const Mat34V& b)
	{
		return
		(
		allElementsEqualVec3V(a.col0,b.col0) && 
		allElementsEqualVec3V(a.col1,b.col1) && 
		allElementsEqualVec3V(a.col2,b.col2) &&
		allElementsEqualVec3V(a.col3,b.col3) 
		);
	}
	PX_FORCE_INLINE bool allElementsEqualMat44V(const Mat44V& a, const Mat44V& b)
	{
		return
		(
		allElementsEqualVec4V(a.col0,b.col0) && 
		allElementsEqualVec4V(a.col1,b.col1) && 
		allElementsEqualVec4V(a.col2,b.col2) &&
		allElementsEqualVec4V(a.col3,b.col3) 
		);
	}

	PX_FORCE_INLINE bool allElementsNearEqualFloatV(const FloatV a, const FloatV b);
	PX_FORCE_INLINE bool allElementsNearEqualVec3V(const Vec3V a, const Vec3V b);
	PX_FORCE_INLINE bool allElementsNearEqualVec4V(const Vec4V a, const Vec4V b);
	PX_FORCE_INLINE bool allElementsNearEqualMat33V(const Mat33V& a, const Mat33V& b)
	{
		return
		(
		allElementsNearEqualVec3V(a.col0,b.col0) && 
		allElementsNearEqualVec3V(a.col1,b.col1) && 
		allElementsNearEqualVec3V(a.col2,b.col2)
		);
	}
	PX_FORCE_INLINE bool allElementsNearEqualMat34V(const Mat34V& a, const Mat34V& b)
	{
		return
		(
		allElementsNearEqualVec3V(a.col0,b.col0) && 
		allElementsNearEqualVec3V(a.col1,b.col1) && 
		allElementsNearEqualVec3V(a.col2,b.col2) &&
		allElementsNearEqualVec3V(a.col3,b.col3) 
		);
	}
	PX_FORCE_INLINE bool allElementsNearEqualMat44V(const Mat44V& a, const Mat44V& b)
	{
		return
		(
		allElementsNearEqualVec4V(a.col0,b.col0) && 
		allElementsNearEqualVec4V(a.col1,b.col1) && 
		allElementsNearEqualVec4V(a.col2,b.col2) &&
		allElementsNearEqualVec4V(a.col3,b.col3) 
		);
	}
};




//Math operations on 16-byte aligned floats.

//(0,0,0,0)
PX_FORCE_INLINE FloatV FZero();
//(1,1,1,1)
PX_FORCE_INLINE FloatV FOne();
//(PX_EPS_REAL,PX_EPS_REAL,PX_EPS_REAL,PX_EPS_REAL)
PX_FORCE_INLINE FloatV FEps();
//-f (per component)
PX_FORCE_INLINE FloatV FNeg(const FloatV f)	;
//a+b (per component)
PX_FORCE_INLINE FloatV FAdd(const FloatV a, const FloatV b);
//a-b (per component)
PX_FORCE_INLINE FloatV FSub(const FloatV a, const FloatV b)	;
//a*b (per component)
PX_FORCE_INLINE FloatV FMul(const FloatV a, const FloatV b)	;
//a/b (per component)
PX_FORCE_INLINE FloatV FDiv(const FloatV a, const FloatV b);
//a/b (per component)
PX_FORCE_INLINE FloatV FDivFast(const FloatV a, const FloatV b);
//1.0f/a
PX_FORCE_INLINE FloatV FRecip(const FloatV a);
//1.0f/a
PX_FORCE_INLINE FloatV FRecipFast(const FloatV a);
//1.0f/sqrt(a)
PX_FORCE_INLINE FloatV FRsqrt(const FloatV a);
//1.0f/sqrt(a)
PX_FORCE_INLINE FloatV FRsqrtFast(const FloatV a);
//a*b+c
PX_FORCE_INLINE FloatV FMulAdd(const FloatV a, const FloatV b, const FloatV c);
//c-a*b
PX_FORCE_INLINE FloatV FNegMulSub(const FloatV a, const FloatV b, const FloatV c);
//fabs(a)
PX_FORCE_INLINE FloatV FAbs(const FloatV a);
//c ? a : b (per component)
PX_FORCE_INLINE FloatV FSel(const BoolV c, const FloatV a, const FloatV b);
//a>b (per component)
PX_FORCE_INLINE BoolV FIsGrtr(const FloatV a, const FloatV b);
//a>=b (per component)
PX_FORCE_INLINE BoolV FIsGrtrOrEq(const FloatV a, const FloatV b);
//a==b (per component)
PX_FORCE_INLINE BoolV FIsEq(const FloatV a, const FloatV b);
//Max(a,b) (per component)
PX_FORCE_INLINE FloatV FMax(const FloatV a, const FloatV b);
//Min(a,b) (per component)
PX_FORCE_INLINE FloatV FMin(const FloatV a, const FloatV b);
//Clamp(a,b) (per component)
PX_FORCE_INLINE FloatV FClamp(const FloatV a, const FloatV minV, const FloatV maxV);

//Test all components.
PX_FORCE_INLINE PxU32 FAllGrtr(const FloatV a, const FloatV b);
PX_FORCE_INLINE PxU32 FAllGrtrOrEq(const FloatV a, const FloatV b);
PX_FORCE_INLINE PxU32 FAllEq(const FloatV a, const FloatV b);

//Math operations on 16-byte aligned vector3s.

//(f,f,f,f)
PX_FORCE_INLINE Vec3V V3Splat(const FloatV f); 

//(x,y,z)
PX_FORCE_INLINE Vec3V V3Merge(const FloatV x, const FloatV y, const FloatV z);

//(1,0,0,0)
PX_FORCE_INLINE Vec3V V3UnitX();
//(0,1,0,0)
PX_FORCE_INLINE Vec3V V3UnitY();
//(0,0,1,0)
PX_FORCE_INLINE Vec3V V3UnitZ();

//(f.x,f.x,f.x,f.x)
PX_FORCE_INLINE FloatV V3GetX(const Vec3V f); 
//(f.y,f.y,f.y,f.y)
PX_FORCE_INLINE FloatV V3GetY(const Vec3V f); 
//(f.z,f.z,f.z,f.z)
PX_FORCE_INLINE FloatV V3GetZ(const Vec3V f);

//(f,v.y,v.z,v.w)
PX_FORCE_INLINE Vec3V V3SetX(const Vec3V v, const FloatV f); 
//(v.x,f,v.z,v.w)
PX_FORCE_INLINE Vec3V V3SetY(const Vec3V v, const FloatV f); 
//(v.x,v.y,f,v.w)
PX_FORCE_INLINE Vec3V V3SetZ(const Vec3V v, const FloatV f); 


PX_FORCE_INLINE void V3WriteX(Vec3V& v, const PxF32 f);
PX_FORCE_INLINE void V3WriteY(Vec3V& v, const PxF32 f);
PX_FORCE_INLINE void V3WriteZ(Vec3V& v, const PxF32 f);
PX_FORCE_INLINE void V3WriteW(Vec3V& v, const PxF32 f);
PX_FORCE_INLINE void V3WriteXYZ(Vec3V& v, const PxVec3& f);
PX_FORCE_INLINE PxF32 V3ReadX(const Vec3V& v);
PX_FORCE_INLINE PxF32 V3ReadY(const Vec3V& v);
PX_FORCE_INLINE PxF32 V3ReadZ(const Vec3V& v);
PX_FORCE_INLINE PxF32 V3ReadW(const Vec3V& v);
PX_FORCE_INLINE const PxVec3& V3ReadXYZ(const Vec3V& v);


//(0,0,0,0)
PX_FORCE_INLINE Vec3V V3Zero();
//(1,1,1,1)
PX_FORCE_INLINE Vec3V V3One();
//(PX_EPS_REAL,PX_EPS_REAL,PX_EPS_REAL,PX_EPS_REAL)
PX_FORCE_INLINE Vec3V V3Eps();
//-c (per component)
PX_FORCE_INLINE Vec3V V3Neg(const Vec3V c);						
//a+b (per component)
PX_FORCE_INLINE Vec3V V3Add(const Vec3V a, const Vec3V b);			
//a-b (per component)
PX_FORCE_INLINE Vec3V V3Sub(const Vec3V a, const Vec3V b);	
//a*b (per component)
PX_FORCE_INLINE Vec3V V3Scale(const Vec3V a, const FloatV b);	
//a*b (per component)
PX_FORCE_INLINE Vec3V V3Mul(const Vec3V a, const Vec3V b);	
//a/b (per component)
PX_FORCE_INLINE Vec3V V3ScaleInv(const Vec3V a, const FloatV b);		
//a/b (per component)
PX_FORCE_INLINE Vec3V V3Div(const Vec3V a, const Vec3V b);		
//a/b (per component)
PX_FORCE_INLINE Vec3V V3ScaleInvFast(const Vec3V a, const FloatV b);		
//a/b (per component)
PX_FORCE_INLINE Vec3V V3DivFast(const Vec3V a, const Vec3V b);		
//1.0f/a
PX_FORCE_INLINE Vec3V V3Recip(const Vec3V a);
//1.0f/a
PX_FORCE_INLINE Vec3V V3RecipFast(const Vec3V a);
//1.0f/sqrt(a)
PX_FORCE_INLINE Vec3V V3Rsqrt(const Vec3V a);
//1.0f/sqrt(a)
PX_FORCE_INLINE Vec3V V3RsqrtFast(const Vec3V a);
//a*b+c
PX_FORCE_INLINE Vec3V V3MulAdd(const Vec3V a, const Vec3V b, const Vec3V c);
//c-a*b
PX_FORCE_INLINE Vec3V V3NegMulSub(const Vec3V a, const Vec3V b, const Vec3V c);

//fabs(a)
PX_FORCE_INLINE Vec3V V3Abs(const Vec3V a);

//a.b
PX_FORCE_INLINE FloatV V3Dot(const Vec3V a, const Vec3V b);	
//aXb
PX_FORCE_INLINE Vec3V V3Cross(const Vec3V a, const Vec3V b);	
//|a.a|^1/2
PX_FORCE_INLINE FloatV V3Length(const Vec3V a);
//a.a
PX_FORCE_INLINE FloatV V3LengthSq(const Vec3V a);
//a*|a.a|^-1/2
PX_FORCE_INLINE Vec3V V3Normalise(const Vec3V a);
//a.a>0 ? a*|a.a|^-1/2 : (0,0,0,0)
PX_FORCE_INLINE FloatV V3Length(const Vec3V a);
//a*|a.a|^-1/2
PX_FORCE_INLINE Vec3V V3NormaliseSafe(const Vec3V a);

//c ? a : b (per component)
PX_FORCE_INLINE Vec3V V3Sel(const BoolV c, const Vec3V a, const Vec3V b);
//a>b (per component)
PX_FORCE_INLINE BoolV V3IsGrtr(const Vec3V a, const Vec3V b);			
//a>=b (per component)
PX_FORCE_INLINE BoolV V3IsGrtrOrEq(const Vec3V a, const Vec3V b);	
//a==b (per component)
PX_FORCE_INLINE BoolV V3IsEq(const Vec3V a, const Vec3V b);
//Max(a,b) (per component)
PX_FORCE_INLINE Vec3V V3Max(const Vec3V a, const Vec3V b);					
//Min(a,b) (per component)
PX_FORCE_INLINE Vec3V V3Min(const Vec3V a, const Vec3V b);	
//Clamp(a,b) (per component)
PX_FORCE_INLINE Vec3V V3Clamp(const Vec3V a, const Vec3V minV, const Vec3V maxV);

//Test all components.
PX_FORCE_INLINE PxU32 V3AllGrtr(const Vec3V a, const Vec3V b);
PX_FORCE_INLINE PxU32 V3AllGrtrOrEq(const Vec3V a, const Vec3V b);
PX_FORCE_INLINE PxU32 V3AllEq(const Vec3V a, const Vec3V b);


//Math operations on 16-byte aligned vector4s.
//(f,f,f,f)
PX_FORCE_INLINE Vec4V V4Splat(const FloatV f);

//(floatVArray[0],floatVArray[1],floatVArray[2],floatVArray[3])
PX_FORCE_INLINE Vec4V V4Merge(const FloatV* const floatVArray);

//(1,0,0,0)
PX_FORCE_INLINE Vec4V V4UnitW();
//(0,1,0,0)
PX_FORCE_INLINE Vec4V V4UnitY();
//(0,0,1,0)
PX_FORCE_INLINE Vec4V V4UnitZ();
//(0,0,0,1)
PX_FORCE_INLINE Vec4V V4UnitW();

//(f.x,f.x,f.x,f.x)
PX_FORCE_INLINE FloatV V4GetW(const Vec4V f); 
//(f.y,f.y,f.y,f.y)
PX_FORCE_INLINE FloatV V4GetX(const Vec4V f); 
//(f.z,f.z,f.z,f.z)
PX_FORCE_INLINE FloatV V4GetZ(const Vec4V f); 
//(f.w,f.w,f.w,f.w)
PX_FORCE_INLINE FloatV V4GetW(const Vec4V f); 

//(f,v.y,v.z,v.w)
PX_FORCE_INLINE Vec4V V4SetX(const Vec4V v, const FloatV f); 
//(v.x,f,v.z,v.w)
PX_FORCE_INLINE Vec4V V4SetY(const Vec4V v, const FloatV f); 
//(v.x,v.y,f,v.w)
PX_FORCE_INLINE Vec4V V4SetZ(const Vec4V v, const FloatV f); 
//(v.x,v.y,v.z,f)
PX_FORCE_INLINE Vec4V V4SetW(const Vec4V v, const FloatV f); 

PX_FORCE_INLINE void V4WriteX(Vec4V& v, const PxF32 f);
PX_FORCE_INLINE void V4WriteY(Vec4V& v, const PxF32 f);
PX_FORCE_INLINE void V4WriteZ(Vec4V& v, const PxF32 f);
PX_FORCE_INLINE void V4WriteW(Vec4V& v, const PxF32 f);
PX_FORCE_INLINE void V4WriteXYZ(Vec4V& v, const PxVec3& f);
PX_FORCE_INLINE PxF32 V4ReadX(const Vec4V& v);
PX_FORCE_INLINE PxF32 V4ReadY(const Vec4V& v);
PX_FORCE_INLINE PxF32 V4ReadZ(const Vec4V& v);
PX_FORCE_INLINE PxF32 V4ReadW(const Vec4V& v);
PX_FORCE_INLINE const PxVec3& V4ReadXYZ(const Vec4V& v);

//(0,0,0,0)
PX_FORCE_INLINE Vec4V V4Zero();
//(1,1,1,1)
PX_FORCE_INLINE Vec4V V4One();
//(PX_EPS_REAL,PX_EPS_REAL,PX_EPS_REAL,PX_EPS_REAL)
PX_FORCE_INLINE Vec4V V4Eps();

//-c (per component)
PX_FORCE_INLINE Vec4V V4Neg(const Vec4V c);						
//a+b (per component)
PX_FORCE_INLINE Vec4V V4Add(const Vec4V a, const Vec4V b);	
//a-b (per component)
PX_FORCE_INLINE Vec4V V4Sub(const Vec4V a, const Vec4V b);	
//a*b (per component)
PX_FORCE_INLINE Vec4V V4Scale(const Vec4V a, const FloatV b);
//a*b (per component)
PX_FORCE_INLINE Vec4V V4Mul(const Vec4V a, const Vec4V b);	
//a/b (per component)
PX_FORCE_INLINE Vec4V V4ScaleInv(const Vec4V a, const FloatV b);	
//a/b (per component)
PX_FORCE_INLINE Vec4V V4Div(const Vec4V a, const Vec4V b);	
//a/b (per component)
PX_FORCE_INLINE Vec4V V4ScaleInvFast(const Vec4V a, const FloatV b);		
//a/b (per component)
PX_FORCE_INLINE Vec4V V4DivFast(const Vec4V a, const Vec4V b);		
//1.0f/a
PX_FORCE_INLINE Vec4V V4Recip(const Vec4V a);
//1.0f/a
PX_FORCE_INLINE Vec4V V4RecipFast(const Vec4V a);
//1.0f/sqrt(a)
PX_FORCE_INLINE Vec4V V4Rsqrt(const Vec4V a);
//1.0f/sqrt(a)
PX_FORCE_INLINE Vec4V V4RsqrtFast(const Vec4V a);
//a*b+c
PX_FORCE_INLINE Vec4V V4MulAdd(const Vec4V a, const Vec4V b, const Vec4V c);
//c-a*b
PX_FORCE_INLINE Vec4V V4NegMulSub(const Vec4V a, const Vec4V b, const Vec4V c);

//fabs(a)
PX_FORCE_INLINE Vec4V V4Abs(const Vec4V a);
//bitwise a & ~b
PX_FORCE_INLINE Vec4V V4Andc(const Vec4V a, const VecU32V b);

//a.b
PX_FORCE_INLINE FloatV V4Dot(const Vec4V a, const Vec4V b);		

//|a.a|^1/2
PX_FORCE_INLINE FloatV V4Length(const Vec4V a);
//a.a
PX_FORCE_INLINE FloatV V4LengthSq(const Vec4V a);

//a*|a.a|^-1/2
PX_FORCE_INLINE Vec4V V4Normalise(const Vec4V a);
//a.a>0 ? a*|a.a|^-1/2 : (0,0,0,0)
PX_FORCE_INLINE Vec4V V4NormaliseSafe(const Vec4V a);
//a*|a.a|^-1/2
PX_FORCE_INLINE Vec4V V4NormaliseFast(const Vec4V a);

//c ? a : b (per component)
PX_FORCE_INLINE Vec4V V4Sel(const BoolV c, const Vec4V a, const Vec4V b);	  
//a>b (per component)
PX_FORCE_INLINE BoolV V4IsGrtr(const Vec4V a, const Vec4V b);			
PX_FORCE_INLINE VecU32V V4IsGrtrV32u(const Vec4V a, const Vec4V b);			
//a>=b (per component)
PX_FORCE_INLINE BoolV V4IsGrtrOrEq(const Vec4V a, const Vec4V b);	
//a==b (per component)
PX_FORCE_INLINE BoolV V4IsEq(const Vec4V a, const Vec4V b);
//Max(a,b) (per component)
PX_FORCE_INLINE Vec4V V4Max(const Vec4V a, const Vec4V b);					
//Min(a,b) (per component)
PX_FORCE_INLINE Vec4V V4Min(const Vec4V a, const Vec4V b);

//Clamp(a,b) (per component)
PX_FORCE_INLINE Vec4V V4Clamp(const Vec4V a, const Vec4V minV, const Vec4V maxV);

//Test all components.
PX_FORCE_INLINE PxU32 V4AllGrtr(const Vec4V a, const Vec4V b);
PX_FORCE_INLINE PxU32 V4AllGrtrOrEq(const Vec4V a, const Vec4V b);
PX_FORCE_INLINE PxU32 V4AllEq(const Vec4V a, const Vec4V b);

PX_FORCE_INLINE Vec4V V4LoadAligned(Vec4V* addr);
PX_FORCE_INLINE Vec4V V4LoadUnaligned(Vec4V* addr);

PX_FORCE_INLINE Vec4V V4Floor(Vec4V a);
PX_FORCE_INLINE Vec4V V4Ceil(Vec4V a);
PX_FORCE_INLINE VecU32V V4ConvertToU32VSaturate(const Vec4V a, PxU32 power);
template<int elementIndex>
PX_FORCE_INLINE Vec4V V4SplatElement(Vec4V a);

//Math operations on 16-byte aligned booleans.
//x=false	y=false		z=false		w=false	
PX_FORCE_INLINE BoolV BFFFF();
//x=false	y=false		z=false		w=true
PX_FORCE_INLINE BoolV BFFFT();
//x=false	y=false		z=true		w=false
PX_FORCE_INLINE BoolV BFFTF();
//x=false	y=false		z=true		w=true
PX_FORCE_INLINE BoolV BFFTT();
//x=false	y=true		z=false		w=false
PX_FORCE_INLINE BoolV BFTFF();
//x=false	y=true		z=false		w=true
PX_FORCE_INLINE BoolV BFTFT();
//x=false	y=true		z=true		w=false
PX_FORCE_INLINE BoolV BFTTF();
//x=false	y=true		z=true		w=true
PX_FORCE_INLINE BoolV BFTTT();
//x=true	y=false		z=false		w=false
PX_FORCE_INLINE BoolV BTFFF();
//x=true	y=false		z=false		w=true
PX_FORCE_INLINE BoolV BTFFT();
//x=true	y=false		z=true		w=false
PX_FORCE_INLINE BoolV BTFTF();
//x=true	y=false		z=true		w=true
PX_FORCE_INLINE BoolV BTFTT();
//x=true	y=true		z=false		w=false
PX_FORCE_INLINE BoolV BTTFF(); 
//x=true	y=true		z=false		w=true
PX_FORCE_INLINE BoolV BTTFT(); 
//x=true	y=true		z=true		w=false
PX_FORCE_INLINE BoolV BTTTF(); 
//x=true	y=true		z=true		w=true
PX_FORCE_INLINE BoolV BTTTT(); 

//x=false	y=false		z=false		w=true
PX_FORCE_INLINE BoolV BWMask();
//x=true	y=false		z=false		w=false
PX_FORCE_INLINE BoolV BXMask();
//x=false	y=true		z=false		w=false
PX_FORCE_INLINE BoolV BYMask();
//x=false	y=false		z=true		w=false
PX_FORCE_INLINE BoolV BZMask();

//component-wise && (AND)
BoolV BAnd(const BoolV a, const BoolV b);
//component wise || (OR)
BoolV BOr(const BoolV a, const BoolV b);

//Return 1 if all components equal, zero otherwise.
PX_FORCE_INLINE PxU32 BAllEq(const BoolV a, const BoolV b);


//Math operations on 16-byte aligned Mat33s (represents any 3x3 matrix)
//a*b
PX_FORCE_INLINE Vec3V M33MulV3(const Mat33V& a, const Vec3V b);
//transpose(a) * b
PX_FORCE_INLINE Vec3V M33TrnspsMulV3(const Mat33V& a, const Vec3V b);
//a*b
PX_FORCE_INLINE Mat33V M33MulM33(const Mat33V& a, const Mat33V& b);
//a+b
PX_FORCE_INLINE Mat33V M33Add(const Mat33V& a, const Mat33V& b);
//a+b
PX_FORCE_INLINE Mat33V M33Sub(const Mat33V& a, const Mat33V& b);
//-a
PX_FORCE_INLINE Mat33V M33Neg(const Mat33V& a, const Mat33V& b);
//transpose(a)
PX_FORCE_INLINE Mat33V M33Trnsps(const Mat33V& a);


//Math operations on 16-byte aligned Mat34s (represents transformation matrix - rotation and translation).
//namespace _Mat34V
//{
//	//a*b
//	PX_FORCE_INLINE Vec3V multiplyV(const Mat34V& a, const Vec3V b);
//	//a_rotation * b
//	PX_FORCE_INLINE Vec3V multiply3X3V(const Mat34V& a, const Vec3V b);
//	//transpose(a_rotation)*b
//	PX_FORCE_INLINE Vec3V multiplyTranspose3X3V(const Mat34V& a, const Vec3V b);
//	//a*b 
//	PX_FORCE_INLINE Mat34V multiplyV(const Mat34V& a, const Mat34V& b);
//	//a_rotation*b
//	PX_FORCE_INLINE Mat33V multiply3X3V(const Mat34V& a, const Mat33V& b);
//	//a_rotation*b_rotation
//	PX_FORCE_INLINE Mat33V multiply3X3V(const Mat34V& a, const Mat34V& b);
//	//a+b
//	PX_FORCE_INLINE Mat34V addV(const Mat34V& a, const Mat34V& b);
//	//a^-1
//	PX_FORCE_INLINE Mat34V getInverseV(const Mat34V& a);
//	//transpose(a_rotation)
//	PX_FORCE_INLINE Mat33V getTranspose3X3(const Mat34V& a);
//}; //namespace _Mat34V

//a*b
//#define M34MulV3(a,b)			(M34MulV3(a,b))
////a_rotation * b
//#define M34Mul33V3(a,b)			(M34Mul33V3(a,b))
////transpose(a_rotation)*b
//#define M34TrnspsMul33V3(a,b)	(M34TrnspsMul33V3(a,b))
////a*b 
//#define M34MulM34(a,b)			(_Mat34V::multiplyV(a,b))
//a_rotation*b
//#define M34MulM33(a,b)			(M34MulM33(a,b))
//a_rotation*b_rotation
//#define M34Mul33MM34(a,b)		(M34MulM33(a,b))
//a+b
//#define M34Add(a,b)				(M34Add(a,b))
////a^-1
//#define M34Inverse(a,b)			(M34Inverse(a))
//transpose(a_rotation)
//#define M34Trnsps33(a)			(M33Trnsps3X3(a))


//Math operations on 16-byte aligned Mat44s (represents any 4x4 matrix) 
//namespace _Mat44V
//{
//	//a*b
//	PX_FORCE_INLINE Vec4V multiplyV(const Mat44V& a, const Vec4V b);
//	//transpose(a)*b
//	PX_FORCE_INLINE Vec4V multiplyTransposeV(const Mat44V& a, const Vec4V b);
//	//a*b
//	PX_FORCE_INLINE Mat44V multiplyV(const Mat44V& a, const Mat44V& b);
//	//a+b
//	PX_FORCE_INLINE Mat44V addV(const Mat44V& a, const Mat44V& b);
//	//a&-1
//	PX_FORCE_INLINE Mat44V getInverseV(const Mat44V& a);
//	//transpose(a)
//	PX_FORCE_INLINE Mat44V getTransposeV(const Mat44V& a);
//}; //namespace _Mat44V

//namespace _VecU32V
//{
//	// pack 8 U32s to 8 U16s with saturation
//	PX_FORCE_INLINE VecU16V pack2U32VToU16VSaturate(VecU32V a, VecU32V b);
//	PX_FORCE_INLINE VecU32V orV(VecU32V a, VecU32V b);
//	PX_FORCE_INLINE VecU32V andV(VecU32V a, VecU32V b);
//	PX_FORCE_INLINE VecU32V andcV(VecU32V a, VecU32V b);
//	// conversion from integer to float
//	PX_FORCE_INLINE Vec4V convertToVec4V(VecU32V a);
//	// splat a[elementIndex] into all fields of a
//	template<int elementIndex>
//	PX_FORCE_INLINE VecU32V splatElement(VecU32V a);
//	PX_FORCE_INLINE void storeAligned(VecU32V a, VecU32V* address);
//};

//namespace _VecI32V
//{
//	template<int a> PX_FORCE_INLINE VecI32V splatI32();
//};
//
//namespace _VecU16V
//{
//	PX_FORCE_INLINE VecU16V orV(VecU16V a, VecU16V b);
//	PX_FORCE_INLINE VecU16V andV(VecU16V a, VecU16V b);
//	PX_FORCE_INLINE VecU16V andcV(VecU16V a, VecU16V b);
//	PX_FORCE_INLINE void storeAligned(VecU16V val, VecU16V *address);
//	PX_FORCE_INLINE VecU16V loadAligned(VecU16V* addr);
//	PX_FORCE_INLINE VecU16V loadUnaligned(VecU16V* addr);
//	PX_FORCE_INLINE VecU16V compareGt(VecU16V a, VecU16V b);
//	template<int elementIndex>
//	PX_FORCE_INLINE VecU16V splatElement(VecU16V a);
//	PX_FORCE_INLINE VecU16V subtractModulo(VecU16V a, VecU16V b);
//	PX_FORCE_INLINE VecU16V addModulo(VecU16V a, VecU16V b);
//	PX_FORCE_INLINE VecU32V getLo16(VecU16V a); // [0,2,4,6] 16-bit values to [0,1,2,3] 32-bit vector
//	PX_FORCE_INLINE VecU32V getHi16(VecU16V a); // [1,3,5,7] 16-bit values to [0,1,2,3] 32-bit vector
//};
//
//namespace _VecI16V
//{
//	template <int val> PX_FORCE_INLINE VecI16V splatImmediate();
//};
//
//namespace _VecU8V
//{
//};


//a*b
//#define M44MulV4(a,b)		(M44MulV4(a,b))
////transpose(a)*b
//#define M44TrnspsMulV4(a,b) (M44TrnspsMulV4(a,b))
////a*b
//#define M44MulM44(a,b)		(M44MulM44(a,b))
////a+b
//#define M44Add(a,b)			(M44Add(a,b))
////a&-1
//#define M44Inverse(a)		(M44Inverse(a))
////transpose(a)
//#define M44Trnsps(a)		(M44Trnsps(a))


// dsequeira: these used to be assert'd out in SIMD builds, but they're necessary if
// we want to be able to write some scalar functions which run using SIMD data structures

PX_FORCE_INLINE void V3WriteX(Vec3V& v, const PxF32 f)
{
	((PxVec3 &)v).x=f;
}

PX_FORCE_INLINE void V3WriteY(Vec3V& v, const PxF32 f)
{
	((PxVec3 &)v).y=f;
}

PX_FORCE_INLINE void V3WriteZ(Vec3V& v, const PxF32 f)
{
	((PxVec3 &)v).z=f;
}

PX_FORCE_INLINE void V3WriteXYZ(Vec3V& v, const PxVec3& f)
{
	(PxVec3 &)v = f;
}

PX_FORCE_INLINE PxF32 V3ReadX(const Vec3V& v)
{
	return ((PxVec3 &)v).x;
}

PX_FORCE_INLINE PxF32 V3ReadY(const Vec3V& v)
{
	return ((PxVec3 &)v).y;
}

PX_FORCE_INLINE PxF32 V3ReadZ(const Vec3V& v)
{
	return ((PxVec3 &)v).z;
}

PX_FORCE_INLINE const PxVec3& V3ReadXYZ(const Vec3V& v)
{
	return (PxVec3&)v;
}

PX_FORCE_INLINE void V4WriteX(Vec4V& v, const PxF32 f)
{
	((PxVec4&)v).x=f;
}

PX_FORCE_INLINE void V4WriteY(Vec4V& v, const PxF32 f)
{
	((PxVec4&)v).y=f;
}

PX_FORCE_INLINE void V4WriteZ(Vec4V& v, const PxF32 f)
{
	((PxVec4&)v).z=f;
}

PX_FORCE_INLINE void V4WriteW(Vec4V& v, const PxF32 f)
{
	((PxVec4&)v).w=f;
}

PX_FORCE_INLINE void V4WriteXYZ(Vec4V& v, const PxVec3& f)
{
	((PxVec3&)v)=f;
}

PX_FORCE_INLINE PxF32 V4ReadX(const Vec4V& v)
{
	return ((PxVec4&)v).x;
}

PX_FORCE_INLINE PxF32 V4ReadY(const Vec4V& v)
{
	return ((PxVec4&)v).y;
}

PX_FORCE_INLINE PxF32 V4ReadZ(const Vec4V& v)
{
	return ((PxVec4&)v).z;
}

PX_FORCE_INLINE PxF32 V4ReadW(const Vec4V& v)
{
	return ((PxVec4&)v).w;
}

PX_FORCE_INLINE const PxVec3& V4ReadXYZ(const Vec4V& v)
{
	return (PxVec3&)v;
}


//Now for the cross-platform implementations of the 16-byte aligned maths functions (win32/360/ppu/spu etc).
#if COMPILE_VECTOR_INTRINSICS
#include "PsInlineAoS.h"
#else // #if COMPILE_VECTOR_INTRINSICS
#include "PsVecMathAoSScalarInline.h"
#endif  // #if !COMPILE_VECTOR_INTRINSICS

} // namespace aos
} // namespace shdfnd3
} // namespace physx

#endif //PS_VECMATH_H
