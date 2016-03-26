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
#ifndef XR_CORE_BIT_UTILS_H
#include "xr/core/bit_utils.h"
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
XR_UNITTEST_GROUP_BEGIN( BitUtils )

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( bitsTest )
{
    using namespace xr::Core;

    XR_ASSERT_ALWAYS_TRUE(  HasNULL8( XR_UINT32_C(0x00000000)) );
    XR_ASSERT_ALWAYS_TRUE(  HasNULL16(XR_UINT32_C(0x00000000)) );
    XR_ASSERT_ALWAYS_TRUE(  HasNULL8( XR_UINT64_C(0x0000000000000000)) );
    XR_ASSERT_ALWAYS_TRUE(  HasNULL16(XR_UINT64_C(0x0000000000000000)) );

    uint32_t testVal32 = 1;
    for(int i = 0; i < 32; ++i)
    {
        XR_ASSERT_ALWAYS_TRUE(  HasNULL8( testVal32) );
        XR_ASSERT_ALWAYS_TRUE(  HasNULL16(testVal32) );
        testVal32 <<=1;
    }

    uint64_t testVal64 = 1;
    for(int i = 0; i < 64; ++i)
    {
        XR_ASSERT_ALWAYS_TRUE(  HasNULL8( testVal64) );
        XR_ASSERT_ALWAYS_TRUE(  HasNULL16(testVal64) );
        testVal64 <<=1;
    }


    XR_ASSERT_ALWAYS_FALSE(  HasNULL8( XR_UINT32_C(~0x00000000)) );
    XR_ASSERT_ALWAYS_FALSE(  HasNULL16(XR_UINT32_C(~0x00000000)) );
    XR_ASSERT_ALWAYS_FALSE(  HasNULL8( XR_UINT64_C(~0x0000000000000000)) );
    XR_ASSERT_ALWAYS_FALSE(  HasNULL16(XR_UINT64_C(~0x0000000000000000)) );

    testVal32 = 0xFFFFFFFE;
    for(int i = 0; i < 32; ++i)
    {
        XR_ASSERT_ALWAYS_FALSE(  HasNULL8( testVal32) );
        XR_ASSERT_ALWAYS_FALSE(  HasNULL16(testVal32) );
        testVal32 <<=1;
        testVal32 |= XR_UINT32_C(1);
    }

    testVal64 = XR_UINT64_C(0xFFFFFFFFFFFFFFFE);
    for(int i = 0; i < 64; ++i)
    {
        XR_ASSERT_ALWAYS_FALSE(  HasNULL8( testVal64) );
        XR_ASSERT_ALWAYS_FALSE(  HasNULL16(testVal64) );
        testVal64 <<=1;
        testVal64 |= XR_UINT64_C(1);
    }

    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes8( XR_UINT8_MAX ) , uint8_t(0));
    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes8( uint8_t(0x00)) , uint8_t(8));

    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes16( XR_UINT16_MAX) , 0);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes16( uint16_t(0x00000000)) , 16);

    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes32( XR_UINT32_C(~0x00000000)) , 0);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes32( XR_UINT32_C(0x00000000)) , 32);

    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes64( XR_UINT64_C(~0x00000000)) , 0);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes64( XR_UINT64_C(0x00000000)) , 64);

    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes8( uint8_t(0x08)) , uint8_t(4));
    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes16( uint16_t(0x0080)) , 8);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes32( XR_UINT32_C(0x00008000)) , 16);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes64( XR_UINT64_C(0x0000800000000000)) , 16);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingZeroes64( XR_UINT64_C(0x00008000)) , 48);

    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes8( uint8_t(~0x00000000)) , uint8_t(8));
    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes8( uint8_t(0x00000000)) ,  uint8_t(0));

    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes16( XR_UINT16_MAX) ,16);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes16( uint16_t(0x00000000)) ,  0);

    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes32( XR_UINT32_C(~0x00000000)) ,32);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes32( XR_UINT32_C(0x00000000)) ,  0);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes64( XR_UINT64_C(~0x00000000)) ,64);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes64( XR_UINT64_C(0x00000000)) ,  0);

    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes8( uint8_t(~0x08)) , uint8_t(4));
    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes16( uint16_t(~0x0080)) , 8);

    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes32( XR_UINT32_C(~0x00008000)) , 16);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes64( XR_UINT64_C(~0x00008000)) , 48);
    XR_ASSERT_ALWAYS_EQ(  CountLeadingOnes64( XR_UINT64_C(~0x0000800000000000)) , 16);


    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes8( uint8_t(~0x00000000)) ,0);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes8( uint8_t(0x00000000)) , 8);

    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes16( XR_UINT16_MAX) ,0);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes16( uint16_t(0x00000000)) , 16);

    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes32( XR_UINT32_C(~0x00000000)) , 0);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes32( XR_UINT32_C(0x00000000)) , 32);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes64( XR_UINT64_C(~0x00000000)) , 0);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes64( XR_UINT64_C(0x00000000)) , 64);

    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes8( uint8_t(0x08)) , 3);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes16( uint16_t(0x0080)) , 7);

    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes32( XR_UINT32_C(0x00008000)) , 15);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes64( XR_UINT64_C(0x0000800000000000)) , 47);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingZeroes64( XR_UINT64_C(0x00008000)) , 15);

    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes8( uint8_t(~0x00000000)) ,8);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes8( uint8_t(0x00000000)) , 0);

    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes16( XR_UINT16_MAX) ,16);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes16( uint16_t(0x00000000)) , 0);

    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes32( XR_UINT32_C(~0x00000000)) ,32);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes32( XR_UINT32_C(0x00000000)) ,  0);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes64( XR_UINT64_C(~0x00000000)) ,64);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes64( XR_UINT64_C(0x00000000)) ,  0);

    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes8( uint8_t(~0x08)) , 3);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes16( uint16_t(~0x0080)) , 7);


    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes32( XR_UINT32_C(~0x00008000)) , 15);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes64( XR_UINT64_C(~0x00008000)) , 15);
    XR_ASSERT_ALWAYS_EQ(  CountTrailingOnes64( XR_UINT64_C(~0x0000800000000000)) , 47);


    const size_t kTestArraySize = 113;
    {
        char test[kTestArraySize];
        XR_ASSERT_ALWAYS_EQ(kTestArraySize, XR_ARRAY_SIZE(test));
        XR_UNUSED(test);
#if XR_COMPILER_SUPPORTS_CONSTEXPR
        static_assert(XR_ARRAY_SIZE(test) == kTestArraySize, "ArraySizeCheck");
#endif
    }
    {
        uint64_t test[kTestArraySize];
        XR_ASSERT_ALWAYS_EQ(kTestArraySize, XR_ARRAY_SIZE(test));
        XR_UNUSED(test);
#if XR_COMPILER_SUPPORTS_CONSTEXPR
        static_assert(XR_ARRAY_SIZE(test) == kTestArraySize, "ArraySizeCheck");
#endif
    }
    {
        struct blah{
            uint32_t xxx;
        }test[kTestArraySize];
        XR_ASSERT_ALWAYS_EQ(kTestArraySize, XR_ARRAY_SIZE(test));
        XR_UNUSED(test);
    }

}
XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
