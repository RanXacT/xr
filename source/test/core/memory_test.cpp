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
#ifndef XR_CORE_MEMORY_H
#include "xr/core/memory.h"
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
XR_UNITTEST_GROUP_BEGIN( Memory )


// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Protect )
{
    const size_t size = 32 * 1024;
    void * p = ::xr::Core::Memory::PageAlloc(size);
    xr::Core::MemFill8(p, 0x23, size);

    ::xr::Core::Memory::PageProtect(p,size, ::xr::Core::Memory::kPageAccessRead);

    XR_ASSERT_ALWAYS_EQ(xr::Core::MemCheck8(p, 0x23, size), xr::Core::kSuccess);

    ::xr::Core::Memory::PageProtect(p,size, ::xr::Core::Memory::kPageAccessNone);
    // We don't actually verify exceptions are thrown in the test

    ::xr::Core::Memory::PageProtect(p,size, ::xr::Core::Memory::kPageAccessReadWrite);

    XR_ASSERT_ALWAYS_EQ(xr::Core::MemCheck8(p, 0x23, size), xr::Core::kSuccess);

    xr::Core::MemFill8(p, 0x55, size);
    XR_ASSERT_ALWAYS_EQ(xr::Core::MemCheck8(p, 0x55, size), xr::Core::kSuccess);

    ::xr::Core::Memory::PageFree(p, size);

    p = ::xr::Core::Memory::PageAlloc(size, ::xr::Core::Memory::kPageAccessNone);
    ::xr::Core::Memory::PageProtect(p,size, ::xr::Core::Memory::kPageAccessReadWrite);
    xr::Core::MemFill8(p, 0x23, size);
    XR_ASSERT_ALWAYS_EQ(xr::Core::MemCheck8(p, 0x23, size), xr::Core::kSuccess);

    ::xr::Core::Memory::PageFree(p, size);

    p = ::xr::Core::Memory::PageAlloc(size, ::xr::Core::Memory::kPageAccessRead);
    ::xr::Core::Memory::PageProtect(p,size, ::xr::Core::Memory::kPageAccessReadWrite);
    xr::Core::MemFill8(p, 0xCE, size);
    XR_ASSERT_ALWAYS_EQ(xr::Core::MemCheck8(p, 0xCE, size), xr::Core::kSuccess);
    ::xr::Core::Memory::PageFree(p, size);


    p = ::xr::Core::Memory::PageAlloc(size, ::xr::Core::Memory::kPageAccessReadWrite);
    ::xr::Core::Memory::PageProtect(p,size, ::xr::Core::Memory::kPageAccessReadWrite);
    xr::Core::MemFill8(p, 0x23, size);
    XR_ASSERT_ALWAYS_EQ(xr::Core::MemCheck8(p, 0x23, size), xr::Core::kSuccess);
    ::xr::Core::Memory::PageFree(p, size);

}



// --------------------------------------------------------------------------------------  FUNCTION
// Defensive tests ensure asserts are triggered, this fails on coverage builds
// because asserts are omitted. More thought could go into a better solution
// but there is no better solution yet.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( DefensiveTest1 )
{
    if(!::xr::Core::IsDebuggerConnected())
    {
        const size_t size = 32 * 1024;
#if !defined(XR_CODE_COVERAGE_BUILD)
        ::xr::Core::Test::ExpectCurrentTestFail(this);
#endif
#if defined(XR_COMPILER_MICROSOFT)
		// Supress (intended) unused variable warning.
#pragma warning(disable: 28193)
#endif
        void * p = ::xr::Core::Memory::PageAlloc(size, xr::Core::Memory::PageAccess(-1) );
        (void)(p);
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// Defensive tests ensure asserts are triggered, this fails on coverage builds
// because asserts are omitted. More thought could go into a better solution
// but there is no better solution yet.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( DefensiveTest2 )
{
    if(!::xr::Core::IsDebuggerConnected())
    {
        const size_t size = 32 * 1024;
#if !defined(XR_CODE_COVERAGE_BUILD)
        ::xr::Core::Test::ExpectCurrentTestFail(this);
#endif
        ::xr::Core::Memory::PageProtect(nullptr, size, xr::Core::Memory::PageAccess(-1) );
    }
}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
