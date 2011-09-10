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

#ifndef PX_TASK_H
#define PX_TASK_H

#include "Px.h"
#include "PxTaskManager.h"
#include "PxAssert.h"

namespace physx
{
// pull public foundation into physx namespace
using namespace pubfnd;

namespace pxtask
{

/**
 * \brief Base class of all task types
 *
 * BaseTask defines a runnable reference counted task with built-in profiling.
 */
class BaseTask
{
public:
	BaseTask();
	virtual ~BaseTask();

    /**
     * \brief user implemented abstract method
     *
     * run() methods must be thread safe, stack friendly (no alloca, etc), and
     * must never block.
     */
    virtual void        run() = 0;

    /**
     * \brief user implemented abstract method
     *
     * The returned name is used for profiling purposes.  It does not have to be
     * unique, but unique names are helpful.
     */
    virtual const char *getName() const = 0;

    //! \brief implemented by derived implimementation classes
    virtual void		addReference() = 0;
    //! \brief implemented by derived implimementation classes
    virtual void		removeReference() = 0;
    //! \brief implemented by derived implimementation classes
    virtual void		release() = 0;

    //! \brief emit profiling start event, with optional 16bit statistic value
	void emitStartEvent( PxU16 stat );
    //! \brief emit profiling stop event, with optional 16bit statistic value
	void emitStopEvent( PxU16 stat );

	/**
     * \brief Execute user run method with wrapping profiling events.
     *
     * Optional entry point for use by CpuDispatchers.
	 */
	PX_INLINE void runProfiled()
	{
		emitStartEvent( 0 );
		run();
		emitStopEvent( mProfileStat );
	}

	/**
     * \brief Specify stop event statistic
     *
     * If called before or while the task is executing, the given value
	 * will appear in the task's event bar in the profile viewer
	 */
	PX_INLINE void setProfileStat( PxU16 stat )
	{
		mProfileStat = stat;
	}

    /**
     * \brief Return TaskManager to which this task was submitted
     *
     * Note, can return NULL if task was not submitted, or has been
     * completed.
     */
	PX_INLINE TaskManager * getTaskManager() const	{ return mTm; }

protected:
	PxU16				mEventID;       //!< Registered profile event ID
	PxU16               mProfileStat;   //!< Profiling statistic
	TaskManager *       mTm;            //!< Owning TaskManager instance
};


/**
 * \brief a BaseTask implementation with deferred execution and full dependencies
 *
 * A Task must be submitted to a TaskManager to to be executed.  Tasks may
 * optionally be named when they are submitted.
 */
class Task : public BaseTask
{
public:
	Task() : mTaskID(0) {}
	virtual ~Task() {}

    //! \brief Task release method implementation
    virtual void release()
	{
		PX_ASSERT(mTm);

        // clear mTm before calling taskCompleted() for safety
		TaskManager *save = mTm;
		mTm = NULL;
		save->taskCompleted( *this );
	}

    //! \brief Inform the TaskManager this task must finish before the given
    //         task is allowed to start.
    PX_INLINE void finishBefore( TaskID taskID )
	{
		PX_ASSERT(mTm);
		mTm->finishBefore( *this, taskID);
	}

    //! \brief Inform the TaskManager this task cannot start until the given
    //         task has completed.
    PX_INLINE void startAfter( TaskID taskID )
	{
		PX_ASSERT(mTm);
		mTm->startAfter( *this, taskID );
	}

    /**
     * Manually increment this task's reference count.  The task will
     * not be allowed to run until removeReference() is called.
     */
    PX_INLINE void addReference()
	{
		PX_ASSERT(mTm);
		mTm->addReference( mTaskID );
	}

    /**
     * Manually decrement this task's reference count.  If the reference
     * count reaches zero, the task will be dispatched.
     */
    PX_INLINE void removeReference()
	{
		PX_ASSERT(mTm);
		mTm->decrReference( mTaskID );
	}

	PX_INLINE TaskID	    getTaskID() const		{ return mTaskID; }

	/**
	 * \brief Called by TaskManager at submission time for initialization
	 *
	 * Perform simulation step initialization here.
	 */
	virtual void submitted()
	{
		mStreamIndex = 0;
		mPreSyncRequired = false;
		mProfileStat = 0;
	}

	PX_INLINE void		requestSyncPoint()		{ mPreSyncRequired = true; }


protected:
    TaskID				mTaskID;            //!< ID assigned at submission
    PxU32               mStreamIndex;       //!< GpuTask CUDA stream index
    bool				mPreSyncRequired;   //!< GpuTask sync flag

	friend class TaskMgr;
    friend class GpuWorkerThread;
};


/**
 * \brief a BaseTask implementation with immediate execution and simple dependencies
 *
 * A LightCpuTask bypasses the TaskManager launch dependencies and will be
 * submitted directly to your scene's CpuDispatcher.  When the run() function
 * completes, it will decrement the reference count of the specified
 * continuation task.
 *
 * You must use a full-blown pxtask::Task if you want your task to be resolved
 * by another pxtask::Task, or you need more than a single dependency to be
 * resolved when your task completes, or your task will not run on the
 * CpuDispatcher.
 */
class LightCpuTask : public BaseTask
{
public:
	LightCpuTask()
		: mCont( NULL )
		, mRefCount( 0 )
	{
	}
	virtual ~LightCpuTask()
	{
	}

    /**
     * \brief initialize this task, prepare to run
     *
     * Provide a TaskManager instance and continuation task. Submission is
     * deferred until the task's mRefCount is decremented to zero.  Note that we
     * only use the TaskManager to query the CpuDispatcher.
	 */
	PX_INLINE void setContinuation( TaskManager& tm, BaseTask *c )
	{
		PX_ASSERT( mRefCount == 0 );
		mRefCount = 1;
		mCont = c;
		mTm = &tm;
		if( mCont )
			mCont->addReference();
	}

    /**
     * \brief initialize this task, prepare to run
     *
     * This overload of init() query's the TaskManager from the continuation
     * task, which cannot be NULL.
	 */
	PX_INLINE void setContinuation( BaseTask *c )
	{
		PX_ASSERT( c );
		PX_ASSERT( mRefCount == 0 );
		mRefCount = 1;
		mCont = c;
		if( mCont )
		{
			mCont->addReference();
			mTm = mCont->getTaskManager();
			PX_ASSERT( mTm );
		}
	}

    /**
     * Manually decrement this task's reference count.  If the reference
     * count reaches zero, the task will be dispatched.
     */
    void removeReference();

    /**
     * Manually increment this task's reference count.  The task will
     * not be allowed to run until removeReference() is called.
     */
    void addReference();

    /**
     * \brief called by CpuDispatcher after run method has completed
     *
     * Decrements the continuation task's reference count, if specified.
     */
    void release()
	{
		if( mCont )
			mCont->removeReference();
	}

protected:

	BaseTask *          mCont;          //!< Continuation task, can be NULL
	volatile PxI32      mRefCount;      //!< Task is dispatched when reaches 0
};


} // end pxtask namespace
} // end physx namespace

#endif
