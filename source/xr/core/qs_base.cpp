// ######################################################################################### - FILE
/*!

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_DETAIL_QS_BASE_H
#include "xr/core/detail/qs_base.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_THREADING_MUTEX_H
#include "xr/core/threading/mutex.h"
#endif
#ifndef XR_CORE_THREADING_MONITOR_H
#include "xr/core/threading/monitor.h"
#endif
#ifndef XR_CORE_THREADING_THREAD_H
#include "xr/core/threading/thread.h"
#endif
#ifndef XR_CORE_LOG_H
#include "xr/core/log.h"
#endif
// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core { namespace detail {

static LogHandle sQueueLogHandle("xr.queue");

// ***************************************************************************************** - TYPE
/*! internal type to manage threading. This allows some flexibility.
    It is in a separate type so that the templated Stack object doesn't
    Need to expose the platform code in it's header file. */
// ***************************************************************************************** - TYPE
class QSProtector
{
public:
    QSProtector(size_t maxCount);
    ~QSProtector();
    /// Returns the number of entries available to insert
    size_t InsertionLock();
    // Pass in the number of entries actually inserted
    void InsertionUnLock( size_t numInserted );
    /// Returns the number of entries that can be removed
    size_t RemovalLock( );
    /// pass in the number of entries actually removed
    void RemovalUnLock( size_t numRemoved );
    inline void Kick();
    inline size_t UnsafeGetAvailableCount()
    {
        return mCurrentCount;
    }
    inline size_t UnsafeGetFreeCount()
    {
        return kMaxCount - mCurrentCount;
    }
private:
    QSProtector & operator=( const QSProtector & );
    xr::Core::Mutex      mMutex;
    xr::Core::Monitor    mConditionItemRemoved;
    xr::Core::Monitor    mConditionItemAdded;

    volatile size_t  mCurrentCount;
    const size_t     kMaxCount;
};

// --------------------------------------------------------------------------------------  FUNCTION
/* */
// --------------------------------------------------------------------------------------  FUNCTION
QSProtector::QSProtector(size_t maxCount): mMutex(), mConditionItemRemoved(), mConditionItemAdded(), mCurrentCount(0), kMaxCount(maxCount)
{
    //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "::%p:%d::Create", this, xr::Core::Thread::GetCurrentThreadID());
}
// --------------------------------------------------------------------------------------  FUNCTION
/* */
// --------------------------------------------------------------------------------------  FUNCTION
QSProtector::~QSProtector()
{
    //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "::%p:%d::Delete", this, xr::Core::Thread::GetCurrentThreadID());
}
// --------------------------------------------------------------------------------------  FUNCTION
/* */
// --------------------------------------------------------------------------------------  FUNCTION
size_t QSProtector::InsertionLock()
{
    mMutex.Lock();
    //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "[:%p:%d:", this, xr::Core::Thread::GetCurrentThreadID());
    
    // If the queue is full, wait for something to be removed
    size_t temp = mCurrentCount;
    while( temp >= kMaxCount)
    {
        //XR_LOG_TRACE_MESSAGE(&sQueueLogHandle, "WaitForRem/]" XR_EOL);
        mConditionItemRemoved.Wait(mMutex);
        temp = mCurrentCount;
        //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "[:%p:%d:Waking:", this, xr::Core::Thread::GetCurrentThreadID());
    }

    return kMaxCount - mCurrentCount;
}
// --------------------------------------------------------------------------------------  FUNCTION
/* */
// --------------------------------------------------------------------------------------  FUNCTION
void QSProtector::InsertionUnLock( size_t numPushd )
{
    size_t temp = mCurrentCount;
    temp += numPushd;
    mCurrentCount = temp;
    XR_ASSERT_DEBUG_LE_M(mCurrentCount, kMaxCount, "Queue count exceeded max.");
    //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "Add:%d]" XR_EOL, temp);
    mConditionItemAdded.Signal();
    
    // If there's newly removed items, wake up a friend. This causes less contention than calling
    // broadcast
    if (mCurrentCount < kMaxCount)
        mConditionItemRemoved.Signal();
    mMutex.Unlock();

}
// --------------------------------------------------------------------------------------  FUNCTION
/* */
// --------------------------------------------------------------------------------------  FUNCTION
size_t QSProtector::RemovalLock()
{
    mMutex.Lock();
    //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "<:%p:%d:", this, xr::Core::Thread::GetCurrentThreadID());

    // If the queue is empty, wait for something to be added.
    size_t temp = mCurrentCount;
    while( temp <= 0 )
    {
        //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "WaitingForAdd/>" XR_EOL);
        mConditionItemAdded.Wait(mMutex);
        temp = mCurrentCount;
        //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "<:%p:%d:Waking", this, xr::Core::Thread::GetCurrentThreadID());
    }

    XR_ASSERT_DEBUG_EQ_M(mCurrentCount, temp, "Queue count exceeded max.");
    return mCurrentCount;
}
// --------------------------------------------------------------------------------------  FUNCTION
/* */
// --------------------------------------------------------------------------------------  FUNCTION
void QSProtector::RemovalUnLock( size_t numPopped )
{
    size_t temp = mCurrentCount;
    temp -= numPopped;
    mCurrentCount = temp;
    XR_ASSERT_DEBUG_LE_M(mCurrentCount, kMaxCount, "Queue count exceeded max.");
    
    //XR_LOG_TRACE_FORMATTED(&sQueueLogHandle, "Rem:%d>" XR_EOL, temp);
    // Wake anyone waiting because the queue was full.
    mConditionItemRemoved.Signal();
    
    // If there's more work, wake up a friend. This causes less contention than calling
    // broadcast
    if (temp > 0)
        mConditionItemAdded.Signal();

    mMutex.Unlock();

    
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void QSProtector::Kick()
{
    mConditionItemRemoved.Broadcast();
    mConditionItemAdded.Broadcast();
}
QSBase::QSBase(size_t size, size_t count, const char * name)
{
    XR_ASSERT_DEBUG_GT_M(size, 0, "Cannot have queue or stack of size 0");
    mBuffer = XR_ALLOC(size + sizeof(QSProtector), name);
    uintptr_t temp = uintptr_t(mBuffer) + uintptr_t(size);
    mProtector = new((void*)temp)QSProtector(count);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template <typename T>
static void DeleteHelper(T * p)
{
    p->~T();
}

QSBase::~QSBase()
{
    DeleteHelper(mProtector);
    XR_FREE(mBuffer);
    mBuffer = nullptr;
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t QSBase::InsertionLock()
{
    return mProtector->InsertionLock();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void QSBase::InsertionUnLock( size_t numInserted )
{
    mProtector->InsertionUnLock(numInserted);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t QSBase::RemovalLock( )
{
    return mProtector->RemovalLock();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void QSBase::RemovalUnLock( size_t numRemoved )
{
    mProtector->RemovalUnLock(numRemoved);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t QSBase::UnsafeGetAvailableCount()
{
    return mProtector->UnsafeGetAvailableCount();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t QSBase::UnsafeGetFreeCount()
{
    return mProtector->UnsafeGetFreeCount();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void QSBase::Kick()
{
    mProtector->Kick();
}

}}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
