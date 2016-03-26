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
#ifndef XR_CORE_ALLOCATORS_LINEAR_H
#include "xr/core/allocators/linear.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_MEM_UTILS_H
#include "xr/core/mem_utils.h"
#endif
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( LinearAllocator )

template <class A>
void SimpleTestHelper(A * a)
{
    // Linear allocator should not crash
    {
        const size_t allocSize = XR_PLATFORM_PTR_SIZE;
        void * p = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p, 0x54, allocSize);
    }

    XR_ASSERT_ALWAYS_EQ(a->Clear(), XR_PLATFORM_PTR_SIZE);

    {
        const size_t allocSize = XR_PLATFORM_PTR_SIZE;
        void * p = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p, 0x54, allocSize);
        a->Free(p, 0, XR_FILE_LINE);
    }

    XR_ASSERT_ALWAYS_EQ(a->Clear(), XR_PLATFORM_PTR_SIZE);

    {
        const size_t allocSize = XR_PLATFORM_PTR_SIZE;
        void * p0 = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p0, 0x54, allocSize);

        void * p1 = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p1, 0x54, allocSize);

        // linear allocator should allocate linearly.
        XR_ASSERT_ALWAYS_EQ(xr::Core::AddBytesToPtr(p0, XR_PLATFORM_PTR_SIZE), p1);
    }

    XR_ASSERT_ALWAYS_EQ(a->Clear(), XR_PLATFORM_PTR_SIZE*2);

    {
        const size_t allocSize = XR_PLATFORM_PTR_SIZE;
        void * p0 = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p0, 0x54, allocSize);

        uintptr_t verify = uintptr_t(p0);
        a->Free(p0, 0, XR_FILE_LINE);

        void * p1 = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p1, 0x54, allocSize);

        // linear allocator should not collect memory
        XR_ASSERT_ALWAYS_EQ( verify + XR_PLATFORM_PTR_SIZE, uintptr_t(p1) );
    }

    XR_ASSERT_ALWAYS_EQ(a->Clear(), XR_PLATFORM_PTR_SIZE*2);

    for(int i = 0; i < 1000; ++i)
    {
        const size_t allocSize = XR_PLATFORM_PTR_SIZE;
        void * p = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p, 0x5E, allocSize);
        a->Free(p, 0, XR_FILE_LINE);
    }

    XR_ASSERT_ALWAYS_EQ(a->GetHighwaterMark(), XR_PLATFORM_PTR_SIZE * 2);

    XR_ASSERT_ALWAYS_EQ(a->Clear(), XR_PLATFORM_PTR_SIZE * 1000);

    XR_ASSERT_ALWAYS_EQ(a->GetHighwaterMark(), XR_PLATFORM_PTR_SIZE * 1000);

    {
        const size_t allocSize = XR_PLATFORM_PTR_SIZE;
        void * p0 = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p0, 0x54, allocSize);

        uintptr_t verify = uintptr_t(p0);
        a->Free(p0, 0, XR_FILE_LINE);

        void * p1 = a->Alloc(allocSize, allocSize, xr::Core::kMemNormal, "No Description", XR_FILE_LINE);
        xr::Core::MemFill8(p1, 0x54, allocSize);

        // linear allocator should not collect memory
        XR_ASSERT_ALWAYS_EQ( verify + XR_PLATFORM_PTR_SIZE, uintptr_t(p1) );
    }

    XR_ASSERT_ALWAYS_EQ(a->Clear(), XR_PLATFORM_PTR_SIZE*2);

    XR_ASSERT_ALWAYS_EQ(a->GetHighwaterMark(), XR_PLATFORM_PTR_SIZE * 1000);

    a->Clear();
}
// --------------------------------------------------------------------------------------  FUNCTION
/*!

     */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( simpleTest )
{
    const size_t kTestAlloc = 1024;
    const size_t kTestCheck = 24;
    const size_t kTestUse   = kTestAlloc - kTestCheck;
    void * buffer = XR_ALLOC(kTestAlloc, "test");

    xr::Core::MemFill8(buffer, 0xFE, kTestAlloc);

    xr::Core::LinearAllocator la("Test", buffer, kTestUse, 128 );

    SimpleTestHelper(&la);

    XR_ASSERT_ALWAYS_EQ(xr::Core::MemCheck8( xr::Core::AddBytesToPtr(buffer, kTestUse), 0xFE, kTestCheck ), xr::Core::kSuccess);

    XR_FREE(buffer);
}

XR_UNITTEST_TEST_FUNC( comTest )
{

    xr::Core::LinearAllocatorParameters params;
    params.mBucketAlignment     = XR_PLATFORM_PTR_SIZE;
    params.mMainBucketSize      = 1024;
    params.mOverflowBucketSize  = 1024;
    params.mMetadataAllocator   = nullptr;
    params.mBucketAllocator     = nullptr;
    params.mName                = "Tester";

    xr::Core::ILinearAllocator *la = xr::Core::LinearAllocatorCreate(params);

    SimpleTestHelper(la);

    XR_DELETE(la);

}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
