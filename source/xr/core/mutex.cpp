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
#ifndef XR_CORE_THREADING_MUTEX_H
#include "xr/core/threading/mutex.h"
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

#ifndef XR_CORE_MEM_UTILS_H
#include "xr/core/mem_utils.h"
#endif

#endif

#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ######################################################################################### - FILE
// ######################################################################################### - FILE
template <>
void AutoProtectScope<Mutex>::Unlock()
{
    XR_ASSERT_DEBUG_EQ(mStatus, kMutexLocked);
    if(mStatus == kMutexLocked)
    {
        mpMutex->Unlock();
        mStatus = kMutexNotLocked;
    }
}
template <>
void AutoProtectScope<RecursiveMutex>::Unlock()
{
    XR_ASSERT_DEBUG_EQ(mStatus, kMutexLocked);
    if(mStatus == kMutexLocked)
    {
        mpMutex->Unlock();
        mStatus = kMutexNotLocked;
    }
}
// ######################################################################################### - FILE
// ######################################################################################### - FILE

#if defined(XR_PLATFORM_WINDOWS)
// ######################################################################################### - FILE
// Some basic validation of a fundamental assumption.
// ######################################################################################### - FILE
static_assert(sizeof(XR_INTERNAL_RTL_CRITICAL_SECTION) == sizeof(RTL_CRITICAL_SECTION), "Internal critical section definition does not match windows");

// ######################################################################################### - FILE
// Mutex Members
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Mutex::Mutex()
{
    InitializeSRWLock ((SRWLOCK *)&mSRWLock);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Mutex::~Mutex()
{
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool Mutex::TryLock()const
{
    BOOLEAN ret = TryAcquireSRWLockExclusive((SRWLOCK*)&mSRWLock);
    return ret != FALSE;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Mutex::Lock()const
{
    AcquireSRWLockExclusive((SRWLOCK*)&mSRWLock);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Mutex::Unlock()const
{
    ReleaseSRWLockExclusive((SRWLOCK*)&mSRWLock);
}

// ######################################################################################### - FILE
// RecursiveMutex Members
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
RecursiveMutex::RecursiveMutex()
{
    InitializeCriticalSection((CRITICAL_SECTION*)&mCriticalSection);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
RecursiveMutex::~RecursiveMutex()
{
    DeleteCriticalSection((CRITICAL_SECTION*)&mCriticalSection);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool RecursiveMutex::TryLock()const
{
    BOOL ret = TryEnterCriticalSection((CRITICAL_SECTION*)&mCriticalSection);
    return ret != FALSE;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RecursiveMutex::Lock()const
{
    EnterCriticalSection((CRITICAL_SECTION*)&mCriticalSection);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RecursiveMutex::Unlock()const
{
    LeaveCriticalSection((CRITICAL_SECTION*)&mCriticalSection);
}


#elif defined(_POSIX_THREADS)
// ######################################################################################### - FILE
// ######################################################################################### - FILE

inline void HandleErrno(int errvalue, const char * where)
{
    XR_ASSERT_ALWAYS_EQ_FM(errvalue, 0, "Error: Error returned from %s. errno:%d", where, errvalue);
}

// ######################################################################################### - FILE
// Mutex Simple Members
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Mutex::Mutex()
{
    MemClear8(&mSystemMutex, sizeof(mSystemMutex));
    pthread_mutexrttr_t attr;
    int errval = pthread_mutexrttr_init(&attr);
    HandleErrno(errval, "pthread_mutextattr_init");
    pthread_mutexrttr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    HandleErrno(errval, "pthread_mutexrttr_settype");
    errval = pthread_mutex_init(&mSystemMutex, &attr);
    HandleErrno(errval, "pthread_mutex_init");
    errval = pthread_mutexrttr_destroy(&attr);
    HandleErrno(errval, "pthread_mutexrttr_destroy");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Mutex::~Mutex()
{
    int errval = pthread_mutex_destroy(&mSystemMutex);
    HandleErrno(errval, "pthread_mutex_destroy");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool Mutex::TryLock() const
{
    int errval = pthread_mutex_trylock(&mSystemMutex);
    HandleErrno(errval, "pthread_mutex_trylock");
    if(errval == 0)
    {
        return true;
    }
    return false;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Mutex::Lock() const
{
    int errval = pthread_mutex_lock(&mSystemMutex);
    HandleErrno(errval, "pthread_mutex_lock");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Mutex::Unlock() const
{
    int errval = pthread_mutex_unlock(&mSystemMutex);
    HandleErrno(errval, "pthread_mutex_unlock");
}

#else // Platform
#error "Need a Mutex implementation for this platform (or addit to an existing platform)"
#endif // Platform

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
