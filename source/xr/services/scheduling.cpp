// ######################################################################################### - FILE
/*!

Job System:
Job system utilizes a thread pool to run ready jobs. Job dependencies are
maintained internally by forward dependencies (i.e. when a job finishes it
decrements downstream jobs and readies them if their counts reach 0).


Timing issues are prevented using the following means:
+ FreeList / ReadyList: These are encapsulated and thread safety is assumed
  by the underlying types. Lockless / other versions can be used in the future
  should performance be acceptable.
+ WaitOn() is implemented using a monitor, which requires a user space mutex.
+ Event Management: protected by the WaitOn Mutex. Since the Mutex is held
  only for very small periods of time contention is hopefully minimized.
  Also since this mutex must be locked for a completed job anyway it's
  additive cost is small.


\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_SERVICES_SCHEDULING_H
#include "xr/services/scheduling.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_LOG_H
#include "xr/core/log.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_THREADING_THREAD_H
#include "xr/core/threading/thread.h"
#endif
#ifndef XR_CORE_THREADING_ATOMIC_H
#include "xr/core/threading/atomic.h"
#endif
#ifndef XR_CORE_CONTAINERS_BLOCKING_QUEUE_H
#include "xr/core/threading/blocking_queue.h"
#endif
#ifndef XR_CORE_CONTAINERS_BLOCKING_STACK_H
#include "xr/core/threading/blocking_stack.h"
#endif
#ifndef XR_CORE_THREADING_MUTEX_H
#include "xr/core/threading/mutex.h"
#endif
#ifndef XR_CORE_THREADING_MONITOR_H
#include "xr/core/threading/monitor.h"
#endif
#include "xr/core/static_profile.h"
#include <malloc.h> // alloca

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Scheduling{

static Core::LogHandle sScedulerLogHandle("xr.scheduling");
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
enum JobState
{
    XR_JOB_FREE,
    XR_JOB_ALLOCATED,
    XR_JOB_RUNNING
};
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
class JobThread: public Core::Thread
{
public:
    JobThread(): mReadyQueue(nullptr), mManager(nullptr) {}
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    uintptr_t Run() XR_OVERRIDE;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    xr::Core::BlockingQueue<JobInstance *> * mReadyQueue;
    // ------------------------------------------------------------------------------------  MEMBER
    /// \internal Not used directly, only here for debugging.
    // ------------------------------------------------------------------------------------  MEMBER
    IManager               * mManager;
};
// ***************************************************************************************** - TYPE
/*
*/
// ***************************************************************************************** - TYPE
class JobInstance
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void RunOnce(Core::BlockingStack<JobInstance*> *freeList, Core::BlockingQueue<JobInstance*> *readyList)
    {
        mFreeList = freeList;
        mReadyList = readyList;
    }

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandle Initialize(Core::Runnable r, size_t antecedentCount = 0);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandle Initialize(Core::Runnable r, size_t antecedentCount, const Core::Arguments *a);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void Release();

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    JobInstance * Run();
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    bool IsComplete(uint64_t xid);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void AppendAntecedent(JobInstance * source, uint64_t source_xid);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Returns the number of antecedents already completed.
    // ------------------------------------------------------------------------------------  MEMBER
    size_t AppendAntecedents(JobHandle * handles, size_t antecedentCount);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Special function used internally for jobs instances that have not
    /// been returned yet and we can guarantee are not started nor in
    /// thread contention.
    // ------------------------------------------------------------------------------------  MEMBER
    void AppendAntecedent_NotStarted_NoLock(JobInstance * source);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void AppendBarrier(size_t count);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void Notify();
    // ------------------------------------------------------------------------------------  MEMBER
    // This version returns an enabled job instead of enqueueing it.
    // ------------------------------------------------------------------------------------  MEMBER
    JobInstance * NotifyReturnOnEnabled();
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline void WaitOn(uint64_t xid);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline uint64_t GetXid() const;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Nothing to do here.
    /// Call Initialize explicitly.
    // ------------------------------------------------------------------------------------  MEMBER
    JobInstance() { }
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~JobInstance() { }
private:
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    static void AddNotificationLocked(JobInstance * source, JobInstance * notifies);

    // ------------------------------------------------------------------------------------  MEMBER
    /// Monitor used for free list.
    // ------------------------------------------------------------------------------------  MEMBER
    static xr::Core::Mutex        sMutex;
    static xr::Core::Monitor      sMonitor;
    // ------------------------------------------------------------------------------------  MEMBER
    /// shared value used to prevent duplicate values, they must be unique.
    // ------------------------------------------------------------------------------------  MEMBER
    static uint64_t         sGlobalXID;
    // ------------------------------------------------------------------------------------  MEMBER
    /// This value is tuned to Keep a JobInstance at 16 pointers.
    // ------------------------------------------------------------------------------------  MEMBER
