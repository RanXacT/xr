// ######################################################################################### - FILE
/*! \file
    \brief Mutex Implementation
    Basic user space Mutex implementation. No timeouts, no inter-process support.
    be sure to release the mutex on the thread that locked it.

    \sa http://en.wikipedia.org/wiki/Critical_section

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_THREADING_MUTEX_H
#define XR_CORE_THREADING_MUTEX_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE
#if defined(XR_PLATFORM_WINDOWS)
// ######################################################################################### - FILE
/*!
 To prevent inclusion of windows.h (or winnt.h more specifically)
 the critical section struct and it's contents are copied here. Any changes
 will require a rebuild with a WINVER update anyway, so at our WINVER,
 this is safe to do. (Note that this is verified in the source file via
 static assertions.
 */
//@{
// ######################################################################################### - FILE
#pragma pack(push, 8)
struct XR_INTERNAL_RTL_CRITICAL_SECTION {
private: // Don't access directly, these are undocumented, but fair game in debugging.
    volatile struct _RTL_CRITICAL_SECTION_DEBUG * DebugInfo;
    volatile long LockCount;
    volatile long RecursionCount;
    volatile void* OwningThread;               // from the thread's ClientId->UniqueThread
    volatile void* LockSemaphore;
    volatile unsigned long * SpinCount;        // force size on 64-bit systems when packed
};
// This is the real type.
struct _RTL_CRITICAL_SECTION;

struct XR_INTERNAL_SRWLOCK {
private: // Don't access directly, these are undocumented, but fair game in debugging.
    uintptr_t Locked : 1;
    uintptr_t Waiting : 1;
    uintptr_t Waking : 1;
    uintptr_t MultipleShared : 1;
    uintptr_t Shared : (XR_PLATFORM_PTR_SIZE * 8) - 4;
};

struct _RTL_SRWLOCK;
#pragma pack(pop)

// ######################################################################################### - FILE
//@}
// ######################################################################################### - FILE

#else
#include <unistd.h>

#if !defined(_POSIX_THREADS)
#error "unintended state."
#endif

#include <pthread.h>

#endif

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/*! \copydoc mutex.h

*/
// ***************************************************************************************** - TYPE
class RecursiveMutex{
public:
    RecursiveMutex();
    ~RecursiveMutex();

    // ------------------------------------------------------------------------------------  MEMBER
    /// non blocking function attempts to immediately obtain the mutex.
    /// Returns true if mutex was acquired.
    // ------------------------------------------------------------------------------------  MEMBER
    bool TryLock()const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Blocking function will not return until the mutex is obtained.
    // ------------------------------------------------------------------------------------  MEMBER
    void Lock()const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Will release a previously obtained mutex. Note that this should be
    /// called from the thread which originally obtained it.
    // ------------------------------------------------------------------------------------  MEMBER
    void Unlock()const ;

    // ------------------------------------------------------------------------------------  MEMBER
    // These return the underlying system types for custom code.
    // These are not often used, but necessary in some circumstances.
    // ------------------------------------------------------------------------------------  MEMBER
#if defined(XR_PLATFORM_WINDOWS)
    inline _RTL_CRITICAL_SECTION * UnderlyingSystemObject()
    {
        return (_RTL_CRITICAL_SECTION*)&mCriticalSection;
    }
#elif defined(_POSIX_THREADS)
    inline pthread_mutex_t         *UnderlyingSystemObject()
    {
        return &mSystemMutex;
    }
#endif

private:

#if defined(XR_PLATFORM_WINDOWS)
    struct XR_INTERNAL_RTL_CRITICAL_SECTION mCriticalSection;
#elif defined(_POSIX_THREADS)
    mutable pthread_mutex_t mSystemMutex;
#endif
};

// ***************************************************************************************** - TYPE
/*! \copydoc mutex.h

*/
// ***************************************************************************************** - TYPE
class Mutex{
public:
    Mutex();
    ~Mutex();

