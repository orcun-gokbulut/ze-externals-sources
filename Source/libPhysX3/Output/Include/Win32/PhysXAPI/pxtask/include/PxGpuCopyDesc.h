/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#ifndef PX_GPU_COPY_DESC_H
#define PX_GPU_COPY_DESC_H

/*!
\file
\brief Descriptor for a CUDA Memory copy
*/

namespace physx
{
namespace pxtask
{

/**
 * \brief Input descriptor for the GpuDispatcher's built-in copy kernel
 *
 * All host memory involved in copy transactions must be page-locked.
 * If more than one descriptor is passed to the copy kernel in one launch,
 * the descriptors themselves must be in page-locked memory.
 */
struct GpuCopyDesc
{
	enum CopyType {
		HostToDevice,
		DeviceToHost,
		DeviceToDevice,
		DeviceMemset32,
	};

	size_t		dest;
	size_t		source; // 32bit value when type == DeviceMemset
	size_t		bytes;
	CopyType	type;

	/** Copy is optimally performed as 64bit words, requires 64bit alignment.  But it can
	 * gracefully degrade to 32bit copies if necessary
	 */
	PX_INLINE bool isValid()
	{
		bool ok = true;
		ok &= ( (dest & 0x3) == 0 );
		ok &= ( (type == DeviceMemset32) || (source & 0x3) == 0 );
		ok &= ( (bytes & 0x3) == 0 );
		return ok;
	}
};


} // end pxtask namespace
} // end physx namespace

#endif
