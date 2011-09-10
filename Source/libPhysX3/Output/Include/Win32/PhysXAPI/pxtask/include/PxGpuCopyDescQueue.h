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

#ifndef PX_GPU_COPY_DESC_QUEUE_H
#define PX_GPU_COPY_DESC_QUEUE_H

/*!
\file
\brief Container for enqueing copy descriptors in pinned memory
*/

#include "PxAssert.h"
#include "PxGpuCopyDesc.h"
#include "PxGpuDispatcher.h"
#include "PxCudaContextManager.h"

namespace physx
{
namespace pxtask
{

/// \brief Container class for queueing GpuCopyDesc instances in pinned (non-pageable) CPU memory
class GpuCopyDescQueue
{
public:
	GpuCopyDescQueue( GpuDispatcher& d )
		: mDispatcher( d )
		, mStream(0)
		, mBuffer(0)
		, mReserved(0)
		, mOccupancy(0)
		, mFlushed(0)
	{
	}

	~GpuCopyDescQueue()
	{
		if( mBuffer )
		{
			mDispatcher.getCudaContextManager()->getMemoryManager()->free( CudaBufferMemorySpace::T_PINNED_HOST, (size_t) mBuffer );
		}
	}

	/// \brief Reset the enqueued copy descriptor list
	///
	/// Must be called at least once before any copies are enqueued, and each time the launched
	/// copies are known to have been completed.  The recommended use case is to call this at the
	/// start of each simulation step.
	void reset( CUstream stream, PxU32 reserveSize )
	{
		if( reserveSize > mReserved )
		{
			if( mBuffer )
			{
				mDispatcher.getCudaContextManager()->getMemoryManager()->free(
					CudaBufferMemorySpace::T_PINNED_HOST,
					(size_t) mBuffer );
				mReserved = 0;
			}
			mBuffer = (GpuCopyDesc*) mDispatcher.getCudaContextManager()->getMemoryManager()->alloc(
				CudaBufferMemorySpace::T_PINNED_HOST,
				reserveSize * sizeof(GpuCopyDesc),
				NV_ALLOC_INFO("PxGpuCopyDescQueue", GPU_UTIL));
			if( mBuffer )
			{
				mReserved = reserveSize;
			}
		}

		mOccupancy = 0;
		mFlushed = 0;
		mStream = stream;
	}

	/// \brief Enqueue the specified copy descriptor, or launch immediately if no room is available
	void enqueue( GpuCopyDesc& desc )
	{
		PX_ASSERT( desc.isValid() );
		if( desc.bytes == 0 )
			return;

		if( mOccupancy < mReserved )
		{
			mBuffer[ mOccupancy++ ] = desc;
		}
		else
		{
			mDispatcher.launchCopyKernel( &desc, 1, mStream );
		}
	}

	/// \brief Launch all copies queued since the last flush or reset
	void flushEnqueued()
	{
		if( mOccupancy > mFlushed )
		{
			mDispatcher.launchCopyKernel( mBuffer + mFlushed, mOccupancy - mFlushed, mStream );
			mFlushed = mOccupancy;
		}
	}

private:
	GpuDispatcher&	mDispatcher;
	GpuCopyDesc*	mBuffer;
	CUstream        mStream;
	PxU32			mReserved;
	PxU32			mOccupancy;
	PxU32			mFlushed;
};

} // end pxtask namespace
} // end physx namespace

#endif
