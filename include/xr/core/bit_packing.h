// ######################################################################################### - FILE
/*! \file
    Provides simple utilities for bit level buffer packing. 

To be more specific one can insert into a buffer using an arbitrary number of bits. x bits, then 
y bits, then z bits, etc. Bit counts need not be multiples of 2 or anything. The reader should 
consume them in the order (and by the bit size) they were written or defined by the file spec, or whatever.

This also provides utilities to compress and access arrays based on the number of bits 
required to hold the largest value in the array. This compression is lossless, simple, and 
has O(1) access time. Note that the bit shifting isn't free, but reduction in cache usage may 
offset the additional computation cost (or the memory saving may be worth it anyway).

Obviously manual compressing to uint8 or uint16 would be faster, but may waste memory and you 
may not know the proper size at compile time. This is a more generic solution that adapts to 
the data at runtime.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_BIT_PACKING_H
#define XR_CORE_BIT_PACKING_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core{

// ***************************************************************************************** - TYPE
/// Dense Bit Stream, read linearly.
// ***************************************************************************************** - TYPE
class BitPackStreamReader{
public:
    const uint8_t  * start;
    const uint8_t  * next;
    const uint8_t  * end;
    uint32_t         bitHolder;
    uint_fast8_t     numBitsAvailable;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline BitPackStreamReader() :
        start(nullptr),
        next(nullptr),
        end(nullptr),
        bitHolder(0),
        numBitsAvailable(0)
    {

    }
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline void     Init(XR_IN_BYTES(sizeInBytes) const void *buffer, size_t sizeInBytes)
    {
        start = ( (const uint8_t  *) buffer);
        next  = (start);
        end   = (start + sizeInBytes);
        bitHolder        = 0;
        numBitsAvailable = 0;
    }
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline BitPackStreamReader(XR_IN_BYTES(sizeInBytes) const void *buffer, size_t sizeInBytes)
    {
        Init(buffer, sizeInBytes);
    }

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Returns the next N bites (where 1 <= N <=32)
    // ------------------------------------------------------------------------------------  MEMBER
    uint32_t ReadU32(uint_fast8_t numBits);

    // ------------------------------------------------------------------------------------  MEMBER
    /// Pad to the next byte alignment
    // ------------------------------------------------------------------------------------  MEMBER
    void Align();

private:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Unused assignment operator
    // ------------------------------------------------------------------------------------  MEMBER
    BitPackStreamReader & operator= (const BitPackStreamReader & other);
};


// --------------------------------------------------------------------------------------  FUNCTION
/// Dense Bit Stream, read linearly.
// --------------------------------------------------------------------------------------  FUNCTION
class BitPackStreamWriter{
public:
    uint8_t        * start;
    uint8_t        * next;
    const uint8_t  * end;
    uint32_t         bitHolder;
    uint_fast8_t     numBitsAvailable;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline BitPackStreamWriter() :
    start(nullptr),
        next(nullptr),
        end(nullptr),
        bitHolder(0),
        numBitsAvailable(0)
    {

    }
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline void     Init(XR_IN_BYTES(sizeInBytes) void *buffer, size_t sizeInBytes)
    {
        start = ( (uint8_t  *) buffer);
        next  = (start);
        end   = (start + sizeInBytes);
        bitHolder        = 0;
        numBitsAvailable =32;
    }
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline BitPackStreamWriter(XR_IN_BYTES(sizeInBytes) void *buffer, size_t sizeInBytes)
    {
        Init(buffer, sizeInBytes);
    }

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Writes the next N bites (where 1 <= N <=32)
    // ------------------------------------------------------------------------------------  MEMBER
    void WriteU32(uint32_t value, uint_fast8_t numBits);

    // ------------------------------------------------------------------------------------  MEMBER
    /// Pad to the next byte alignment
    // ------------------------------------------------------------------------------------  MEMBER
    void Align();


    // ------------------------------------------------------------------------------------  MEMBER
    /// This flushes the contents and returns the number of bytes used so far.
    // ------------------------------------------------------------------------------------  MEMBER
    size_t Close();
    // ------------------------------------------------------------------------------------  MEMBER
    /// returns the number of bytes used so far, be sure to "close" the
    /// stream before you commit it to disk though! as this does not flush
    /// the stream!
    // ------------------------------------------------------------------------------------  MEMBER
    size_t GetSizeInBytes();

private:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Unused assignment operator
    // ------------------------------------------------------------------------------------  MEMBER
    BitPackStreamWriter & operator= (const BitPackStreamWriter & other);
};


// --------------------------------------------------------------------------------------  FUNCTION
/// Lookup index in dense bit array.
// --------------------------------------------------------------------------------------  FUNCTION
uint32_t BitArrayLookupU32(const uint8_t * const base, size_t index, uint_fast8_t numBitsPerEntry);


// --------------------------------------------------------------------------------------  FUNCTION
/// Compact an array in place, returns the number of bytes used after compacting
/// (although it can be easily calculated).
/// \note The output array can be the same array (supports in place usage).
/// \note In general, the output array must be at least the same size as the input array.
/// \note if the output array is nullptr, the return value is still the output array size.
// --------------------------------------------------------------------------------------  FUNCTION
size_t BitArrayCompact32(const uint32_t * inArray, uint8_t * outArray, size_t count, uint_fast8_t &outNumBitsPerEntry);

}}
#endif //#ifndef XR_CORE_BIT_PACKING_H
