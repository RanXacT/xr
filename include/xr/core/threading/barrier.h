// ######################################################################################### - FILE
/*! \file
    A barrier is a specialized monitor for preventing threads from
    continuing until a "barrier" has been cleared (released). This assumes
    a single Release is used.

    This is mainly used for futures. A future may block on the data value
    being access before it's been properly written. A non-computer related
    equivalent would be the opening of a store. any number of people may
    line up outside, but until the manager opens the front door they wait.
    Once the store is open anyone may go in as they please.

    \li Waiting Threads Call WaitOn()
    \li The Clearing Thread Calls Release() to "Release" the waiting threads.
    \li GetWaitCount() returns the number of threads thta have cleared the
        barrier.

    \note Do not call WaitOn() multiple times. Each thread should only call it
          once. If knowledge of cleared status is required, the caller should
          cache it.
    \note One should be careful when choosing when to destroy this object.
          GetWaitCount() can be used for this purpose, but another means
          is fine.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_THREADING_BARRIER_H
#define XR_CORE_THREADING_BARRIER_H

#if defined( _MSC_VER )
#pragma once
#endif

// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif
#ifndef XR_CORE_THREADING_MONITOR_H
#include "xr/core/threading/monitor.h"
#endif
// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {
    class Mutex;
    }
}
// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/*!
    */
// ***************************************************************************************** - TYPE
class Barrier
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /// It is generally expected that the caller will have a mutex they
    /// would like to be reused in this barrier.
    // ------------------------------------------------------------------------------------  MEMBER
    Barrier(Mutex *);
    ~Barrier();
    // ------------------------------------------------------------------------------------  MEMBER
    /// returns when the Barrier has been released.
    /// Once called it should not be called again. The caller should cache
    /// the barrier's state based on returning from this WaitOn.
    // ------------------------------------------------------------------------------------  MEMBER
    void WaitOn();
    // ------------------------------------------------------------------------------------  MEMBER
    /// Releases all waiting threads (And any future threads to try to wait
    /// on it).
    // ------------------------------------------------------------------------------------  MEMBER
    void Release();

    // ------------------------------------------------------------------------------------  MEMBER
    /// returns the number of threads that have requested access.
    // ------------------------------------------------------------------------------------  MEMBER
    inline size_t GetWaitCount() {return mWaitCount;}

private:
    Mutex  * mMutex;        ///< Associated Mutex (Required for monitor). See constructor.
    Monitor  mMonitor;      ///< Monitor for synchronization.
    size_t   mWaitCount;    ///< Number of Theads that have Waited on this barrier
    bool     mIsReleased;   ///< True after "Release" Has been called.
};

}}
#endif //#ifndef XR_CORE_THREADING_BARRIER_H
