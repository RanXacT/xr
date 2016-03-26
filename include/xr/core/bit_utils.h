// ######################################################################################### - FILE
/*! \file


\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_BIT_UTILS_H
#define XR_CORE_BIT_UTILS_H

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

#if XR_COMPILER_SUPPORTS_CONSTEXPR
template <typename T, size_t N>
constexpr size_t XR_ARRAY_SIZE(T (&) [N]) {
    return N;
}
#else
#define XR_ARRAY_SIZE(T) ( sizeof(T)/ sizeof(T[0]) )
#endif
namespace xr { namespace Core {

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief return pointer with bytes added to it (avoid pointer math) */
// --------------------------------------------------------------------------------------  FUNCTION
inline void * AddBytesToPtr(void* p, size_t bytes)
{
    return ((uint8_t*)p) + bytes;
}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief return the next address which meets the passed alignment.
    Will return the same address if it already meets that requirement. */
// --------------------------------------------------------------------------------------  FUNCTION
template <typename T>
inline T * AlignPtr(T* p, uintptr_t alignment)
{
    const uintptr_t k = alignment - 1;
    return (T*)( ((uintptr_t)p + k) & ~k);
}
// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief return the next integer which meets the passed alignment.
    Will return the same integer if it already meets that requirement. */
// --------------------------------------------------------------------------------------  FUNCTION
//@{
inline size_t AlignSize(size_t p, size_t alignment)
{
    const size_t k = alignment - 1;
    return ( ((size_t)p + k) & ~k);
}
inline uint8_t AlignUint8(uint8_t p, uint8_t alignment)
{
    const uint8_t k = alignment - 1;
    return ( ((uint8_t)p + k) & ~k);
}
inline uint16_t AlignUint16(uint16_t p, uint16_t alignment)
{
    const uint16_t k = alignment - 1;
    return ( ((uint16_t)p + k) & ~k);
}
inline uint32_t AlignUint32(uint32_t p, uint32_t alignment)
{
    const uint32_t k = alignment - 1;
    return ( ((uint32_t)p + k) & ~k);
}
inline uint64_t AlignUint64(uint64_t p, uint64_t alignment)
{
    const uint64_t k = alignment - 1;
    return ( ((uint64_t)p + k) & ~k);
}
//@}


// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Count number of sequential zeroes from high bit down */
// --------------------------------------------------------------------------------------  FUNCTION
//@{
uint8_t CountLeadingZeroes8(uint8_t x);
uint8_t CountLeadingZeroes16(uint16_t x);
uint8_t CountLeadingZeroes32(uint32_t x);
uint8_t CountLeadingZeroes64(uint64_t x);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Count number of sequential zeroes from low bit up */
// --------------------------------------------------------------------------------------  FUNCTION
//@{
uint8_t CountTrailingZeroes8(uint8_t x);
uint8_t CountTrailingZeroes16(uint16_t x);
uint8_t CountTrailingZeroes32(uint32_t x);
uint8_t CountTrailingZeroes64(uint64_t x);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Count number of sequential ones from high bit down */
// --------------------------------------------------------------------------------------  FUNCTION
//@{
uint8_t CountLeadingOnes8(uint8_t x);
uint8_t CountLeadingOnes16(uint16_t x);
uint8_t CountLeadingOnes32(uint32_t x);
uint8_t CountLeadingOnes64(uint64_t x);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Count number of sequential ones from low bit up */
// --------------------------------------------------------------------------------------  FUNCTION
//@{
uint8_t CountTrailingOnes8(uint8_t x);
uint8_t CountTrailingOnes16(uint16_t x);
uint8_t CountTrailingOnes32(uint32_t x);
uint8_t CountTrailingOnes64(uint64_t x);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief returns true if passed word contains a 1 byte nullptr */
// --------------------------------------------------------------------------------------  FUNCTION
//@{
bool HasNULL8(uint32_t x);
bool HasNULL8(uint64_t x);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief returns true if passed word contains a 2 byte nullptr */
// --------------------------------------------------------------------------------------  FUNCTION
//@{
bool HasNULL16(uint32_t x);
bool HasNULL16(uint64_t x);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief returns true if passed word contains a 2 byte match to passed uin8_t */
// --------------------------------------------------------------------------------------  FUNCTION
//@{
bool HasWord8(uint32_t x, uint8_t b);
bool HasWord8(uint64_t x, uint8_t b);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief returns true if passed word contains a 2 byte match to passed uint16_t
    \note matches must be 2 byte aligned.
*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
bool HasWord16(uint32_t x, uint16_t b);
bool HasWord16(uint64_t x, uint16_t b);
//@}


}}
#endif //#ifndef XR_CORE_BIT_UTILS_H
