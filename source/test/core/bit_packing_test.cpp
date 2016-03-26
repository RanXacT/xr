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
#ifndef XR_CORE_BIT_PACKING_H
#include "xr/core/bit_packing.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_BIT_UTILS_H
#include "xr/core/bit_utils.h"
#endif
#ifndef XR_CORE_MEM_UTILS_H
#include "xr/core/mem_utils.h"
#endif

#ifndef XR_CORE_RANDOM_MERSENNE_H
#include "xr/core/random/mersenne.h"
#endif

// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( BitPacking )

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( StreamTestSimple )
{
    using namespace xr::Core;

    const size_t testSizeBytes = 256;
    uint8_t test[testSizeBytes];
    xr::Core::MemClear8(test, XR_ARRAY_SIZE(test));

    {
        BitPackStreamWriter writer;
        writer.Init(test, testSizeBytes);
        writer.WriteU32(0x1F, 5);
        writer.Close();
    }
    {
        BitPackStreamReader reader;
        reader.Init(test, testSizeBytes);
        uint32_t test0 = reader.ReadU32(5);
        XR_ASSERT_ALWAYS_EQ(test0, 0x1F);
    }

    {
        BitPackStreamWriter writer;
        writer.Init(test, testSizeBytes);
        writer.WriteU32(0x00, 5);
        writer.WriteU32(0x1F, 5);
        writer.WriteU32(0x00, 5);
        writer.WriteU32(0x1F, 5);
        writer.WriteU32(0x00, 5);
        writer.WriteU32(0x1F, 5);
        writer.WriteU32(0x00, 5);
        writer.WriteU32(0x1F, 5);
        writer.Close();
    }
    {
        BitPackStreamReader reader;
        reader.Init(test, testSizeBytes);
        uint32_t test0 = reader.ReadU32(5);
        uint32_t test1 = reader.ReadU32(5);
        uint32_t test2 = reader.ReadU32(5);
        uint32_t test3 = reader.ReadU32(5);
        uint32_t test4 = reader.ReadU32(5);
        uint32_t test5 = reader.ReadU32(5);
        uint32_t test6 = reader.ReadU32(5);
        uint32_t test7 = reader.ReadU32(5);
        XR_ASSERT_ALWAYS_EQ(test0, 0x00);
        XR_ASSERT_ALWAYS_EQ(test1, 0x1F);
        XR_ASSERT_ALWAYS_EQ(test2, 0x00);
        XR_ASSERT_ALWAYS_EQ(test3, 0x1F);
        XR_ASSERT_ALWAYS_EQ(test4, 0x00);
        XR_ASSERT_ALWAYS_EQ(test5, 0x1F);
        XR_ASSERT_ALWAYS_EQ(test6, 0x00);
        XR_ASSERT_ALWAYS_EQ(test7, 0x1F);
    }


    {
        BitPackStreamWriter writer;
        writer.Init(test, testSizeBytes);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.Close();
    }
    {
        BitPackStreamReader reader;
        reader.Init(test, testSizeBytes);
        uint32_t test0 = reader.ReadU32(11);
        uint32_t test1 = reader.ReadU32(11);
        uint32_t test2 = reader.ReadU32(11);
        uint32_t test3 = reader.ReadU32(11);
        uint32_t test4 = reader.ReadU32(11);
        uint32_t test5 = reader.ReadU32(11);
        uint32_t test6 = reader.ReadU32(11);
        uint32_t test7 = reader.ReadU32(11);
        XR_ASSERT_ALWAYS_EQ(test0, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test1, 0x000);
        XR_ASSERT_ALWAYS_EQ(test2, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test3, 0x000);
        XR_ASSERT_ALWAYS_EQ(test4, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test5, 0x000);
        XR_ASSERT_ALWAYS_EQ(test6, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test7, 0x000);
    }

}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( StreamTestPermute )
{
    using namespace xr::Core;

    const size_t testSizeBytes = (32 * 64) / 8;
    uint8_t      test[testSizeBytes];
    uint32_t     fill[testSizeBytes / 4];

    xr::Core::MemClear8(test, XR_ARRAY_SIZE(test));
    xr::Core::MemClear8(fill, XR_ARRAY_SIZE(fill));

    MersenneTwisterPRNG random;

    for(uint_fast8_t numBits = 1; numBits < 32; ++numBits)
    {
        for(size_t numWords = 1; numWords < 64; ++numWords)
        {
            random.Seed((uint32_t)numWords);

            for(size_t i = 0; i < numWords; ++i)
            {
                fill[i] = random.Next();
            }

            {
                BitPackStreamWriter writer;
                writer.Init(test, testSizeBytes);
                for(size_t i = 0; i < numWords; ++i)
                {
                    writer.WriteU32(fill[i] & ((1<<numBits)-1), numBits);
                }
                writer.Close();
            }

            {
                BitPackStreamReader reader;
                reader.Init(test, testSizeBytes);
                for(size_t i = 0; i < numWords; ++i)
                {
                    XR_ASSERT_ALWAYS_EQ(reader.ReadU32(numBits), fill[i] & ((1<<numBits)-1));
                }
            }
        }
    }

}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( ArrayTestSimple )
{
    using namespace xr::Core;

    const size_t testSizeBytes = 256;
    uint8_t test[testSizeBytes];
    xr::Core::MemClear8(test, XR_ARRAY_SIZE(test));

    {
        BitPackStreamWriter writer;
        writer.Init(test, testSizeBytes);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.Close();
    }



    {
        uint32_t test0 = BitArrayLookupU32(test, 0, 11);
        uint32_t test1 = BitArrayLookupU32(test, 1, 11);
        uint32_t test2 = BitArrayLookupU32(test, 2, 11);
        uint32_t test3 = BitArrayLookupU32(test, 3, 11);
        uint32_t test4 = BitArrayLookupU32(test, 4, 11);
        uint32_t test5 = BitArrayLookupU32(test, 5, 11);
        uint32_t test6 = BitArrayLookupU32(test, 6, 11);
        uint32_t test7 = BitArrayLookupU32(test, 7, 11);
        XR_ASSERT_ALWAYS_EQ(test0, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test1, 0x000);
        XR_ASSERT_ALWAYS_EQ(test2, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test3, 0x000);
        XR_ASSERT_ALWAYS_EQ(test4, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test5, 0x000);
        XR_ASSERT_ALWAYS_EQ(test6, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test7, 0x000);
    }

    {
        BitPackStreamWriter writer;
        writer.Init(test, testSizeBytes);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.WriteU32(0x000, 11);
        writer.WriteU32(0x7FF, 11);
        writer.Close();
    }



    {
        uint32_t test0 = BitArrayLookupU32(test, 0, 11);
        uint32_t test1 = BitArrayLookupU32(test, 1, 11);
        uint32_t test2 = BitArrayLookupU32(test, 2, 11);
        uint32_t test3 = BitArrayLookupU32(test, 3, 11);
        uint32_t test4 = BitArrayLookupU32(test, 4, 11);
        uint32_t test5 = BitArrayLookupU32(test, 5, 11);
        uint32_t test6 = BitArrayLookupU32(test, 6, 11);
        uint32_t test7 = BitArrayLookupU32(test, 7, 11);
        XR_ASSERT_ALWAYS_EQ(test0, 0x000);
        XR_ASSERT_ALWAYS_EQ(test1, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test2, 0x000);
        XR_ASSERT_ALWAYS_EQ(test3, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test4, 0x000);
        XR_ASSERT_ALWAYS_EQ(test5, 0x7FF);
        XR_ASSERT_ALWAYS_EQ(test6, 0x000);
        XR_ASSERT_ALWAYS_EQ(test7, 0x7FF);
    }

}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( ArrayTestPermute )
{
    using namespace xr::Core;

    const size_t testSizeBytes = (32 * 64) / 8;
    uint8_t      test[testSizeBytes];
    uint32_t     fill[testSizeBytes / 4];
    xr::Core::MemClear8(test, XR_ARRAY_SIZE(test));
    xr::Core::MemClear8(fill, XR_ARRAY_SIZE(fill));
    MersenneTwisterPRNG random;

    for(uint_fast8_t numBits = 1; numBits < 24; ++numBits)
    {
        for(size_t numWords = 1; numWords < 64; ++numWords)
        {
            random.Seed((uint32_t)numWords);

            for(size_t i = 0; i < numWords; ++i)
            {
                fill[i] = random.Next();
            }

            {
                BitPackStreamWriter writer;
                writer.Init(test, testSizeBytes);
                for(size_t i = 0; i < numWords; ++i)
                {
                    writer.WriteU32(fill[i] & ((1<<numBits)-1), numBits);
                }
                writer.Close();
            }

            for(size_t i = 0; i < numWords; ++i)
            {
                XR_ASSERT_ALWAYS_EQ(BitArrayLookupU32(test, i, numBits), fill[i] & ((1<<numBits)-1));
            }
        }
    }

}


// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( ArrayCompactTest )
{
    using namespace xr::Core;

    const size_t testSizeBytes = (32 * 64) / 8;
    uint8_t      test[testSizeBytes];
    uint32_t     fill[testSizeBytes / 4];

    for(uint_fast8_t numBits = 1; numBits < 24; ++numBits)
    {
        const uint32_t mask = ((1<<numBits)-1);

        for(size_t numWords = 1; numWords < 64; ++numWords)
        {
            for(size_t i = 0; i < numWords; ++i)
            {
                fill[i] = 0xFFFFFFFF & mask;
            }

            uint_fast8_t numBitsDetected = 0;
            BitArrayCompact32(fill, test, numWords, numBitsDetected);
            XR_ASSERT_ALWAYS_EQ(numBitsDetected, numBits);

            for(size_t i = 0; i < numWords; ++i)
            {
                auto f = fill[i] & ((1 << numBits) - 1);
                auto b = BitArrayLookupU32((const uint8_t*)test, i, numBits);
                XR_ASSERT_ALWAYS_EQ(b, f);
            }
        }
    }

}
XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
