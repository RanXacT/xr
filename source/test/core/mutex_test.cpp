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
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif

// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Mutex )

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Mutex )
{
    xr::Core::Mutex m;

    m.Lock();
    m.Unlock();
    bool locked = m.TryLock();
    XR_ASSERT_ALWAYS_EQ(locked, true);
    m.Unlock();
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( AutoProtectScope )
{
    xr::Core::Mutex m;

    xr::Core::AutoProtectScope<xr::Core::Mutex> aps(&m);

    aps.Unlock();
}


// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( RecursiveAutoProtectScope )
{
    xr::Core::RecursiveMutex m;

    xr::Core::AutoProtectScope<xr::Core::RecursiveMutex> ap1(&m);
    xr::Core::AutoProtectScope<xr::Core::RecursiveMutex> ap2(&m);
    xr::Core::AutoProtectScope<xr::Core::RecursiveMutex> ap3(&m);

    ap1.Unlock();
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( RecursiveMutexSimple )
{
    xr::Core::RecursiveMutex m;

    m.Lock();
    m.Unlock();
    bool locked = m.TryLock();
    XR_ASSERT_ALWAYS_EQ(locked, true);
    m.Unlock();
}
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( RecursiveMutexRecursive )
{
    xr::Core::RecursiveMutex m;

    m.Lock();
    m.Lock();
    m.Lock();
    m.Unlock();
    m.Unlock();
    m.Unlock();
}


XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