#if XR_PLATFORM_PTR_SIZE == 4
    static const size_t  kEventListNumAllocated = 5;
#else
    static const size_t  kEventListNumAllocated = 6;
#endif

    // ------------------------------------------------------------------------------------  MEMBER
    /// This array size is tuned to Keep a JobInstance at 16 pointers.
    // ------------------------------------------------------------------------------------  MEMBER
    static const size_t  kEventListMaxUsable = kEventListNumAllocated-1;


    // ------------------------------------------------------------------------------------  MEMBER
    /// Our Unique ID
    // ------------------------------------------------------------------------------------  MEMBER
    volatile uint64_t          mXID;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Update mRemainingAntecedents atomically
    // ------------------------------------------------------------------------------------  MEMBER
    volatile uintptr_t         mRemainingAntecedents;
    // ------------------------------------------------------------------------------------  MEMBER
    /// This is where it goes once ready
    // ------------------------------------------------------------------------------------  MEMBER
    Core::BlockingQueue<JobInstance*>* mReadyList;
    // ------------------------------------------------------------------------------------  MEMBER
    /// This is where it goes when done
    // ------------------------------------------------------------------------------------  MEMBER
    Core::BlockingStack<JobInstance*>* mFreeList;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Runnable Object
    // ------------------------------------------------------------------------------------  MEMBER
    Core::Runnable             mRunnable;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Protected by sMutex
    // ------------------------------------------------------------------------------------  MEMBER
    volatile uintptr_t         mEventListCount;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Protected by sMutex
    // ------------------------------------------------------------------------------------  MEMBER
    JobInstance    *  volatile mEventList[kEventListNumAllocated];
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    Core::Arguments       mArguments;
};

