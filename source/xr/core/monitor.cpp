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
#ifndef XR_CORE_THREADING_MONITOR_H
#include "xr/core/threading/monitor.h"
#endif
#ifndef XR_CORE_THREADING_MUTEX_H
#include "xr/core/threading/mutex.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
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
// -----------------------------------------------------------------------------------------  MACRO
// Some basic validation of a fundamental assumption.
// -----------------------------------------------------------------------------------------  MACRO
static_assert(sizeof(XR_INTERNAL_RTL_CONDITION_VARIABLE) == sizeof(CONDITION_VARIABLE), "Internal condition variable definition does not match windows");

// ######################################################################################### - FILE
// Monitor Members
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Monitor::Monitor()
{
    InitializeConditionVariable((CONDITION_VARIABLE*) &mCondition);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Monitor::~Monitor()
{
    // Monitor is not destroyed.
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Monitor::Wait(XR_IN xr::Core::Mutex & mutex) const
{
    if (SleepConditionVariableSRW((CONDITION_VARIABLE*)&mCondition, mutex.UnderlyingSystemObject(), INFINITE, 0) == FALSE)
    {
        uint32_t err = (uint32_t)GetLastError();
        XR_ASSERT_ALWAYS_EQ_FM(err, ERROR_TIMEOUT, "SleepConditionVariableSRW Error 0x%8.8" XR_UINT32_PRINTX "!", err);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Monitor::Wait(XR_IN xr::Core::RecursiveMutex & mutex) const
{
    if (SleepConditionVariableCS((CONDITION_VARIABLE*)&mCondition, mutex.UnderlyingSystemObject(), INFINITE) == FALSE)
    {
        WakeAllConditionVariable ((CONDITION_VARIABLE*)&mCondition);
        uint32_t err = (uint32_t)GetLastError();
        XR_ASSERT_ALWAYS_EQ_FM(err, ERROR_TIMEOUT, "SleepConditionVariableCS Error 0x%8.8" XR_UINT32_PRINTX "!", err);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Monitor::Signal() const
{
    WakeConditionVariable ((CONDITION_VARIABLE*)&mCondition);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Monitor::Broadcast() const
{
    WakeAllConditionVariable ((CONDITION_VARIABLE*)&mCondition);
}

#elif defined(_POSIX_THREADS)
// ######################################################################################### - FILE
// ######################################################################################### - FILE

inline void HandleErrno(int errvalue, const char * where)
{
    XR_ASSERT_ALWAYS_EQ_FM(errvalue, 0, "Error: Error returned from %s. errno:%d", where, errvalue);
}

// ######################################################################################### - FILE
// Monitor Simple Members
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Monitor::Monitor()
{
    int err = pthread_cond_init(&mCondition, nullptr);
    HandleErrno(err, "pthread_cond_init");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Monitor::~Monitor()
{
    int err = pthread_cond_destroy(&mCondition);
    HandleErrno(err, "pthread_cond_destroy");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Monitor::Wait(xr::Core::Mutex & mutex) const
{
    int err = pthread_cond_wait(&mCondition, mutex.UnderlyingSystemObject());
    HandleErrno(err, "pthread_cond_wait");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Monitor::Signal() const
{
    int err = pthread_cond_signal(&mCondition);
    HandleErrno(err, "pthread_cond_signal");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Monitor::Broadcast() const
{
    int err = pthread_cond_broadcast(&mCondition);
    HandleErrno(err, "pthread_cond_signal");
}
#else // Platform
#error "Need a Monitor implementation for this platform (or addit to an existing platform)"
#endif // Platform

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
