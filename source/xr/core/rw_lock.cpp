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
#ifndef XR_CORE_THREADING_RW_LOCK_H
#include "xr/core/threading/rw_lock.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_THREADING_THREAD_H
#include "xr/core/threading/thread.h"
#endif

#if defined(XR_PLATFORM_WINDOWS)
XR_DISABLE_ALL_WARNINGS()
#include <windows.h>
XR_RESTORE_ALL_WARNINGS()
#else

// Unix variants.
#include <unistd.h>
#if defined(_POSIX_THREADS)
#include <pthread.h>
#include <errno.h>
#endif

#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

#if defined(XR_PLATFORM_WINDOWS)
// ######################################################################################### - FILE
// Some basic validation of a fundamental assumption.
// ######################################################################################### - FILE
    static_assert(sizeof(XR_RTL_SRWLOCK) == sizeof(RTL_SRWLOCK), "Internal srwlock definition does not match windows");

// ######################################################################################### - FILE
// RWLock Members
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
RWLock::RWLock()
{
    InitializeSRWLock((RTL_SRWLOCK*)&mRWLock);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
RWLock::~RWLock()
{
    // Nothing to do
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool RWLock::TryLockRead()const
{
    BOOL ret = TryAcquireSRWLockShared((RTL_SRWLOCK*)&mRWLock);
    return ret != FALSE;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RWLock::LockRead()const
{
    AcquireSRWLockShared((RTL_SRWLOCK*)&mRWLock);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RWLock::UnlockRead()const
{
    ReleaseSRWLockShared((RTL_SRWLOCK*)&mRWLock);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool RWLock::TryLockWrite()const
{
    BOOL ret = TryAcquireSRWLockExclusive((RTL_SRWLOCK*)&mRWLock);
    return ret != FALSE;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RWLock::LockWrite()const
{
    AcquireSRWLockExclusive((RTL_SRWLOCK*)&mRWLock);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RWLock::UnlockWrite()const
{
    ReleaseSRWLockExclusive((RTL_SRWLOCK*)&mRWLock);
}

#elif defined(_POSIX_THREADS)
// ######################################################################################### - FILE
// ######################################################################################### - FILE

inline void HandleErrno(int errvalue, const char * where)
{
    XR_ASSERT_ALWAYS_EQ_FM(errvalue, 0, "Error: Error returned from %s. errno:%d", where, errvalue);
}

// ######################################################################################### - FILE
// RWLock Simple Members
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
RWLock::RWLock()
{
    pthread_rwlockattr_t    attr;
    int errval = pthread_rwlockattr_init(&attr);
    HandleErrno(errval, "pthread_rwlockattr_init");
    errval = pthread_rwlock_init(&mRWLock, &attr);
    HandleErrno(errval, "pthread_rwlock_init");
    errval = pthread_rwlockattr_destroy(&attr);
    HandleErrno(errval, "pthread_rwlock_destroy");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
RWLock::~RWLock()
{
    int errval = pthread_rwlock_destroy(&mRWLock);
    HandleErrno(errval, "pthread_rwlock_destroy");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool RWLock::TryLockRead() const
{
    int errval = pthread_rwlock_tryrdlock(&mRWLock);
    HandleErrno(errval, "pthread_rwlock_tryrdlock");
    if(errval == 0)
    {
        return true;
    }
    return false;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RWLock::LockRead() const
{
    int errval = pthread_rwlock_rdlock(&mRWLock);
    HandleErrno(errval, "pthread_rwlock_rdlock");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RWLock::UnlockRead() const
{
    int errval = pthread_rwlock_unlock(&mRWLock);
    HandleErrno(errval, "pthread_rwlock_unlock");
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool RWLock::TryLockWrite() const
{
    int errval = pthread_rwlock_trywrlock(&mRWLock);
    HandleErrno(errval, "pthread_rwlock_trywrlock");
    if(errval == 0)
    {
        return true;
    }
    return false;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RWLock::LockWrite() const
{
    int errval = pthread_rwlock_wrlock(&mRWLock);
    HandleErrno(errval, "pthread_rwlock_wrlock");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RWLock::UnlockWrite() const
{
    int errval = pthread_rwlock_unlock(&mRWLock);
    HandleErrno(errval, "pthread_rwlock_unlock");
}
#else // Platform
#error "Need a rwlock implementation for this platform (or addit to an existing platform)"
#endif // Platform

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
