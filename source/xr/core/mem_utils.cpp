// ######################################################################################### - FILE
/*!
Implementations for most functions are naive, can optimize later.

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

#if !defined(XR_COMPILER_MICROSOFT) && !defined(XR_COMPILER_GCC)
#include <string.h>
#endif
// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

#if !defined(XR_COMPILER_MICROSOFT) && !defined(XR_COMPILER_GCC)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemClear8(  void * __restrict p, size_t count8)
{
    memset(p, 0, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemClear16( uint16_t * __restrict p, size_t count16)
{
    uint16_t *start = (uint16_t *) p;
    for(size_t i = 0; i < count16; ++i)
    {
        start[i] = 0;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemClear32( uint32_t * __restrict p, size_t count32)
{
    uint32_t *start = (uint32_t *) p;
    for(size_t i = 0; i < count32; ++i)
    {
        start[i] = 0;
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemClear64( uint64_t * __restrict p, size_t count64)
{
    uint64_t *start = (uint64_t *) p;
    for(size_t i = 0; i < count64; ++i)
    {
        start[i] = 0;
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemClear128( void * __restrict p, size_t count128)
{
    uint64_t *start = (uint64_t *) p;
    for(size_t i = 0; i < count128*2; i+=2)
    {
        start[i  ] = 0;
        start[i+1] = 0;
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemFill8(  XR_OUT_BYTES(count8 ) void     * __restrict p,  uint8_t seed, size_t count8 )
{
    memset(p, seed, count8);
}
#endif
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemFill16( XR_OUT_COUNT(count16) uint16_t * __restrict p, uint16_t seed, size_t count16)
{
    uint16_t *start = (uint16_t *) p;
    for(size_t i = 0; i < count16; ++i)
    {
        start[i] = seed;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemFill32( XR_OUT_COUNT(count32) uint32_t * __restrict p, uint32_t seed, size_t count32)
{
    uint32_t *start = (uint32_t *) p;
    for(size_t i = 0; i < count32; ++i)
    {
        start[i] = seed;
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemFill64( XR_OUT_COUNT(count64) uint64_t * __restrict p, uint64_t seed, size_t count64)
{
    uint64_t *start = (uint64_t *) p;
    for(size_t i = 0; i < count64; ++i)
    {
        start[i] = seed;
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemFill128(void * __restrict p, XR_IN_BYTES(16) const void    * __restrict pseed, size_t count128)
{
    uint64_t *__restrict start = (uint64_t *) p;
    uint64_t *__restrict seed64  = (uint64_t *) pseed;
    for(size_t i = 0; i < count128; ++i)
    {
        *start++ = seed64[0];
        *start++ = seed64[1];
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult MemCheck8(  XR_IN_BYTES(count8 ) const void     * __restrict p,  uint8_t seed, size_t count8 )
{
    uint8_t *start = (uint8_t *) p;
    for(size_t i = 0; i < count8; ++i)
    {
        if(start[i] != seed)
        {
            return kFailure;
        }
    }
    return kSuccess;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult MemCheck16( XR_IN_COUNT(count16) const uint16_t * __restrict p, uint16_t seed, size_t count16)
{
    uint16_t *start = (uint16_t *) p;
    for(size_t i = 0; i < count16; ++i)
    {
        if(start[i] != seed)
        {
            return kFailure;
        }
    }
    return kSuccess;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult MemCheck32( XR_IN_COUNT(count32) const uint32_t * __restrict p, uint32_t seed, size_t count32)
{
    uint32_t *start = (uint32_t *) p;
    for(size_t i = 0; i < count32; ++i)
    {
        if(start[i] != seed)
        {
            return kFailure;
        }
    }
    return kSuccess;
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult MemCheck64( XR_IN_COUNT(count64) const uint64_t * __restrict p, uint64_t seed, size_t count64)
{
    uint64_t *start = (uint64_t *) p;
    for(size_t i = 0; i < count64; ++i)
    {
        if(start[i] != seed)
        {
            return kFailure;
        }
    }
    return kSuccess;
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult MemCheck128(const void * __restrict p, XR_IN_BYTES(16) const void    * __restrict pseed, size_t count128)
{
    uint64_t *__restrict start = (uint64_t *) p;
    uint64_t *__restrict seed64  = (uint64_t *) pseed;
    for(size_t i = 0; i < count128; ++i)
    {
        if (*start++ != seed64[0] || *start++ != seed64[1])
        {
            return kFailure;
        }
    }
    return kSuccess;
}

#if !defined(XR_COMPILER_MICROSOFT) && !defined(XR_COMPILER_GCC)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemCopy8(  XR_OUT_BYTES(count8 ) void     * __restrict pDest, XR_IN_BYTES(count8 ) const void     * __restrict pSrc, size_t count8 )
{
    ::memcpy(pDest, pSrc, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemCopy16( XR_OUT_COUNT(count16) uint16_t * __restrict pDest, XR_IN_COUNT(count16) const uint16_t * __restrict pSrc, size_t count16)
{
    ::memcpy(pDest, pSrc, count16 * 2);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemCopy32( XR_OUT_COUNT(count32) uint32_t * __restrict pDest, XR_IN_COUNT(count32) const uint32_t * __restrict pSrc, size_t count32)
{
    ::memcpy(pDest, pSrc, count32 * 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemCopy64( XR_OUT_COUNT(count64) uint64_t * __restrict pDest, XR_IN_COUNT(count64) const uint64_t * __restrict pSrc, size_t count64)
{
    ::memcpy(pDest, pSrc, count64 * 8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MemCopy128(void * __restrict pDest, const void * __restrict pSrc, size_t count128)
{
    ::memcpy(pDest, pSrc, count128 * 16);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int MemCompare8(   XR_IN_BYTES(count8 ) const void     * __restrict p1,  XR_IN_BYTES(count8 ) const void     * __restrict p2, size_t count8 )
{
    return ::memcmp(p1, p2, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int MemCompare16(  XR_IN_COUNT(count16) const uint16_t * __restrict p1,  XR_IN_COUNT(count16) const uint16_t * __restrict p2, size_t count16)
{
    return ::memcmp(p1, p2, count16 * 2);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int MemCompare32(  XR_IN_COUNT(count32) const uint32_t * __restrict p1,  XR_IN_COUNT(count32) const uint32_t * __restrict p2, size_t count32)
{
    return ::memcmp(p1, p2, count32 * 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int MemCompare64(  XR_IN_COUNT(count64) const uint64_t * __restrict p1,  XR_IN_COUNT(count64) const uint64_t * __restrict p2, size_t count64)
{
    return ::memcmp(p1, p2, count64 * 8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int MemCompare128(const void * __restrict p1, const void * __restrict p2, size_t count128)
{
    return ::memcmp(p1, p2, count128 * 16);
}
#endif
}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
