// ######################################################################################### - FILE
/*! \file


\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_MEM_UTILS_H
#define XR_CORE_MEM_UTILS_H

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
#if defined(XR_COMPILER_MICROSOFT)
extern "C" {
#if _MSC_VER > 1700
    _Check_return_ int     __cdecl memcmp(_In_reads_bytes_(_Size) const void * _Buf1, _In_reads_bytes_(_Size) const void * _Buf2, _In_ size_t _Size);

    _Post_equal_to_(_Dst)
    _At_buffer_((unsigned char*)_Dst, _Iter_, _Size, _Post_satisfies_(((unsigned char*)_Dst)[_Iter_] == ((unsigned char*)_Src)[_Iter_]))
    void *  __cdecl memcpy(_Out_writes_bytes_all_(_Size) void * _Dst, _In_reads_bytes_(_Size) const void * _Src, _In_ size_t _Size);

    _Post_equal_to_(_Dst)
    _At_buffer_((unsigned char*)_Dst, _Iter_, _Size, _Post_satisfies_(((unsigned char*)_Dst)[_Iter_] == _Val))
    void *  __cdecl memset(_Out_writes_bytes_all_(_Size) void * _Dst, _In_ int _Val, _In_ size_t _Size);

#elif _MSC_VER == 1700
    extern _Check_return_ int     __cdecl memcmp(_In_reads_bytes_(_Size) const void * _Buf1, _In_reads_bytes_(_Size) const void * _Buf2, _In_ size_t _Size);

    _Post_equal_to_(_Dst)
        _At_buffer_((unsigned char*)_Dst, _Iter_, _Size, _Post_satisfies_(((unsigned char*)_Dst)[_Iter_] == ((unsigned char*)_Src)[_Iter_]))
        extern void *  __cdecl memcpy(_Out_writes_bytes_all_(_Size) void * _Dst, _In_reads_bytes_(_Size) const void * _Src, _In_ size_t _Size);

    _Post_equal_to_(_Dst)
        _At_buffer_((unsigned char*)_Dst, _Iter_, _Size, _Post_satisfies_(((unsigned char*)_Dst)[_Iter_] == _Val))
        extern void *  __cdecl memset(_Out_writes_bytes_all_(_Size) void * _Dst, _In_ int _Val, _In_ size_t _Size);
#elif _MSC_VER == 1600
    extern int __cdecl memcmp(const void *,const void *,size_t _Size);
    extern void * __cdecl memcpy(void *,const void *,size_t _Size);
    extern void * __cdecl memset(void *,int,size_t _Size);
#else
#error "Provide proper forward declares"
#endif
#if !defined(XR_COMPILER_INTEL)
    // Tell the compiler to make these intrinsics (or else they will be function calls)
#   pragma intrinsic(memcmp)
#   pragma intrinsic(memcpy)
#   pragma intrinsic(memset)
#endif
}
#endif

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief fills memory with 0 in given increments.
    larger increments can be optimized more, but may have platform
    specific alignment requirements. for platform independence the
    alignment of a buffer much be at least the increment.
*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
void MemClear8(  XR_OUT_BYTES(count8 ) void     * __restrict p, size_t count8);
void MemClear16( XR_OUT_COUNT(count16) uint16_t * __restrict p, size_t count16);
void MemClear32( XR_OUT_COUNT(count32) uint32_t * __restrict p, size_t count32);
void MemClear64( XR_OUT_COUNT(count64) uint64_t * __restrict p, size_t count64);
void MemClear128(void     * __restrict p, size_t count128);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief fills memory with passed seed in given increments.
larger increments can be optimized more, but may have platform
specific alignment requirements. for platform independence the
alignment of a buffer much be at least the increment.
*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
void MemFill8(  XR_OUT_BYTES(count8 ) void     * __restrict p,  uint8_t seed, size_t count8 );
void MemFill16( XR_OUT_COUNT(count16) uint16_t * __restrict p, uint16_t seed, size_t count16);
void MemFill32( XR_OUT_COUNT(count32) uint32_t * __restrict p, uint32_t seed, size_t count32);
void MemFill64( XR_OUT_COUNT(count64) uint64_t * __restrict p, uint64_t seed, size_t count64);
void MemFill128(void * __restrict p, XR_IN_BYTES(16) const void    * __restrict pseed, size_t count128);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief verifies that memory was previously filled in given increments
larger increments can be optimized more, but may have platform
specific alignment requirements. for platform independence the
alignment of a buffer much be at least the increment.
*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
StandardResult MemCheck8(  XR_IN_BYTES(count8 ) const void     * __restrict p,  uint8_t seed, size_t count8 );
StandardResult MemCheck16( XR_IN_COUNT(count16) const uint16_t * __restrict p, uint16_t seed, size_t count16);
StandardResult MemCheck32( XR_IN_COUNT(count32) const uint32_t * __restrict p, uint32_t seed, size_t count32);
StandardResult MemCheck64( XR_IN_COUNT(count64) const uint64_t * __restrict p, uint64_t seed, size_t count64);
StandardResult MemCheck128(const void     * __restrict p, XR_IN_BYTES(16) const void    * __restrict pseed, size_t count128);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Copies a buffer to another buffers in given increments
larger increments can be optimized more, but may have platform
specific alignment requirements. for platform independence the
alignment of a buffer much be at least the increment.
*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
void MemCopy8(  XR_OUT_BYTES(count8 ) void     * __restrict pDest, XR_IN_BYTES(count8 ) const void     * __restrict pSrc, size_t count8 );
void MemCopy16( XR_OUT_COUNT(count16) uint16_t * __restrict pDest, XR_IN_COUNT(count16) const uint16_t * __restrict pSrc, size_t count16);
void MemCopy32( XR_OUT_COUNT(count32) uint32_t * __restrict pDest, XR_IN_COUNT(count32) const uint32_t * __restrict pSrc, size_t count32);
void MemCopy64( XR_OUT_COUNT(count64) uint64_t * __restrict pDest, XR_IN_COUNT(count64) const uint64_t * __restrict pSrc, size_t count64);
void MemCopy128(void * __restrict pDest, const void * __restrict pSrc, size_t count128);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Compares two buffers in given increments
larger increments can be optimized more, but may have platform
specific alignment requirements. for platform independence the
alignment of a buffer much be at least the increment.
*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
int MemCompare8(   XR_IN_BYTES(count8 ) const void     * __restrict p1,  XR_IN_BYTES(count8 ) const void     * __restrict p2, size_t count8 );
int MemCompare16(  XR_IN_COUNT(count16) const uint16_t * __restrict p1,  XR_IN_COUNT(count16) const uint16_t * __restrict p2, size_t count16);
int MemCompare32(  XR_IN_COUNT(count32) const uint32_t * __restrict p1,  XR_IN_COUNT(count32) const uint32_t * __restrict p2, size_t count32);
int MemCompare64(  XR_IN_COUNT(count64) const uint64_t * __restrict p1,  XR_IN_COUNT(count64) const uint64_t * __restrict p2, size_t count64);
int MemCompare128(const void * __restrict p1, const void * __restrict p2, size_t count128);
//@}



#if defined(XR_COMPILER_MICROSOFT)
// These are all compiler intrinsics, so just re-direct back to them.

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemClear8(  XR_OUT_BYTES(count8 ) void     * __restrict p, size_t count8)
{
    ::memset(p, 0, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemClear16( XR_OUT_COUNT(count16) uint16_t * __restrict p, size_t count16)
{
    ::memset(p, 0, count16 * 2);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemClear32( XR_OUT_COUNT(count32) uint32_t * __restrict p, size_t count32)
{
    ::memset(p, 0, count32 * 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemClear64( XR_OUT_COUNT(count64) uint64_t * __restrict p, size_t count64)
{
    ::memset(p, 0, count64 * 8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemClear128(void * __restrict p, size_t count128)
{
    ::memset(p, 0, count128 * 16);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemFill8(  XR_OUT_BYTES(count8 ) void     * __restrict p,  uint8_t seed, size_t count8 )
{
    ::memset(p, seed, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemCopy8(  XR_OUT_BYTES(count8 ) void     * __restrict pDest, XR_IN_BYTES(count8 ) const void     * __restrict pSrc, size_t count8 )
{
    ::memcpy(pDest, pSrc, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemCopy16( XR_OUT_COUNT(count16) uint16_t * __restrict pDest, XR_IN_COUNT(count16) const uint16_t * __restrict pSrc, size_t count16)
{
    ::memcpy(pDest, pSrc, count16 * 2);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemCopy32( XR_OUT_COUNT(count32) uint32_t * __restrict pDest, XR_IN_COUNT(count32) const uint32_t * __restrict pSrc, size_t count32)
{
    ::memcpy(pDest, pSrc, count32 * 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemCopy64( XR_OUT_COUNT(count64) uint64_t * __restrict pDest, XR_IN_COUNT(count64) const uint64_t * __restrict pSrc, size_t count64)
{
    ::memcpy(pDest, pSrc, count64 * 8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE void MemCopy128(void * __restrict pDest, const void * __restrict pSrc, size_t count128)
{
    ::memcpy(pDest, pSrc, count128 * 16);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE int MemCompare8(   XR_IN_BYTES(count8 ) const void     * __restrict p1,  XR_IN_BYTES(count8 ) const void     * __restrict p2, size_t count8 )
{
    return ::memcmp(p1, p2, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE int MemCompare16(  XR_IN_COUNT(count16) const uint16_t * __restrict p1,  XR_IN_COUNT(count16) const uint16_t * __restrict p2, size_t count16)
{
    return ::memcmp(p1, p2, count16 * 2);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE int MemCompare32(  XR_IN_COUNT(count32) const uint32_t * __restrict p1,  XR_IN_COUNT(count32) const uint32_t * __restrict p2, size_t count32)
{
    return ::memcmp(p1, p2, count32 * 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE int MemCompare64(  XR_IN_COUNT(count64) const uint64_t * __restrict p1,  XR_IN_COUNT(count64) const uint64_t * __restrict p2, size_t count64)
{
    return ::memcmp(p1, p2, count64 * 8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE int MemCompare128(const void * __restrict p1, const void * __restrict p2, size_t count128)
{
    return ::memcmp(p1, p2, count128 * 16);
}
#endif

#if defined(XR_COMPILER_GCC)
// These are all compiler intrinsics, so just re-direct back to them.

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemClear8(  void * __restrict pDest, size_t count8 )
{
    __builtin_memset(pDest, 0, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemClear16( uint16_t * __restrict pDest, size_t count16)
{
    __builtin_memset(pDest, 0, count16 * 2);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemClear32( uint32_t * __restrict pDest, size_t count32)
{
    __builtin_memset(pDest, 0, count32 * 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemClear64( uint64_t * __restrict pDest, size_t count64)
{
    __builtin_memset(pDest, 0, count64 * 8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemClear128(void * __restrict pDest, size_t count128)
{
    __builtin_memset(pDest, 0, count128 * 16);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemFill8(  void * __restrict pDest, uint8_t seed, size_t count8 )
{
    __builtin_memset(pDest, seed, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemCopy8(  void * __restrict pDest, const void * __restrict pSrc, size_t count8 )
{
    __builtin_memcpy(pDest, pSrc, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemCopy16( uint16_t * __restrict pDest, const uint16_t * __restrict pSrc, size_t count16)
{
    __builtin_memcpy(pDest, pSrc, count16 * 2);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemCopy32( uint32_t * __restrict pDest, const uint32_t * __restrict pSrc, size_t count32)
{
    __builtin_memcpy(pDest, pSrc, count32 * 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemCopy64( uint64_t * __restrict pDest, const uint64_t * __restrict pSrc, size_t count64)
{
    __builtin_memcpy(pDest, pSrc, count64 * 8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void MemCopy128(void * __restrict pDest, const void * __restrict pSrc, size_t count128)
{
    __builtin_memcpy(pDest, pSrc, count128 * 16);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline int MemCompare8(  const void * __restrict pDest, const void * __restrict pSrc, size_t count8 )
{
    return __builtin_memcmp(pDest, pSrc, count8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline int MemCompare16( const uint16_t * __restrict pDest, const uint16_t * __restrict pSrc, size_t count16)
{
    return __builtin_memcmp(pDest, pSrc, count16 * 2);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline int MemCompare32( const uint32_t * __restrict pDest, const uint32_t * __restrict pSrc, size_t count32)
{
    return __builtin_memcmp(pDest, pSrc, count32 * 4);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline int MemCompare64( const uint64_t * __restrict pDest, const uint64_t * __restrict pSrc, size_t count64)
{
    return __builtin_memcmp(pDest, pSrc, count64 * 8);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline int MemCompare128(const void * __restrict pDest, const void * __restrict pSrc, size_t count128)
{
    return __builtin_memcmp(pDest, pSrc, count128 * 16);
}
#endif
}}
#endif //#ifndef XR_CORE_MEM_UTILS_H