    // ------------------------------------------------------------------------------------  MEMBER
    /// non blocking function attempts to immediately obtain the mutex.
    /// Returns true if mutex was acquired.
    // ------------------------------------------------------------------------------------  MEMBER
    bool TryLock()const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Blocking function will not return until the mutex is obtained.
    // ------------------------------------------------------------------------------------  MEMBER
    void Lock()const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Will release a previously obtained mutex. Note that this should be
    /// called from the thread which originally obtained it.
    // ------------------------------------------------------------------------------------  MEMBER
    void Unlock()const ;

    // ------------------------------------------------------------------------------------  MEMBER
    // These return the underlying system types for custom code.
    // These are not often used, but necessary in some circumstances.
    // ------------------------------------------------------------------------------------  MEMBER
#if defined(XR_PLATFORM_WINDOWS)
    inline _RTL_SRWLOCK  *UnderlyingSystemObject()
    {
        return (_RTL_SRWLOCK*) &mSRWLock;
    }
#elif defined(_POSIX_THREADS)
    inline pthread_mutex_t         *UnderlyingSystemObject()
    {
        return &mSystemMutex;
    }
#endif

private:

#if defined(XR_PLATFORM_WINDOWS)
    struct XR_INTERNAL_SRWLOCK mSRWLock;
#elif defined(_POSIX_THREADS)
    mutable pthread_mutex_t mSystemMutex;
#endif
};
// ***************************************************************************************** - TYPE
/*! \brief Simple wrapper for a mutex lock / unlock operation. which
        This ensures proper unlock when the present scope is left. It should
        be used whenever possible since it prevents early exits in code from
        causing locking errors.
*/
// ***************************************************************************************** - TYPE
template <typename T = Mutex>
class AutoProtectScope{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Enum tracks linked Mutex state for extended use cases.
    // ------------------------------------------------------------------------------------  MEMBER
    enum LockStatus
    {
        kMutexLocked,
        kMutexNotLocked
    };
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief Pass the Mutex object to lock / unlock, this is the typical usage.
        \param mutex mutex to lock in constructor, and unlock in destructor
    */
    // ------------------------------------------------------------------------------------  MEMBER
    inline AutoProtectScope(const T *mutex) : mpMutex(mutex), mStatus(kMutexLocked)
    {
        if(mutex != nullptr)
        {
            mpMutex->Lock();
        }
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief Allow this class to only unlock (in case the Mutex was already locked).
        This can be useful if you want to have a timeout and handle that condition,
        also useful in other cases where locking with the same scope is impractical.
        \param mutex Mutex object to manipulate
        \param status status of mutex. If the status is passed as kMutexNotLocked
            then the constructor will lock it.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    inline AutoProtectScope(const T *mutex, LockStatus status) : mpMutex(mutex), mStatus(status)
    {
        if(status == kMutexNotLocked)
        {
            mpMutex->Lock();
            mStatus = kMutexLocked;
        }
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Allow this class to only unlock (in case the Mutex was already locked).
    // ------------------------------------------------------------------------------------  MEMBER
    inline ~AutoProtectScope()
    {
        if(mStatus == kMutexLocked)
        {
            mpMutex->Unlock();
            mStatus = kMutexNotLocked;
        }
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Allow the mutex to be unlocked early for special circumstances (not normal operation)
    // ------------------------------------------------------------------------------------  MEMBER
    void Unlock();

private:
    // ------------------------------------------------------------------------------------  MEMBER
    // No copy construction
    // ------------------------------------------------------------------------------------  MEMBER
    inline AutoProtectScope(const AutoProtectScope&);
    inline AutoProtectScope& operator= (AutoProtectScope const&);

    const T      *mpMutex;
    LockStatus        mStatus;
};



}} // namespace
#endif //#ifndef XR_CORE_THREADING_MUTEX_H