static_assert(sizeof(JobInstance) == (16 * sizeof(void*)), "size validation" );

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
xr::Core::Mutex        JobInstance::sMutex;
xr::Core::Monitor      JobInstance::sMonitor;
uint64_t JobInstance::sGlobalXID = 0;
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
JobHandle  JobInstance::Initialize(Core::Runnable r, size_t antecedentCount, const Core::Arguments *a)
{
    mArguments = a != nullptr ? *a : Core::Arguments(0,0,0,0);
    return Initialize(r, antecedentCount);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
JobHandle  JobInstance::Initialize(Core::Runnable r, size_t antecedentCount)
{
    mRemainingAntecedents  = (uintptr_t)antecedentCount;
    mRunnable              = r;
    mXID                   = xr::Core::AtomicIncrement(&sGlobalXID);;
    mEventListCount        = 0;

    // A memset might be faster...
    for(size_t i = 0; i < kEventListNumAllocated; i++)
    {
        mEventList[i] = nullptr;
    }
    return JobHandle(mXID, this);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void JobInstance::Release()
{
    XR_ASSERT_DEBUG_EQ(mXID, JobHandle::kJobInstanceHandleInvalid);
    mFreeList->Push(this);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
JobInstance * JobInstance::Run()
{
    XR_ASSERT_ALWAYS_EQ(mRemainingAntecedents, 0);

    uint64_t xid = mXID;
    XR_UNUSED(xid);
    XR_ASSERT_ALWAYS_NE(xid, JobHandle::kJobInstanceHandleInvalid);

    // Run the job.
    if(mRunnable != nullptr)
    {
#if (XR_PLATFORM_PTR_SIZE == 4) && defined(XR_COMPILER_MICROSOFT)

        // Need to handle both _cdecl (1 arg) and _thiscall (at least for 0 arguments)
        // Call convention differences make this simple as putting the argument into both
        // ecx (_thiscall) and pushing it onto the stack (cdecl). Differences in stack 
        // cleanup are safe because the function takes no arguments so the stack cleanup is 
        // benign.
        __asm mov  ecx, dword ptr[this];
        __asm add  ecx, mArguments;
        __asm mov  edx, dword ptr[this];
        __asm mov  edx, dword ptr[edx + mRunnable];;
        __asm push ecx;
        __asm call edx;
        __asm add  esp,4;

#elif (XR_PLATFORM_PTR_SIZE == 4) && defined(XR_COMPILER_GCC)

        // Lambda expects ecx to have pointer to captures...
        // normal functions expect it on stack. Here we place it in both.
        // I can't find any documentation on this but it is clear based on disasm
        // at least as of gcc 4.7 and things like this rarely change for fear of 
        // breaking compatibility across compiler versions.
        asm(
            "movl %0, %%ecx ;"
            "movl %1, %%edx ;"
            "push %%ecx ;"
            "call *%%edx ;"
            "pop %%ecx ;"
            : : "r"(&mArguments), "r"(mRunnable) : "eax", "ecx", "edx"
            );

#else
        // X64's (and most other arch's) ABI is less complicated
        // no need for assembly to call function.
        mRunnable(&mArguments);
#endif

    }

    //`````````````````````````````````````````````````````````````````
    // Now do post run processing.
    sMutex.Lock();

    // Clear the XID, this signals the job as done.
    mXID                       = JobHandle::kJobInstanceHandleInvalid;

    sMutex.Unlock();
    sMonitor.Broadcast();

    //`````````````````````````````````````````````````````````````````
    // Now process the event list. (Outside of the monitor lock!)
    size_t count = (size_t)mEventListCount;
    XR_ASSERT_ALWAYS_LE(count, kEventListNumAllocated);

    // Optimization: Often a job will enable other jobs, in this case
    // just run the newly enabled job, it is probably related.
    JobInstance * first = nullptr;
    uintptr_t i = 0;
    for(; i < count; i++)
    {
        first = mEventList[i]->NotifyReturnOnEnabled();
        if(first != nullptr)
        {
            ++i;
            break;
        }
    }
    // Schedule any additional jobs.
    for(; i < count; i++)
    {
        mEventList[i]->Notify();
    }

    Release();

    return first;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool JobInstance::IsComplete(uint64_t xid)
{
    if(mXID != xid)
    {
        // Definitely done. XID is invalidated after the job is complete.
        return true;
    }

    return false;
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void JobInstance::AddNotificationLocked(JobInstance * source, JobInstance * notifies)
{
    //`````````````````````````````````````````````````````````````````
    // Job Not done yet, Add it to the list.
    uintptr_t tempCount = source->mEventListCount;

    //`````````````````````````````````````````````````````````````````
    // Normal case, we can insert into the current job instance's memory
    if(tempCount < kEventListMaxUsable )
    {
        XR_ASSERT_ALWAYS_NE(notifies, source);
        source->mEventList[tempCount] = notifies;
        source->mEventListCount = tempCount + 1;
        return;
    }

    //`````````````````````````````````````````````````````````````````
    // Job instance is already full of events... Need to create a dummy
    // event to put this new one in. Generally this case should not
    // happen. If it happens often perhaps one should increase
    // kEventListMaxUsable.
    else if(tempCount == kEventListMaxUsable)
    {
        source->mEventListCount = tempCount + 1;

        JobInstance * dummy = source->mFreeList->Pop();

        dummy->Initialize(nullptr);

        // This JobInstance is not linked in yet and is not
        // in contention. Nor is it full, so insertion is trivial
        // as long as we do it before it is linked.
        dummy->mRemainingAntecedents = 1;
        dummy->mEventListCount       = 1;
        dummy->mEventList[0]         = notifies;

        // Link in the Dummy.
        source->mEventList[tempCount]        = dummy;
        return;
    }

    //`````````````````````````````````````````````````````````````````
    // Find and update the existing dummy instance (or chain)
    else if(tempCount > kEventListMaxUsable)
    {
        AddNotificationLocked(source->mEventList[kEventListMaxUsable],  notifies);
        return;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void JobInstance::AppendAntecedent(JobInstance * source, uint64_t source_xid)
{
    bool added = true;
    sMutex.Lock();
    //`````````````````````````````````````````````````````````````````
    // If the job is done, just notify already.
    if(source->IsComplete(source_xid))
    {
        added = false;
    }
    else
    {
        AddNotificationLocked(source, this);
    }
    sMutex.Unlock();

    // The job was already done.
    if(!added)
    {
        Notify();
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t JobInstance::AppendAntecedents(JobHandle * handles, size_t antecedentCount)
{
    size_t numAlreadyCompleted = 0;
    sMutex.Lock();
    for(size_t i = 0; i < antecedentCount; ++i)
    {
        //`````````````````````````````````````````````````````````````````
        // If the job is done, just notify already.
        if(handles[i].IsDone())
        {
            ++numAlreadyCompleted;
        }
        else
        {
            AddNotificationLocked(handles[i].mInstance, this);
        }
    }
    sMutex.Unlock();

    return numAlreadyCompleted;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void JobInstance::AppendAntecedent_NotStarted_NoLock(JobInstance * source)
{
    AddNotificationLocked(source, this);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void JobInstance::Notify()
{
    uintptr_t initialValue;
    do
    {
        initialValue = mRemainingAntecedents;
        // If the counter is 1, there is no contention. Just run it. 
        if(initialValue == 1)
        {
            mRemainingAntecedents = 0;
            mReadyList->Enqueue(this);
            return;
        }

        // Can move to "NoFence" version
    } while( xr::Core::AtomicCompareAndSwap(&mRemainingAntecedents, initialValue, initialValue-1) != initialValue);

    XR_ASSERT_ALWAYS_NE_M(initialValue, 0, "Job has invalid antecedents count!");
    return;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
JobInstance * JobInstance::NotifyReturnOnEnabled()
{
    uintptr_t initialValue;
    do
    {
        initialValue = mRemainingAntecedents;
        // If the counter is 1, there is no contention. Just run it. 
        if(initialValue == 1)
        {
            mRemainingAntecedents = 0;
            return this;
        }

        // Can move to "NoFence" version
    } while( xr::Core::AtomicCompareAndSwap(&mRemainingAntecedents, initialValue, initialValue-1) != initialValue);

    XR_ASSERT_ALWAYS_NE_M(initialValue, 0, "Job has invalid antecedents count!");
    return nullptr;
}
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
void JobInstance::WaitOn(uint64_t xid)
{
    sMutex.Lock();

    // Test predicate:
    while(!IsComplete(xid))
    {
        sMonitor.Wait(sMutex);
    }

    sMutex.Unlock();

}
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
uint64_t JobInstance::GetXid() const
{
    return mXID;
}
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
void JobInstance::AppendBarrier( size_t count )
{
    xr::Core::AtomicAdd(&mRemainingAntecedents , count);
}


class ManagerInternal : public IManager{
public:

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandle InsertReady(
        Core::Runnable r,
        const Core::Arguments *args = nullptr) XR_OVERRIDE;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandle InsertReady(
        size_t runnableCount,
        Core::Runnable * runnableArray,
        size_t argumentsCount,
        const Core::Arguments * argsArray) XR_OVERRIDE;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandleBlocked InsertBlocked(
        Core::Runnable r,
        const Core::Arguments *args = nullptr) XR_OVERRIDE;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandleBlocked InsertBlocked(
        size_t runnableCount,
        Core::Runnable * runnableArray,
        size_t argumentsCount,
        const Core::Arguments * argsArray) XR_OVERRIDE;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandle InsertAfter(
        Core::Runnable r,
        const Core::Arguments *args,
        JobHandle * handle0,
        size_t handleCount) XR_OVERRIDE;

private:

    InitializeOptions                mOptions;
    JobInstance                    * mInstances;
    JobThread                      * mThreads;
    xr::Core::BlockingQueue<JobInstance *> * mReadyList;
    xr::Core::BlockingStack<JobInstance *> * mFreeList;

    friend class IManager;
};

// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
JobHandle ManagerInternal::InsertReady(Core::Runnable r, const Core::Arguments *args)
{
    JobHandle h = mFreeList->Pop()->Initialize(r, 0, args);
    mReadyList->Enqueue(h.mInstance);
    return h;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
JobHandle ManagerInternal::InsertReady(
    size_t runnableCount,
    Core::Runnable *runnableArray,
    size_t argumentsCount,
    const Core::Arguments * argsArray)
{
    // Need an extra instance to wrap the collection.
    JobInstance ** instances = (JobInstance **)alloca( sizeof(JobInstance*) * (runnableCount+1));
    mFreeList->Pop(instances, (runnableCount+1));

    // Make the last one the wrapper job.
    JobHandle hWrap = instances[runnableCount]->Initialize(nullptr);

    if(argumentsCount == 0)
    {
        // This can be optimized.
        for(size_t i = 0; i < runnableCount; i++)
        {
            instances[i]->Initialize(runnableArray[i], 0);
            hWrap.mInstance->AppendAntecedent_NotStarted_NoLock(instances[i]);
        }
    }
    else if(argumentsCount == 1)
    {
        // This can be optimized.
        for(size_t i = 0; i < runnableCount; i++)
        {
            instances[i]->Initialize(runnableArray[i], 0, &argsArray[0]);
            hWrap.mInstance->AppendAntecedent_NotStarted_NoLock(instances[i]);
        }
    }
    else 
    {
        XR_ASSERT_ALWAYS_EQ(runnableCount, argumentsCount);

        // This can be optimized.
        for(size_t i = 0; i < runnableCount; i++)
        {
#if defined(XR_COMPILER_MICROSOFT)
			// Supress bogus overflow warning.
#pragma warning(disable: 6385)
#endif
            instances[i]->Initialize(runnableArray[i], 0, &argsArray[i]);
            hWrap.mInstance->AppendAntecedent_NotStarted_NoLock(instances[i]);
        }
    }

    mReadyList->Enqueue(instances, runnableCount);

    return hWrap;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
JobHandleBlocked ManagerInternal::InsertBlocked(
    size_t runnableCount,
    Core::Runnable *runnableArray,
    size_t argumentsCount,
    const Core::Arguments * argsArray)
{
    

    JobInstance ** instances = (JobInstance **)alloca( sizeof(JobInstance*) * (runnableCount+1));
    mFreeList->Pop(instances, (runnableCount+1));

    JobHandle hWrap = instances[runnableCount]->Initialize(nullptr, 1);

    if(argumentsCount == 0)
    {
        // This can be optimized.
        for(size_t i = 0; i < runnableCount; i++)
        {
            instances[i]->Initialize(runnableArray[i], 0);
            instances[i]->AppendAntecedent_NotStarted_NoLock(hWrap.mInstance);
        }
    }
    else if(argumentsCount == 1)
    {
        // This can be optimized.
        for(size_t i = 0; i < runnableCount; i++)
        {
            instances[i]->Initialize(runnableArray[i], 0, &argsArray[0]);
            instances[i]->AppendAntecedent_NotStarted_NoLock(hWrap.mInstance);
        }
    }
    else 
    {
        XR_ASSERT_ALWAYS_EQ(runnableCount, argumentsCount);

        // This can be optimized.
        for(size_t i = 0; i < runnableCount; i++)
        {
            instances[i]->Initialize(runnableArray[i], 0, &argsArray[i]);
            instances[i]->AppendAntecedent_NotStarted_NoLock(hWrap.mInstance);
        }
    }

    return JobHandleBlocked(hWrap);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
xr::Scheduling::JobHandleBlocked ManagerInternal::InsertBlocked( Core::Runnable r, const Core::Arguments *args )
{
    JobHandle hWrap = mFreeList->Pop()->Initialize(r, 1, args);
    return JobHandleBlocked(hWrap);
}
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
JobHandle ManagerInternal::InsertAfter(Core::Runnable r, const Core::Arguments * args, JobHandle * handle0, size_t handleCount)
{
    JobHandleBlocked h (mFreeList->Pop()->Initialize(r, handleCount, args));

    size_t skippedCount = h.mInstance->AppendAntecedents(handle0, handleCount);

    if(skippedCount != 0)
    {
        // Release the ones that were already completed.
        h.ReleaseBarrier(skippedCount);
    }

    return h;
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
IManager * IManager::Initialize(InitializeOptions * options)
{
    ManagerInternal * p = XR_NEW("Manager") ManagerInternal();
    p->mOptions = *options;

    p->mInstances = XR_NEW_ALIGN("Scheduler::Instances", 16)  JobInstance[options->mFreeListSize];
    p->mThreads   = XR_NEW("Scheduler::Threads")    JobThread[options->mNumThreads];
    p->mReadyList = XR_NEW("Scheduler::ReadyQueue") Core::BlockingQueue<JobInstance*>(options->mReadyListSize, "Scheduler::ReadyQueue");
    p->mFreeList  = XR_NEW("Scheduler::FreeList")   Core::BlockingStack<JobInstance*>(options->mFreeListSize, "Scheduler::FreeList");

    for(size_t i = 0; i < options->mNumThreads; i++)
    {
        p->mThreads[i].mManager = p;
        p->mThreads[i].mReadyQueue = p->mReadyList;
        // Start the Thread.
        p->mThreads[i].Start();
    }

    for(size_t i = 0; i < options->mFreeListSize; i++)
    {
        p->mInstances[i].RunOnce(p->mFreeList, p->mReadyList);
        // These are all free.
        p->mFreeList->Push(&p->mInstances[i]);
    }

    return p;
}


// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void IManager::Shutdown(IManager * m)
{
    ManagerInternal * sched = (ManagerInternal*)m;
    // This is mainly used as a flag to ensure we intent to shut down
    // (and that a nullptr was not accidentally placed in the queue)
    for(size_t i = 0; i < sched->mOptions.mNumThreads; i++)
    {
        sched->mThreads[i].RequestQuit();
    }

    for(size_t i = 0; i < sched->mOptions.mNumThreads; i++)
    {
        sched->mReadyList->Enqueue(nullptr);
    }

    for(size_t i = 0; i < sched->mOptions.mNumThreads; i++)
    {
        // Join the Threads.
        sched->mThreads[i].Join();
    }

    XR_DELETE(sched->mFreeList);
    XR_DELETE(sched->mReadyList);
    XR_DELETE_ARRAY(sched->mThreads);
    XR_DELETE_ARRAY(sched->mInstances);
    XR_DELETE(sched);
}



// ***************************************************************************************** - TYPE
// JobRunner Functions.
// ***************************************************************************************** - TYPE

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
uintptr_t JobThread::Run()
{
    uint64_t jobCount = 0;
    // This is basically it.
    for(;;)
    {
        JobInstance * ji = mReadyQueue->Dequeue();
        if(ji != nullptr)
        {
            while(ji != nullptr)
            {
                XR_LOG_TRACE_FORMATTED(&sScedulerLogHandle, "Thread:0x%" XR_UINTPTR_PRINTx " Starting JobInstance:0x%p" XR_EOL , this->GetID(), ji);
                ji = ji->Run();
                ++jobCount;
            }
        }
        else
        {
            // This nullptr should be intentionally present.
            XR_ASSERT_ALWAYS_EQ(IsQuitRequested(), true);
            break;
        }
    }


    XR_LOG_DEBUG_FORMATTED(&sScedulerLogHandle, "Thread:0x%" XR_UINTPTR_PRINTx " ran %" XR_UINT64_PRINT " Jobs" XR_EOL , this->GetID(), jobCount);
    return 0;
}
// ***************************************************************************************** - TYPE
// JobHandle Functions.
// ***************************************************************************************** - TYPE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool           JobHandle::IsDone() const  { return mInstance->IsComplete(mXID); }
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void           JobHandle::WaitOn() const  { mInstance->WaitOn(mXID); }


static const uintptr_t kJobBarrierReleaser_Checkword = 0x9719661d;

void JobBarrierReleaser(const ::xr::Core::Arguments * args)
{
    JobInstance * p = (JobInstance *)(args->a0);
    
    XR_ASSERT_ALWAYS_EQ(args->a1, kJobBarrierReleaser_Checkword);
    uint64_t xid = p->GetXid();
    XR_ASSERT_ALWAYS_EQ(args->a2, uintptr_t(xid >> 32) & (XR_UINT32_MAX) );
    XR_ASSERT_ALWAYS_EQ(args->a3, uintptr_t(xid >>  0) & (XR_UINT32_MAX) );

    p->Notify();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Core::Runnable JobHandleBlocked::AddBarrier( xr::Core::Arguments &args, size_t count /*= 1*/ ) const
{
    args.a0 = (uintptr_t)mInstance;
    args.a1 = kJobBarrierReleaser_Checkword; // Random value used for verification.

    uint64_t xid = mInstance->GetXid();
    mInstance->AppendBarrier(count);
    args.a2 = (xid >> 32) & XR_UINT32_MAX; // used for verification.
    args.a3 = (xid >>  0) & XR_UINT32_MAX; // used for verification.
    return &JobBarrierReleaser;
}

void JobHandleBlocked::ReleaseBarrier( size_t count /*= 1*/ )
{
    // Basic check.
    XR_ASSERT_ALWAYS_EQ(mInstance->GetXid(), mXID);

    for(size_t i = 0; i < count; i++)
    {
        mInstance->Notify();
    }
}

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
