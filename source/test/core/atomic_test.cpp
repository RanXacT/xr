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
#ifndef XR_CORE_THREADING_ATOMIC_H
#include "xr/core/threading/atomic.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Atomic )


template<typename T>
void LetsTestIt()
{
    T temp = T(0);
    T retVal;

    // Failed CAS
    retVal = xr::Core::AtomicCompareAndSwap(&temp, T(1), T(5));
    XR_ASSERT_ALWAYS_EQ(retVal, T(0));
    XR_ASSERT_ALWAYS_EQ(temp, T(0));

    // Successful CAS
    retVal = xr::Core::AtomicCompareAndSwap(&temp, T(0), T(10));
    XR_ASSERT_ALWAYS_EQ(retVal, T(0));
    XR_ASSERT_ALWAYS_EQ(temp, T(10));
    // Successful CAS
    retVal = xr::Core::AtomicCompareAndSwap(&temp, T(10), T(0) );
    XR_ASSERT_ALWAYS_EQ(retVal, T(10));
    XR_ASSERT_ALWAYS_EQ(temp, T(0));

    // Inc
    retVal = xr::Core::AtomicIncrement(&temp);
    XR_ASSERT_ALWAYS_EQ(retVal, T(0));
    XR_ASSERT_ALWAYS_EQ(temp, T(1));

    // Dec
    retVal = xr::Core::AtomicDecrement(&temp);
    XR_ASSERT_ALWAYS_EQ(retVal, T(1));
    XR_ASSERT_ALWAYS_EQ(temp, T(0));

    // Add
    retVal = xr::Core::AtomicAdd(&temp, T(5));
    XR_ASSERT_ALWAYS_EQ(retVal, T(0));
    XR_ASSERT_ALWAYS_EQ(temp, T(5));

    // Sub
    retVal = xr::Core::AtomicSubtract(&temp, T(5) );
    XR_ASSERT_ALWAYS_EQ(retVal, T(5));
    XR_ASSERT_ALWAYS_EQ(temp, T(0));
}


template<typename T>
void LetsTestCAS(T comparSuccess, T comparFail, T replace)
{
    T test = comparSuccess;
    T retVal;

    // Failed CAS
    retVal = xr::Core::AtomicCompareAndSwap(&test, comparFail, replace);
    XR_ASSERT_ALWAYS_EQ(retVal, comparSuccess);
    XR_ASSERT_ALWAYS_EQ(test, comparSuccess);

    // Successful CAS
    retVal = xr::Core::AtomicCompareAndSwap(&test, comparSuccess, replace);
    XR_ASSERT_ALWAYS_EQ(retVal, comparSuccess);
    XR_ASSERT_ALWAYS_EQ(test, replace);
}


XR_ALIGN_PREFIX( XR_ATOMIC_DOUBLE_POINTER_ALIGN )
struct tester0
{
    uintptr_t t0;
    uintptr_t t1;

    bool operator==(const tester0 &other) const {
        return t0 == other.t0 && t1 == other.t1;
    }
}XR_ALIGN_POSTFIX( XR_ATOMIC_DOUBLE_POINTER_ALIGN );


// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( typeTests )
{
    LetsTestIt<int32_t>();
    LetsTestIt<uint32_t>();
    LetsTestIt<int64_t>();
    LetsTestIt<uint64_t>();
    LetsTestIt<intptr_t>();
    LetsTestIt<uintptr_t>();



    tester0 * t0 = XR_NEW_ALIGN("test", XR_ALIGN_OF(tester0)) tester0;
    tester0 * t1 = XR_NEW_ALIGN("test", XR_ALIGN_OF(tester0)) tester0;
    tester0 * t2 = XR_NEW_ALIGN("test", XR_ALIGN_OF(tester0)) tester0;
    t0->t0 = 1; t0->t1 = 4;
    t1->t0 = 2; t1->t1 = 5;
    t2->t0 = 3; t2->t1 = 6;

    LetsTestCAS( t0, t1, t2);

    XR_DELETE(t0);
    XR_DELETE(t1);
    XR_DELETE(t2);
}


XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
