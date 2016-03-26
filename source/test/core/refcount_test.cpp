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
#ifndef XR_CORE_INTRUSIVE_PTR_H
#include "xr/core/intrusive_ptr.h"
#endif
#ifndef XR_CORE_REFCOUNT_H
#include "xr/core/refcount.h"
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
XR_UNITTEST_GROUP_BEGIN( RefCount )

class ITestType: public xr::Core::RefCounted
{

};

class DTestType: public ITestType
{

};

void TestFuncDerived(DTestType * s1)
{
    DTestType * t = s1;

    // "in" type should be directly applicable to
    // non "in" type.
    xr::Core::intrusive_ptr<ITestType> t1 = s1;
    xr::Core::intrusive_ptr<DTestType> t2 = s1;

    xr::Core::intrusive_ptr<ITestType> t3 = t;
    xr::Core::intrusive_ptr<DTestType> t4 = t;

    // Test permutations
    bool equality = t1 == t2;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = t2 == s1;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = t1 == s1;
    XR_ASSERT_ALWAYS_TRUE(equality);

    // Invert the arguments
    equality = t1 == t2;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = s1 == t2;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = s1 == t1;
    XR_ASSERT_ALWAYS_TRUE(equality);

    // Now against raw pointers
    equality = t1 == t;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = t2 == t;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = s1 == t;
    XR_ASSERT_ALWAYS_TRUE(equality);

    equality = t1 == nullptr;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = t2 == nullptr;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = s1 == nullptr;
    XR_ASSERT_ALWAYS_FALSE(equality);



    // Test permutations
    equality = t1 != t2;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = t2 != s1;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = t1 != s1;
    XR_ASSERT_ALWAYS_FALSE(equality);

    // Invert the arguments
    equality = t1 != t2;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = s1 != t2;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = s1 != t1;
    XR_ASSERT_ALWAYS_FALSE(equality);

    // Now against raw pointers
    equality = t1 != t;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = t2 != t;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = s1 != t;
    XR_ASSERT_ALWAYS_FALSE(equality);

    equality = t1 != nullptr;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = t2 != nullptr;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = s1 != nullptr;
    XR_ASSERT_ALWAYS_TRUE(equality);

    // Now test assignments
    t1 = s1;
    t2 = s1;
    t3 = t;
    t4 = t;
}

void TestFuncBase(ITestType * s1)
{
    xr::Core::intrusive_ptr<ITestType> t1 = s1;


    // Test permutations
    bool equality = t1 == s1;
    XR_ASSERT_ALWAYS_TRUE(equality);

    // Invert the arguments
    equality = s1 == t1;
    XR_ASSERT_ALWAYS_TRUE(equality);

    // Now against raw pointers
    equality = t1 == s1;
    XR_ASSERT_ALWAYS_TRUE(equality);

    equality = t1 == nullptr;
    XR_ASSERT_ALWAYS_FALSE(equality);
    equality = s1 == nullptr;
    XR_ASSERT_ALWAYS_FALSE(equality);


    equality = t1;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = !t1;
    XR_ASSERT_ALWAYS_FALSE(equality);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Basic )
{
    DTestType * t = XR_NEW("RefCountTestType") DTestType();

    xr::Core::intrusive_ptr<DTestType> s  = t;

    bool equality = s;
    XR_ASSERT_ALWAYS_TRUE(equality);
    equality = !s;
    XR_ASSERT_ALWAYS_FALSE(equality);

    {
        xr::Core::intrusive_ptr<DTestType> n;
        equality = n;
        XR_ASSERT_ALWAYS_FALSE(equality);
        equality = !n;
        XR_ASSERT_ALWAYS_TRUE(equality);
    }

    // These functions test normal and casted operation.
    TestFuncDerived(s.Get());
    TestFuncBase(s.Get());

    // Make sure none of the calls have leaked reference counts.
    size_t count = s->UnsafeGetRefCount();
    XR_ASSERT_ALWAYS_EQ(count, 2);

    // Detatch smart pointer
    s = nullptr;

    count = t->UnsafeGetRefCount();
    XR_ASSERT_ALWAYS_EQ(count, 1);

    // Should delete the object 
    // (Will trigger leak detection logic if not)
    t->Release();
}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
