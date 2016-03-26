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
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
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
XR_UNITTEST_GROUP_BEGIN( Allocator )


struct PODType
{
    int32_t test1;
    char8_t test2;
};

struct NonPODType
{
    static int32_t count;
    NonPODType() : test1(0x12345678), test2(0x42) {
        count++;
    }

    ~NonPODType()
    {
        count--;
    }

    void isValid()
    {
        XR_ASSERT_ALWAYS_EQ(test1, 0x12345678);
        XR_ASSERT_ALWAYS_EQ(test2, 0x42);
    }

    static void Validator(void * type, size_t valcount)
    {
        NonPODType * p = reinterpret_cast<NonPODType*>(type);
        for(size_t i = 0; i < valcount; i++)
        {
            p[i].isValid();
        }
    }

    int32_t test1;
    char8_t test2;
};
int32_t NonPODType::count = 0;

typedef void (*TypeValidator) (void * type, size_t count);

template <typename T>
static void AllocTestHelper(TypeValidator validator)
{
    // Single instance macros
    {

        T * tester = XR_NEW( "test")  T;
        XR_ASSERT_ALWAYS_TRUE(tester != nullptr);
        if(validator != nullptr)
        {
            validator(tester, 1);
        }
        XR_DELETE(tester);

        tester = XR_NEW_ALIGN( "test", XR_ALIGN_OF(T) )  T;
        XR_ASSERT_ALWAYS_TRUE(tester != nullptr);
        if(validator != nullptr)
        {
            validator(tester, 1);
        }
        XR_DELETE(tester);

        tester = XR_NEW_FLAGS( "test", ::xr::Core::kMemNormal )  T;
        XR_ASSERT_ALWAYS_TRUE(tester != nullptr);
        if(validator != nullptr)
        {
            validator(tester, 1);
        }
        XR_DELETE(tester);

        tester = XR_NEW_ALIGN_FLAGS( "test", XR_ALIGN_OF(T), ::xr::Core::kMemNormal )  T;
        XR_ASSERT_ALWAYS_TRUE(tester != nullptr);
        if(validator != nullptr)
        {
            validator(tester, 1);
        }
        XR_DELETE(tester);
    }
    {

        // Array macros
        T * tester = XR_NEW("test") T[10];
        XR_ASSERT_ALWAYS_TRUE(tester != nullptr);
        if(validator != nullptr)
        {
            validator(tester, 10);
        }
        XR_DELETE_ARRAY(tester);


        tester = XR_NEW_FLAGS( "test", ::xr::Core::kMemNormal) T[10];
        XR_ASSERT_ALWAYS_TRUE(tester != nullptr);
        if(validator != nullptr)
        {
            validator(tester, 10);
        }
        XR_DELETE_ARRAY(tester);
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( basic )
{
#if defined(XR_COMPILER_MICROSOFT)
	// Supress constant comparison test warning.
#pragma warning(disable: 6326)
#endif
    // This line doesn't technically belong here, but clang compiler won't do the test
    // statically, so it needed a home and there is no defines_test.cpp (since
    // all of the defines tests should be compile time.
    XR_ASSERT_ALWAYS_EQ(XR_ALIGN_OF(uint64_t),  XR_PLATFORM_UINT64_T_ALIGNMENT  );


    AllocTestHelper< uint8_t         >(nullptr);
    AllocTestHelper< uint16_t        >(nullptr);
    AllocTestHelper< uint32_t        >(nullptr);
    AllocTestHelper< uint64_t        >(nullptr);
    AllocTestHelper<  int8_t         >(nullptr);
    AllocTestHelper<  int16_t        >(nullptr);
    AllocTestHelper<  int32_t        >(nullptr);
    AllocTestHelper<  int64_t        >(nullptr);
    AllocTestHelper< PODType         >(nullptr);
    AllocTestHelper< NonPODType      >(NonPODType::Validator);

    void * p = XR_ALLOC(1, "blah");
    XR_FREE(p);

    p = XR_ALLOC_ALIGN(1, "blah", 1);
    XR_FREE(p);

    p = XR_ALLOC_FLAGS(1, "blah", ::xr::Core::kMemNormal);
    XR_FREE(p);

    p = XR_ALLOC_ALIGN_FLAGS(1, "blah", 1, ::xr::Core::kMemNormal);
    XR_FREE_SZ(p, 1);

    char* str = XR_STRDUP("DuplicateMe", "Allocname");
    XR_FREE(str);

    str = XR_STRDUP_FLAGS("DuplicateMe", "Allocname", ::xr::Core::kMemNormal);
    XR_FREE(str);



    wchar_t* wstr = XR_STRDUP( XR_STR_WCHAR_T("DuplicateMe"), "Allocname");
    XR_FREE(wstr);

    wstr = XR_STRDUP_FLAGS( XR_STR_WCHAR_T("DuplicateMe"), "Allocname", ::xr::Core::kMemNormal);
    XR_FREE(wstr);
}

class IClass1
{
public:
    virtual ~IClass1() {}
    virtual void Do() {}
};

class IClass2
{
public:
    virtual ~IClass2() {}
    virtual void Something() {}
};

class IClass3
{
public:
    virtual ~IClass3() {}
    virtual void Complex() {}
};

class ImplTest: public IClass1, public IClass2, public IClass3
{
public:
    ImplTest(): blah(100) {}
    ~ImplTest()
    {

    }

    virtual void Another() {}

    int blah;
};

template <typename T>
void TestDeleteHelper()
{
    ImplTest * p = XR_NEW("test") ImplTest();
    XR_ASSERT_ALWAYS_EQ(p->blah, 100);
    T * d = p;
    XR_DELETE(d);

    p = XR_NEW_ALIGN("test", 16) ImplTest();
    XR_ASSERT_ALWAYS_EQ(p->blah, 100);
    d = p;
    XR_DELETE(d);

    p = XR_NEW_ALIGN_FLAGS("test", 16, xr::Core::kMemNormal) ImplTest();
    XR_ASSERT_ALWAYS_EQ(p->blah, 100);
    d = p;
    XR_DELETE(d);

    p = XR_NEW_FLAGS("test", xr::Core::kMemNormal) ImplTest();
    XR_ASSERT_ALWAYS_EQ(p->blah, 100);
    d = p;
    XR_DELETE(d);
}


template <typename T>
void TestDeleteHelperArray()
{
    ImplTest * p;
    T * d;

    p = XR_NEW("test") ImplTest [2];
    XR_ASSERT_ALWAYS_EQ(p[0].blah, 100);
    XR_ASSERT_ALWAYS_EQ(p[1].blah, 100);
    d = p;
    XR_DELETE_ARRAY(d);

    p = XR_NEW_ALIGN("test", 16) ImplTest [2];
    XR_ASSERT_ALWAYS_EQ(p[0].blah, 100);
    XR_ASSERT_ALWAYS_EQ(p[1].blah, 100);
    d = p;
    XR_DELETE_ARRAY(d);

    p = XR_NEW_FLAGS("test", xr::Core::kMemNormal) ImplTest [2];
    XR_ASSERT_ALWAYS_EQ(p[0].blah, 100);
    XR_ASSERT_ALWAYS_EQ(p[1].blah, 100);
    d = p;
    XR_DELETE_ARRAY(d);

    p = XR_NEW_ALIGN_FLAGS("test", 16, xr::Core::kMemNormal) ImplTest [2];
    XR_ASSERT_ALWAYS_EQ(p[0].blah, 100);
    XR_ASSERT_ALWAYS_EQ(p[1].blah, 100);
    d = p;
    XR_DELETE_ARRAY(d);
}


// --------------------------------------------------------------------------------------  FUNCTION
/*!  If this test fails then the new / delete don't properly handle multiple
     inheritance, which should be managed by the compiler. this case is tricky
     because the deleted pointer isn't the pointer that was allocated.

     */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( multipleInheritence )
{
    TestDeleteHelper<ImplTest>();
    TestDeleteHelper<IClass1>();
    TestDeleteHelper<IClass2>();
    TestDeleteHelper<IClass3>();
    TestDeleteHelperArray<ImplTest>();

    // This would not be valid C++ syntax.
    // It works by chance in MSVC but it fails in GCC.
    // Note that if you passed the allocated array as an array
    // of base classes, the pointer math would not work so such
    // usage is not valid in any case.
    //TestDeleteHelperArray<IClass1>();
    //TestDeleteHelperArray<IClass2>();
    //TestDeleteHelperArray<IClass3>();

}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
