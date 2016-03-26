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
#ifndef XR_CORE_BIT_UTILS_H
#include "xr/core/bit_utils.h"
#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {
// ######################################################################################### - FILE
// BitPackStreamReader
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
/// \brief Returns the next N bites (where 0 < N <=32)
// --------------------------------------------------------------------------------------  FUNCTION
uint32_t BitPackStreamReader::ReadU32(uint_fast8_t numBits)
{
    uint32_t       retVal = bitHolder;
    if(numBits <= numBitsAvailable)
    {
        // Need to mask in case there are more bits
        const uint32_t mask   = ((1<<numBits)-1);
        // book keeping
        numBitsAvailable -= numBits;
        bitHolder >>= numBits;
        return retVal & mask;
    }

    const uint_fast8_t remainingBits = numBits - numBitsAvailable;
    const uint32_t remainingMask   = ((1<<remainingBits)-1);

    // Read next Word
    bitHolder = *((const uint32_t*)next);
    bitHolder = XR_LITTLE_ENDIAN_TO_NATIVE_UINT32_T(bitHolder);
    // put return value together.
    retVal = retVal | ((bitHolder&remainingMask) << numBitsAvailable);

    // book keeping.
    next += 4;
    numBitsAvailable = 32-remainingBits;
    bitHolder >>= remainingBits;

    return retVal;
}
// --------------------------------------------------------------------------------------  FUNCTION
/// Pad to the next byte alignment
// --------------------------------------------------------------------------------------  FUNCTION
void BitPackStreamReader::Align()
{
    numBitsAvailable = 0;
    bitHolder = 0;
}

// ######################################################################################### - FILE
// BitPackStreamWriter
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
/// \brief Returns the next N bites (where 0 < N <=32)
// --------------------------------------------------------------------------------------  FUNCTION
void BitPackStreamWriter::WriteU32(uint32_t value, uint_fast8_t numBitsToWrite)
{
    // This is benign to the algorithm, but probably the sign of
    // a coding error on the part of the caller.
    XR_ASSERT_DEBUG_EQ( value & (~(((1<<numBitsToWrite)-1))) , 0);
    // numBitsAvailable == 0 means that this stream was closed.
    XR_ASSERT_DEBUG_NE( numBitsAvailable , 0);

    while(numBitsToWrite > 0)
    {
        if(numBitsAvailable > 0)
        {
            // Take the smaller.
            uint_fast8_t  bitsCurrent = numBitsToWrite < numBitsAvailable ? numBitsToWrite : numBitsAvailable;

            // Mask off higher bits
            const uint32_t mask = ((1<<bitsCurrent)-1);

            // Shove the bits into the bit holder.
            bitHolder |= (value & mask) << (32-numBitsAvailable);

            // book keeping.
            numBitsAvailable -= bitsCurrent;

            // Remove what we've written from the passed value / count.
            numBitsToWrite -= bitsCurrent;
            value  >>= bitsCurrent;
        }

        // If we ran out of space, open up a new word
        if(numBitsAvailable == 0)
        {
            *(uint32_t*)next = XR_NATIVE_TO_LITTLE_ENDIAN_UINT32_T( bitHolder );
            numBitsAvailable = 32;
            next += 4;
            bitHolder = 0;
        }
    }

    return;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t BitPackStreamWriter::Close()
{
    // numBitsAvailable == 0 means that this stream was already closed.
    XR_ASSERT_DEBUG_NE( numBitsAvailable , 0);

    // Write the whole word, will waste a few bits, can revisit later
    // if desired, but this allows the reader to always read 32 bits
    // without needing to worry about crossing the end of the buffer.
    if(numBitsAvailable < 32)
    {
        *(uint32_t*)next = XR_NATIVE_TO_LITTLE_ENDIAN_UINT32_T( bitHolder );
        numBitsAvailable = 0;
        next += 4;
        bitHolder = 0;
    }

    return next - start;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t BitPackStreamWriter::GetSizeInBytes()
{
    // Currently write the whole word.
    return next - start + (numBitsAvailable == 0 ? 0 : 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void BitPackStreamWriter::Align()
{
    if(numBitsAvailable < 32)
    {
        *(uint32_t*)next = XR_NATIVE_TO_LITTLE_ENDIAN_UINT32_T( bitHolder );
        numBitsAvailable = 32;
        next += 4;
        bitHolder = 0;
    }
}

// ######################################################################################### - FILE
// Other functions
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
/// Lookup index in dense bit array.
// --------------------------------------------------------------------------------------  FUNCTION
uint32_t BitArrayLookupU32(const uint8_t * const base, size_t index, uint_fast8_t numBitsPerEntry)
{
    // Make sure we aren't about to truncate.
    // If this fires, then the following code will not work.
    // I.e. the data set is too large for the code on this platform
    // switch to x64!
    XR_ASSERT_ALWAYS_LT(numBitsPerEntry, (sizeof(size_t)*8) - 8);

    // This will truncate on 32 bit platforms past by up to 8 bits
    // (on sufficiently large data sets)
    const size_t   bitOffset  = index * numBitsPerEntry;
    const size_t   byteOffset = bitOffset / 8;

    // Get nearest Byte Address. This assumes unaligned Reads are OK.
    const size_t *location  = (const size_t *) (base + byteOffset);
    // Read as size_t and shift. The shift will also fail if numBitsPerEntry
    // is > 24 on 32 bit platforms.
    const size_t value      = *location >> (bitOffset & 7);
    // now mask off extra bits.
    return value & ((1<<numBitsPerEntry)-1);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t BitArrayCompact32( uint32_t * base, size_t count, uint_fast8_t &outNumBitsPerEntry )
{
    uint32_t max = 0;
    for(size_t i = 0; i < count; ++i)
    {
        uint32_t value = base[i];
        if(value > max)
        {
            max = value;
        }
    }

    uint_fast8_t numBitsPerEntry = 32 - CountLeadingZeroes32(max);

    BitPackStreamWriter w;
    w.Init(base, count * 4);
    for(size_t i = 0; i < count; ++i)
    {
        w.WriteU32(base[i], numBitsPerEntry);
    }

    outNumBitsPerEntry= numBitsPerEntry;
    return w.Close();
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t BitArrayCompact32( const uint32_t * inArray, uint8_t * outArray, size_t count, uint_fast8_t &outNumBitsPerEntry )
{
    uint32_t max = 0;
    for(size_t i = 0; i < count; ++i)
    {
        uint32_t value = inArray[i];
        if(value > max)
        {
            max = value;
        }
    }

    uint_fast8_t numBitsPerEntry = 32 - CountLeadingZeroes32(max);

    if(outArray != nullptr)
    {
        BitPackStreamWriter w;
        w.Init(outArray, count * 4);
        for(size_t i = 0; i < count; ++i)
        {
            w.WriteU32(inArray[i], numBitsPerEntry);
        }
        w.Close();
    }

    outNumBitsPerEntry= numBitsPerEntry;
    return ((numBitsPerEntry+7)/8) * count;
}

}}//namespace xr::Core

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
