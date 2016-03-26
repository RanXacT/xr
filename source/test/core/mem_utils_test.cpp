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
#ifndef XR_CORE_MEM_UTILS_H
#include "xr/core/mem_utils.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#include <stdio.h>
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( MemUtils )

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( MemUtilsTest )
{
    using namespace xr::Core;
    {
        uint8_t test;
        MemClear8(&test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck8( &test, 0, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck8( &test, 1, 1), kFailure);
        MemFill8(&test, 0xCA, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck8( &test, 0xCA, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck8( &test, 0, 1), kFailure);

        uint8_t test2;
        MemCopy8(&test2, &test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck8( &test2, 0xCA, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCompare8( &test2, &test, 1), 0);

        uint8_t test3 = 0xFF;
        XR_ASSERT_ALWAYS_NE(MemCompare8( &test2, &test3, 1), 0);
    }

    {
        uint16_t test;
        MemClear16(&test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck16( &test, 0, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck16( &test, 1, 1), kFailure);
        MemFill16(&test, 0xCA, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck16( &test, 0xCA, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck16( &test, 0, 1), kFailure);

        uint16_t test2;
        MemCopy16(&test2, &test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck16( &test2, 0xCA, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCompare16( &test2, &test, 1), 0);

        uint16_t test3 = 0xFF;
        XR_ASSERT_ALWAYS_NE(MemCompare16( &test2, &test3, 1), 0);
    }

    {
        uint32_t test;
        MemClear32(&test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck32( &test, 0, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck32( &test, 1, 1), kFailure);
        MemFill32(&test, 0xCA, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck32( &test, 0xCA, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck32( &test, 0, 1), kFailure);

        uint32_t test2;
        MemCopy32(&test2, &test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck32( &test2, 0xCA, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCompare32( &test2, &test, 1), 0);

        uint32_t test3 = 0xFF;
        XR_ASSERT_ALWAYS_NE(MemCompare32( &test2, &test3, 1), 0);
    }

    {
        uint64_t test;
        MemClear64(&test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck64( &test, 0, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck64( &test, 1, 1), kFailure);
        MemFill64(&test, 0xCA, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck64( &test, 0xCA, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck64( &test, 0, 1), kFailure);

        uint64_t test2;
        MemCopy64(&test2, &test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck64( &test2, 0xCA, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCompare64( &test2, &test, 1), 0);

        uint64_t test3 = 0xFF;
        XR_ASSERT_ALWAYS_NE(MemCompare64( &test2, &test3, 1), 0);
    }


    {
        struct test128_t{
            uint64_t test[2];
        };

        test128_t zero = { {XR_UINT64_C(0), XR_UINT64_C(0)} };
        test128_t seed = { {XR_UINT64_C(0x0123456701234567), XR_UINT64_C(0x8765432187654321)} };
        test128_t test = { { XR_UINT64_C(0x55555555), XR_UINT64_C(0x77777777) } };

        MemClear128(&test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck128( &test, &zero, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck128( &test, &seed, 1), kFailure);
        MemFill128(&test, &seed, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck128( &test, &seed, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCheck128( &test, &zero, 1), kFailure);

        test128_t test2;
        MemCopy128(&test2, &test, 1);
        XR_ASSERT_ALWAYS_EQ(MemCheck128( &test2, &seed, 1), kSuccess);
        XR_ASSERT_ALWAYS_EQ(MemCompare128( &test2, &test, 1), 0);

        test128_t other = { {XR_UINT64_C(0x192380), XR_UINT64_C(0x192380654681)} };
        XR_ASSERT_ALWAYS_NE(MemCompare128( &other, &test, 1), 0);
    }
}
XR_UNITTEST_TEST_FUNC( MemUtilsTestArray8 )
{
    using namespace xr::Core;
    const size_t kTestArraySize = 101;
    uint16_t test[kTestArraySize];
    MemClear16(test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck16( test, 0, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck16( test, 1, kTestArraySize), kFailure);
    MemFill16(test, 0xCA, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck16( test, 0xCA, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck16( test, 0, kTestArraySize), kFailure);

    uint16_t test2[kTestArraySize];
    MemCopy16(test2, test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck16( test2, 0xCA, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCompare16( test2, test, kTestArraySize), 0);

}
XR_UNITTEST_TEST_FUNC( MemUtilsTestArray16 )
{
    using namespace xr::Core;
    const size_t kTestArraySize = 101;

    uint8_t test[kTestArraySize];
    MemClear8(test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck8( test, 0, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck8( test, 1, kTestArraySize), kFailure);
    MemFill8(test, 0xCA, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck8( test, 0xCA, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck8( test, 0, kTestArraySize), kFailure);

    uint8_t test2[kTestArraySize];
    MemCopy8(test2, test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck8( test2, 0xCA, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCompare8( test2, test, kTestArraySize), 0);

}
XR_UNITTEST_TEST_FUNC( MemUtilsTestArray32 )
{
    using namespace xr::Core;
    const size_t kTestArraySize = 101;
    uint32_t test[kTestArraySize];
    MemClear32(test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck32( test, 0, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck32( test, 1, kTestArraySize), kFailure);
    MemFill32(test, 0xCA, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck32( test, 0xCA, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck32( test, 0, kTestArraySize), kFailure);

    uint32_t test2[kTestArraySize];
    MemCopy32(test2, test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck32( test2, 0xCA, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCompare32( test2, test, kTestArraySize), 0);

}
XR_UNITTEST_TEST_FUNC( MemUtilsTestArray64 )
{
    using namespace xr::Core;
    const size_t kTestArraySize = 101;
    uint64_t test[kTestArraySize];
    MemClear64(test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck64( test, 0, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck64( test, 1, kTestArraySize), kFailure);
    MemFill64(test, 0xCA, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck64( test, 0xCA, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck64( test, 0, kTestArraySize), kFailure);

    uint64_t test2[kTestArraySize];
    MemCopy64(test2, test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck64( test2, 0xCA, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCompare64( test2, test, kTestArraySize), 0);
}
XR_UNITTEST_TEST_FUNC( MemUtilsTestArray128 )
{
    using namespace xr::Core;
    const size_t kTestArraySize = 101;

    struct test128_t{
        uint64_t test[2];
    };

    test128_t zero = { {XR_UINT64_C(0), XR_UINT64_C(0)} };
    test128_t seed = { {XR_UINT64_C(0x0123456701234567), XR_UINT64_C(0x8765432187654321)} };

    test128_t test[kTestArraySize];
    MemClear128(test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck128( test, &zero, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck128( test, &seed, kTestArraySize), kFailure);
    MemFill128(test, &seed, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck128( test, &seed, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCheck128( test, &zero, kTestArraySize), kFailure);

    test128_t test2[kTestArraySize];
    MemCopy128(test2, test, kTestArraySize);
    XR_ASSERT_ALWAYS_EQ(MemCheck128( test2, &seed, kTestArraySize), kSuccess);
    XR_ASSERT_ALWAYS_EQ(MemCompare128( test2, test, kTestArraySize), 0);
}
XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
