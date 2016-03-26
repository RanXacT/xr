// ######################################################################################### - FILE
/*! \file
    Basic user space r/w mutex. \sa http://en.wikipedia.org/wiki/Readers-writer_lock

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_THREADING_RW_LOCK_H
#define XR_CORE_THREADING_RW_LOCK_H

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
// ***************************************************************************************** - TYPE
/*!
 To prevent inclusion of windows.h (or winnt.h more specifically)
 the critical section struct and it's contents are copied here. Any changes
 will require a rebuild with a WINVER update anyway, so at our WINVER,
 this is safe to do. (Note that this is verified in the source file via
 static assertions.
 */
//@{
// ***************************************************************************************** - TYPE
typedef struct{
    void * Ptr;
} XR_RTL_SRWLOCK;

// Forward declare the actual type.
struct _RTL_SRWLOCK;

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
/*! \brief Readers / Writer Lock
    \sa http://en.wikipedia.org/wiki/Readers-writer_lock
*/
// ***************************************************************************************** - TYPE
class RWLock{
public:
    RWLock();
    ~RWLock();

    // ------------------------------------------------------------------------------------  MEMBER
    /// non blocking function attempts to immediately obtain the mutex.
    /// Returns true if mutex was acquired.
    // ------------------------------------------------------------------------------------  MEMBER
    bool TryLockRead()const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// non blocking function attempts to immediately obtain the mutex.
    /// Returns true if mutex was acquired.
    // ------------------------------------------------------------------------------------  MEMBER
    bool TryLockWrite()const ;

    // ------------------------------------------------------------------------------------  MEMBER
    /// Blocking function will not return until the mutex is obtained.
    // ------------------------------------------------------------------------------------  MEMBER
    void LockRead()const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Will release a previously obtained mutex. Note that this should be
    /// called from the thread which originally obtained it.
    // ------------------------------------------------------------------------------------  MEMBER
    void UnlockRead()const ;

    // ------------------------------------------------------------------------------------  MEMBER
    /// Blocking function will not return until the mutex is obtained.
    // ------------------------------------------------------------------------------------  MEMBER
    void LockWrite()const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Will release a previously obtained mutex. Note that this should be
    /// called from the thread which originally obtained it.
    // ------------------------------------------------------------------------------------  MEMBER
    void UnlockWrite()const ;

    // ------------------------------------------------------------------------------------  MEMBER
    // These return the underlying system types for custom code.
    // These are not often used, but necessary in some circumstances.
    // ------------------------------------------------------------------------------------  MEMBER
#if defined(XR_PLATFORM_WINDOWS)
    inline struct _RTL_SRWLOCK *UnderlyingSystemObject()
    {
        return  (_RTL_SRWLOCK*) &mRWLock;
    }
#elif defined(_POSIX_THREADS)
    inline pthread_rwlock_t         *UnderlyingSystemObject()
    {
        return &mRWLock;
    }
#endif

private:

#if defined(XR_PLATFORM_WINDOWS)
    XR_RTL_SRWLOCK mRWLock;
#elif defined(_POSIX_THREADS)
    mutable pthread_rwlock_t mRWLock;
#endif
};
}}
#endif //#ifndef XR_CORE_THREADING_RW_LOCK_H
