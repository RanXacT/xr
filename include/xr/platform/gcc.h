// ######################################################################################### - FILE
/*! \file
\brief Defines specific to gcc.
This includes support for ppc / x86 / x64

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE
// Guard
#ifndef XR_INTERNAL_DEFINES_GCC_H
#define XR_INTERNAL_DEFINES_GCC_H
#pragma once

#if !defined( __GNUC__ )
#error "This file is intended for GCC"
#endif

// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
//@{
/// \internal
#ifndef __STDC_FORMAT_MACROS
# define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
# define __STDC_CONSTANT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
# define __STDC_LIMIT_MACROS
#endif
#include <stddef.h> // Get the basics
#include <stdint.h> // Get more basics
#include <inttypes.h>
#include <limits.h>
//@}

#if defined(XR_PLATFORM_DARWIN)
// Instead of including wchar.h, we only need these.
#ifndef WCHAR_MIN
#define WCHAR_MIN   __DARWIN_WCHAR_MIN
#endif
#ifndef WCHAR_MAX
#define WCHAR_MAX   __DARWIN_WCHAR_MAX
#endif
#else
#include <wchar.h>
#endif


// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Compiling via GCC
#define XR_COMPILER_GCC 1

#define XR_COMPILER_NAME "GCC"


#ifdef __MINGW32__
#undef XR_COMPILER_NAME
#define XR_COMPILER_NAME "MinGW-GCC"
#define XR_COMPILER_MINGW 1
#define XR_MINGW_VERSION ((__MINGW32_MAJOR_VERSION*10000)+(__MINGW32_MINOR_VERSION*100)+(0))
#define XR_MINGW_VERSION_AT_LEAST( major, minor) ((((major)*10000)+((minor)*100)+(0)) < (XR_MINGW_VERSION))
#endif

#if __clang__
#undef XR_COMPILER_NAME

#if XR_COMPILER_MINGW
#define XR_COMPILER_NAME "Clang-MinGW-GCC"
#else
#define XR_COMPILER_NAME "Clang-GCC"
#endif
#define XR_COMPILER_CLANG 1
#define XR_CLANG_VERSION ((__clang_major__*10000)+(__clang_minor__*100)+(__clang_patchlevel__))
#define XR_CLANG_VERSION_AT_LEAST( major, minor, patch) ((((major)*10000)+((minor)*100)+(patch)) < (XR_CLANG_VERSION))
#endif


//@}

// -----------------------------------------------------------------------------------------  MACRO
/*! Allows simpler checking of GCC version (which is non trivial and error
    prone)
   Sample Usage:
\verbatim
#if  XR_GCC_VERSION_AT_LEAST(4,3,0)
    // Something only valid for gcc 4.3.0 and up.
#endif
\endverbatim
*/
// -----------------------------------------------------------------------------------------  MACRO
#define XR_GCC_VERSION      ((__GNUC__*10000)+(__GNUC_MINOR__*100)+(__GNUC_PATCHLEVEL__))
#define XR_GCC_VERSION_AT_LEAST( major, minor, patch) ((((major)*10000)+((minor)*100)+(patch)) < (XR_GCC_VERSION))

// -----------------------------------------------------------------------------------------  MACRO
// Simple check for minimum requirements to compile this library.
// -----------------------------------------------------------------------------------------  MACRO
#if !XR_GCC_VERSION_AT_LEAST(3,0,0)
#error "Minimum gcc version supported is 3.x, 2.x versions require significant ugliness."
#endif

// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Platform Stuff (most easily identified by predefined macros from compiler):
// -----------------------------------------------------------------------------------------  MACRO
#if defined(__ppc64__)
#   define XR_CPU_PPC            1
#   define XR_CPU_PPC64          1
#   define XR_CPU_NAME           "PowerPC64"
#   define XR_PLATFORM_PTR_SIZE  8
#   define XR_PLATFORM_UINT64_T_ALIGNMENT  8
#elif defined(__ppc__)
#   define XR_CPU_PPC            1
#   define XR_CPU_NAME           "PowerPC"
#   define XR_PLATFORM_PTR_SIZE  4
#   define XR_PLATFORM_UINT64_T_ALIGNMENT  8
#elif defined(__x86_64__)
#   define XR_CPU_X86            1
#   define XR_CPU_X64            1
#   define XR_CPU_NAME           "x64"
#   define XR_PLATFORM_PTR_SIZE  8
#   define XR_PLATFORM_UINT64_T_ALIGNMENT  8
#elif defined(__clang__) && defined(__i386__)
// clang is different. uint64_t is aligned to 4 bytes
// but only in 32 bit pointer mode.
#   define XR_CPU_X86            1
#   define XR_CPU_NAME           "x86"
#   define XR_PLATFORM_PTR_SIZE  4
#   define XR_PLATFORM_UINT64_T_ALIGNMENT  4
#elif defined(__i386__)
#   define XR_CPU_X86            1
#   define XR_CPU_NAME           "x86"
#   define XR_PLATFORM_PTR_SIZE  4
#   define XR_PLATFORM_UINT64_T_ALIGNMENT  8
#elif defined(__arm__)
#   define XR_CPU_ARM            1
#   define XR_CPU_NAME           "ARM"
#   define XR_PLATFORM_PTR_SIZE  4
#   define XR_PLATFORM_UINT64_T_ALIGNMENT  4
#else
#   error "Unknown processor type encountered"
#endif
//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Alignment, must have pre and post markers to support all compilers.
/// \code
/// XR_ALIGN_PREFIX( 128 ) uint32_t MyAlignedValue XR_ALIGN_POSTFIX( 128 );
/// \endcode
#define XR_ALIGN_PREFIX(  min_align )
#define XR_ALIGN_POSTFIX( min_align ) __attribute__ ((aligned (min_align)))
//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#ifndef XR_ALIGN_OF
#define XR_ALIGN_OF(type)   __alignof__(type)
#endif//XR_ALIGN_OF

#if XR_GCC_VERSION_AT_LEAST(4,0,0)
/// Forced Inline (where compiler supported)
#   ifndef XR_FORCE_INLINE
#   define XR_FORCE_INLINE     __attribute__ ((always_inline) )
#   endif//XR_FORCE_INLINE
/// inline disabled (where compiler supported)
#   ifndef XR_NO_INLINE
#   define XR_NO_INLINE        __attribute__ ((noinline))
#   endif//XR_NO_INLINE
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#if  XR_GCC_VERSION_AT_LEAST(2,9,6)
#   ifndef XR_NO_SIDE_EFFECTS_POSTFIX
#   define XR_NO_SIDE_EFFECTS_POSTFIX   __attribute__ ((pure))
#   endif//XR_NO_SIDE_EFFECTS_POSTFIX
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#if  XR_GCC_VERSION_AT_LEAST(4,3,0)
#   ifndef XR_HOT
#   define XR_HOT               __attribute__ ((hot))
#   endif//XR_HOT
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#if  XR_GCC_VERSION_AT_LEAST(4,7,0)
#   ifndef XR_OVERRIDE
#   define XR_OVERRIDE override
#   endif//XR_OVERRIDE
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#if  XR_GCC_VERSION_AT_LEAST(4,7,0)
#   ifndef XR_FINAL
#   define XR_FINAL final
#   endif//XR_FINAL
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#if  XR_GCC_VERSION_AT_LEAST(3,0,0)
// See docs in defines.h
#   ifndef XR_LIKELY
#   define XR_LIKELY(x)         __builtin_expect(!!(x), 1)
#   endif//XR_LIKELY
// See docs in defines.h
#   ifndef XR_UNLIKELY
#   define XR_UNLIKELY(x)       __builtin_expect(!!(x), 0)
#   endif//XR_UNLIKELY
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#if  XR_GCC_VERSION_AT_LEAST(4,6,0)
#   ifndef XR_COMPILER_SUPPORTS_CONSTEXPR
#   define XR_COMPILER_SUPPORTS_CONSTEXPR 1
#   endif//XR_COMPILER_SUPPORTS_CONSTEXPR
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#if XR_GCC_VERSION_AT_LEAST(3,0,0)
#   ifndef XR_WARN_IF_RETURN_UNUSED
#   define XR_WARN_IF_RETURN_UNUSED  __attribute__ ((warn_unused_result))
#   endif//XR_WARN_IF_RETURN_UNUSED
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#if XR_GCC_VERSION_AT_LEAST(2,5,0)
#ifndef XR_NO_RETURN
#define XR_NO_RETURN __attribute__((noreturn))
#endif//XR_NO_RETURN
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// Standard Macro
#define XR_CURRENT_FILE         __FILE__
#define XR_CURRENT_FILE_WIDE    XR_PREPROCESSOR_JOIN(L,XR_CURRENT_FILE)
/// Standard Macro
#define XR_CURRENT_LINE         __LINE__
/// I.e. "xr::Foo"
#define XR_CURRENT_FUNCTION          __FUNCTION__
#define XR_CURRENT_FUNCTION_WIDE     XR_PREPROCESSOR_JOIN(L,XR_CURRENT_FUNCTION)
/// I.e. "xr::Foo(int, const char *)"
#define XR_CURRENT_FUNCTION_DECORATED      __PRETTY_FUNCTION__
#define XR_CURRENT_FUNCTION_DECORATED_WIDE XR_PREPROCESSOR_JOIN(L,XR_CURRENT_FUNCTION_DECORATED)
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// Breaks the current process. Equivelant to __asm { int 3 }
/// May want to revisit and make asm statements explicitly.
#define XR_HALT()               __builtin_trap()

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// Size of wchar (compiler / option dependent)
#if __SIZEOF_WCHAR_T__ == 4
#define XR_WCHAR_SIZE         4
#else
#define XR_WCHAR_SIZE         2
#endif

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// sized char types. Supporting them all is not easy across versions / options.
// char8_t, char16_t, char32_t
typedef char      char8_t;
#if !((XR_GCC_VERSION_AT_LEAST(4,4,0)) && defined(__GXX_EXPERIMENTAL_CXX0X__))
// GCC 4.4.0+ has new char types internally defined. (if -std=gnu++0x or
// -std=c++0x is specified on compile line)
// If native types are not present, define our own.
typedef uint16_t  char16_t;
typedef uint32_t  char32_t;
#endif

#define XR_WCHAR_IS_NATIVE 1

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Type MIN / MAX values. These are not actual constants, because it would be
/// wasteful to do so since debug builds would create constants in every
/// compilation unit, but such constants would be of little to no use.
#define XR_INT8_MIN    int8_t  ( -128)
#define XR_INT8_MAX    int8_t  (  127)
#define XR_INT16_MIN   int16_t (-32768)
#define XR_INT16_MAX   int16_t ( 32767)
#define XR_INT32_MAX   int32_t (2147483647l)
#define XR_INT32_MIN   int32_t (0L-(XR_INT32_MAX)-1L)
#define XR_INT64_MAX   int64_t ( 9223372036854775807ll)
#define XR_INT64_MIN   int64_t (0ULL-(XR_INT64_MAX)-1ULL)
#define XR_UINT8_MIN   uint8_t ( 0x00)
#define XR_UINT8_MAX   uint8_t ( 0xFF)
#define XR_UINT16_MIN  uint16_t(0x0000)
#define XR_UINT16_MAX  uint16_t(0xFFFF)
#define XR_UINT32_MIN  uint32_t(0x00000000ul)
#define XR_UINT32_MAX  uint32_t(0xFFFFFFFFul)
#define XR_UINT64_MIN  uint64_t(0x0000000000000000ull)
#define XR_UINT64_MAX  uint64_t(0xFFFFFFFFFFFFFFFFull)

// Assume that int64 is the max int for now. no need to do otherwise.
#define XR_INTMAX_MIN     XR_INT64_MIN
#define XR_INTMAX_MAX     XR_INT64_MAX
#define XR_UINTMAX_MIN    XR_UINT64_MIN
#define XR_UINTMAX_MAX    XR_UINT64_MAX

// With char types, follow the compiler's lead. if it's signed, use signed,
// if it's unsigned, use unsigned.
#define XR_WCHAR_MIN      wchar_t(WCHAR_MIN)
#define XR_WCHAR_MAX      wchar_t(WCHAR_MAX)
#define XR_CHAR8_MIN      char8_t(CHAR_MIN)
#define XR_CHAR8_MAX      char8_t(CHAR_MAX)

#if CHAR_MIN < 0
#   define XR_CHAR_IS_SIGNED 1
#   define XR_CHAR8_IS_SIGNED 1
#else
#   define XR_CHAR_IS_UNSIGNED 1
#   define XR_CHAR8_IS_UNSIGNED 1
#endif

#if WCHAR_MIN < 0
#   define XR_WCHAR_IS_SIGNED 1
#   define XR_CHAR16_IS_SIGNED 1
#   define XR_CHAR32_IS_SIGNED 1
#else
#   define XR_WCHAR_IS_UNSIGNED 1
#   define XR_CHAR16_IS_UNSIGNED 1
#   define XR_CHAR32_IS_UNSIGNED 1
#endif


#if XR_WCHAR_IS_SIGNED
#   if __SIZEOF_WCHAR_T__ == 4
    // char32_t maps to wchar_t
#   define XR_CHAR16_MIN   char16_t(XR_INT16_MIN)
#   define XR_CHAR16_MAX   char16_t(XR_INT16_MAX)
#   define XR_CHAR32_MIN   char32_t(XR_WCHAR_MIN)
#   define XR_CHAR32_MAX   char32_t(XR_WCHAR_MAX)
#   else
    // char16_t maps to wchar_t
#   define XR_CHAR16_MIN   char16_t(XR_WCHAR_MIN)
#   define XR_CHAR16_MAX   char16_t(XR_WCHAR_MAX)
#   define XR_CHAR32_MIN   char32_t(XR_INT32_MIN)
#   define XR_CHAR32_MAX   char32_t(XR_INT32_MAX)
#   endif


#else //#if XR_WCHAR_IS_SIGNED

#   if __SIZEOF_WCHAR_T__ == 4
    // char32_t maps to wchar_t
#   define XR_CHAR16_MIN   char16_t(XR_UINT16_MIN)
#   define XR_CHAR16_MAX   char16_t(XR_UINT16_MAX)
#   define XR_CHAR32_MIN   char32_t(XR_WCHAR_MIN)
#   define XR_CHAR32_MAX   char32_t(XR_WCHAR_MAX)
#   else //#if __SIZEOF_WCHAR_T__ == 4
    // char16_t maps to wchar_t
#   define XR_CHAR16_MIN   char16_t(XR_WCHAR_MIN)
#   define XR_CHAR16_MAX   char16_t(XR_WCHAR_MAX)
#   define XR_CHAR32_MIN   char32_t(XR_UINT32_MIN)
#   define XR_CHAR32_MAX   char32_t(XR_UINT32_MAX)
#   endif //#if __SIZEOF_WCHAR_T__ == 4

#endif //#if XR_WCHAR_IS_SIGNED

//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// literal value type specifiers
#define XR_INT8_C(v)   static_cast<int8_t>(v)
#define XR_INT16_C(v)  static_cast<int16_t>(v)
#define XR_INT32_C(v)  static_cast<int32_t>(v##L)
#define XR_INT64_C(v)  static_cast<int64_t>(v##LL)
#define XR_UINT8_C(v)  static_cast<uint8_t>(v)
#define XR_UINT16_C(v) static_cast<uint16_t>(v)
#define XR_UINT32_C(v) static_cast<uint32_t>(v##UL)
#define XR_UINT64_C(v) static_cast<uint64_t>(v##ULL)
#define XR_INTMAX_C    XR_INT64_C
#define XR_UINTMAX_C   XR_UINT64_C
#define XR_CHAR8_C(v) static_cast<char8_t>(v)
#define XR_CHAR16_C(v) static_cast<char16_t>(v)
#define XR_CHAR32_C(v) static_cast<char32_t>(v)
#define XR_WCHAR_C(v) static_cast<wchar_t>(v)

//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Pointer min / max (based on pointer size)
#if XR_PLATFORM_PTR_SIZE == 8
#   define XR_INTPTR_MIN        XR_INT64_MIN
#   define XR_INTPTR_MAX        XR_INT64_MAX
#   define XR_UINTPTR_MIN      XR_UINT64_MIN
#   define XR_UINTPTR_MAX      XR_UINT64_MAX
#   define XR_SIZE_MIN         XR_UINT64_MIN
#   define XR_SIZE_MAX         XR_UINT64_MAX
#else
#   define XR_INTPTR_MIN        XR_INT32_MIN
#   define XR_INTPTR_MAX        XR_INT32_MAX
#   define XR_UINTPTR_MIN      XR_UINT32_MIN
#   define XR_UINTPTR_MAX      XR_UINT32_MAX
#   define XR_SIZE_MIN         XR_UINT32_MIN
#   define XR_SIZE_MAX         XR_UINT32_MAX
#endif // XR_PLATFORM_PTR_SIZE == 8
//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// C99 inttypes.h define wrappers http://www.opengroup.org/onlinepubs/000095399/basedefs/inttypes.h.html
/// These define format specifiers for standard integer types.
/// wide characters should not be used in these. localization should be provided separately.
/// printf is for debug use only.
#   define XR_INT8_PRINT     PRId8
#   define XR_INT16_PRINT    PRId16
#   define XR_INT32_PRINT    PRId32
#   define XR_INT64_PRINT    PRId64
#   define XR_UINT8_PRINT    PRIu8
#   define XR_UINT16_PRINT   PRIu16
#   define XR_UINT32_PRINT   PRIu32
#   define XR_UINT64_PRINT   PRIu64
#   define XR_INTMAX_PRINT   PRIdMAX
#   define XR_UINTMAX_PRINT  PRIuMAX
#   define XR_INT8_PRINTx    PRIx8
#   define XR_INT16_PRINTx   PRIx16
#   define XR_INT32_PRINTx   PRIx32
#   define XR_INT64_PRINTx   PRIx64
#   define XR_UINT8_PRINTx   PRIx8
#   define XR_UINT16_PRINTx  PRIx16
#   define XR_UINT32_PRINTx  PRIx32
#   define XR_UINT64_PRINTx  PRIx64
#   define XR_INTMAX_PRINTx  PRIxMAX
#   define XR_UINTMAX_PRINTx PRIxMAX
#   define XR_INT8_PRINTX    PRIX8
#   define XR_INT16_PRINTX   PRIX16
#   define XR_INT32_PRINTX   PRIX32
#   define XR_INT64_PRINTX   PRIX64
#   define XR_UINT8_PRINTX   PRIX8
#   define XR_UINT16_PRINTX  PRIX16
#   define XR_UINT32_PRINTX  PRIX32
#   define XR_UINT64_PRINTX  PRIX64
#   define XR_INTMAX_PRINTX  PRIXMAX
#   define XR_UINTMAX_PRINTX PRIXMAX
#   define XR_INT8_SCAN      SCNd8
#   define XR_INT16_SCAN     SCNd16
#   define XR_INT32_SCAN     SCNd32
#   define XR_INT64_SCAN     SCNd64
#   define XR_UINT8_SCAN     SCNu8
#   define XR_UINT16_SCAN    SCNu16
#   define XR_UINT32_SCAN    SCNu32
#   define XR_UINT64_SCAN    SCNu64
#   define XR_INTMAX_SCAN    SCNiMAX
#   define XR_UINTMAX_SCAN   SCNuMAX
#   define XR_INT8_SCANx     SCNx8
#   define XR_INT16_SCANx    SCNx16
#   define XR_INT32_SCANx    SCNx32
#   define XR_INT64_SCANx    SCNx64
#   define XR_UINT8_SCANx    SCNx8
#   define XR_UINT16_SCANx   SCNx16
#   define XR_UINT32_SCANx   SCNx32
#   define XR_UINT64_SCANx   SCNx64
#   define XR_INTMAX_SCANx   SCNxMAX
#   define XR_UINTMAX_SCANx  SCNxMAX
#   define XR_INT8_SCANX     SCNx8
#   define XR_INT16_SCANX    SCNx16
#   define XR_INT32_SCANX    SCNx32
#   define XR_INT64_SCANX    SCNx64
#   define XR_UINT8_SCANX    SCNx8
#   define XR_UINT16_SCANX   SCNx16
#   define XR_UINT32_SCANX   SCNx32
#   define XR_UINT64_SCANX   SCNx64
#   define XR_INTMAX_SCANX   SCNxMAX
#   define XR_UINTMAX_SCANX  SCNxMAX

#   define XR_INTPTR_PRINT       PRIdPTR
#   define XR_UINTPTR_PRINT      PRIuPTR
#   define XR_INTPTR_PRINTx      PRIxPTR
#   define XR_UINTPTR_PRINTx     PRIxPTR
#   define XR_INTPTR_PRINTX      PRIXPTR
#   define XR_UINTPTR_PRINTX     PRIXPTR

#   define XR_INTPTR_SCAN        SCNdPTR
#   define XR_UINTPTR_SCAN       SCNuPTR
#   define XR_INTPTR_SCANx       SCNxPTR
#   define XR_UINTPTR_SCANx      SCNxPTR
#   define XR_INTPTR_SCANX       SCNxPTR
#   define XR_UINTPTR_SCANX      SCNxPTR

#if defined(XR_COMPILER_MINGW)
// Mingw is difficult and wants "I" instead of "z"
#   define XR_SIZE_PRINT         "Id"
#   define XR_SIZE_PRINTx        "Ix"
#   define XR_SIZE_PRINTX        "IX"
#   define XR_SIZE_SCAN          "Id"
#   define XR_SIZE_SCANx         "Ix"
#   define XR_SIZE_SCANX         "Ix"
#else
// gcc outside of mingw wants z for these.
#   define XR_SIZE_PRINT         "zd"
#   define XR_SIZE_PRINTx        "zx"
#   define XR_SIZE_PRINTX        "zX"
#   define XR_SIZE_SCAN          "zd"
#   define XR_SIZE_SCANx         "zx"
#   define XR_SIZE_SCANX         "zx"
#endif
//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Override for GCC since it has a native implementation.
#ifndef XR_OFFSET_OF
#   if XR_GCC_VERSION_AT_LEAST(4,0,0)
#      define XR_OFFSET_OF(objectName, fieldName) __builtin_offsetof(objectName,  fieldName )
#   else
#       define XR_OFFSET_OF(objectName, fieldName) offsetof(objectName,  fieldName )
#   endif
#endif//XR_OFFSET_OF
//@}

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// GCC byteswap functions (in case they are not provided by the OS)

/*
This process is a bit convoluted.
Here is the guide:

1: bswap16, there is no GCC equivalent, so we just do our own.
2: bswap32/64, Later versions of GCC provide a version, which we use
   but earlier versions of GCC do not, so we provide one.

Each bswap function has an asm version defined for common platforms, and a
C version elsewise.

*/

#if !defined( XR_PLATFORM_LITTLE_ENDIAN ) && !defined(XR_PLATFORM_BIG_ENDIAN)
#error "System header file should specify endianess, it is not easily obtained from predefined macros"
#endif

#ifndef XR_SWAP_UINT16_T
#define XR_SWAP_UINT16_T(value)  static_cast<uint16_t>(xr_internal_bswap16(value))

// There is no bswap16 provided in any gcc version, write our own.
// Use x86 instructions if available.
#if XR_CPU_X86
inline uint16_t xr_internal_bswap16(uint16_t val)
{
    __asm__("rorw $8, %0" : "+r" (val));
    return val;
}
#elif XR_CPU_PPC
inline uint16_t xr_internal_bswap16(uint16_t val)
{
    __asm__("rlwimi %0,%2,8,16,23" : "=&r" (val) : "0" (val >> 8), "r" (val));
    return val;
}
#else
#error "This is less than optimal. If this is to be used, pull it out explicitly into a platform define"
/// standard version of bswap16
inline uint16_t xr_internal_bswap16(uint16_t val)
{
    return (val<<8)|(val>>8);
}
#endif

#endif // #ifndef XR_SWAP_UINT16_T


#ifndef XR_SWAP_UINT32_T

// GCC 4.3+ has built in definitions of bswap32/64
#if  XR_GCC_VERSION_AT_LEAST(4,3,0)

// Note we have to type cast the results to ensure consistency, and to work around issues with later GCC versions.
#   ifndef XR_SWAP_UINT32_T
#       define XR_SWAP_UINT32_T(value)  static_cast<uint32_t>(__builtin_bswap32(value))
#   endif
#   ifndef XR_SWAP_UINT64_T
#       define XR_SWAP_UINT64_T(value)  static_cast<uint64_t>(__builtin_bswap64(value))
#   endif

#else
// Earlier versions of GCC do not implement bswap built-in functions
// no big deal, we can insert our own.

// Use x86 instructions if available.
#   if XR_CPU_X86
        /// Small private definition of byteswap functions.
        inline uint32_t xr_internal_bswap32(uint32_t val)
        {
            __asm__("bswap %0" : "+r" (val));
            return val;
        }

#       if XR_CPU_X64
        /// Small private definition of byteswap functions.
        inline uint64_t xr_internal_bswap64(uint64_t val)
        {
            __asm__("bswapq %0" : "+r" (val));
            return val;
        }
#       else
        /// Small private definition of byteswap functions.
        inline uint64_t xr_internal_bswap64(uint64_t val)
        {
            uint32_t t0 = (uint32_t)(val);
            uint32_t t1 = (uint32_t)(val>>32ll);
            __asm__("bswap %0" : "+r" (t0));
            __asm__("bswap %0" : "+r" (t1));
            return ((uint64_t)(t0)) << 32ll | ((uint64_t)(t1));
        }
#       endif
#   elif XR_CPU_PPC
    /// Small private definition of byteswap functions.
    inline uint32_t xr_internal_bswap32(uint32_t val)
    {
        uint64_t result;
        __asm__("rlwimi %0,%2,24,16,23" : "=&r" (result) : "0" (val>>24), "r" (val));
        __asm__("rlwimi %0,%2,8,8,15"   : "=&r" (result) : "0" (result),  "r" (val));
        __asm__("rlwimi %0,%2,24,0,7"   : "=&r" (result) : "0" (result),  "r" (val));
        return result;
    }
    /// standard version of bswap64
    inline uint64_t xr_internal_bswap64(uint64_t val)
    {
        return (((unsigned long long)xr_internal_bswap32(val&0xffffffffull))<<32) | (xr_internal_bswap32(val>>32));
    }

#   else // #if XR_CPU_X86

        /// standard version of bswap32
        inline uint32_t xr_internal_bswap32(uint32_t val)
        {
            return (XR_SWAP_UINT16_T(val&0xffff)<<16) | (XR_SWAP_UINT16_T(val>>16));
        }
        /// standard version of bswap64
        inline uint64_t xr_internal_bswap64(uint64_t val)
        {
            return (((unsigned long long)xr_internal_bswap32(val&0xffffffffull))<<32) | (xr_internal_bswap32(val>>32));
        }

#   endif // #if XR_CPU_X86

#   ifndef XR_SWAP_UINT32_T
#   define XR_SWAP_UINT32_T(value)  static_cast<uint32_t>(xr_internal_bswap32(value))
#   endif
#   ifndef XR_SWAP_UINT64_T
#   define XR_SWAP_UINT64_T(value)  static_cast<uint64_t>(xr_internal_bswap64(value))
#   endif
#endif

#endif // #ifndef XR_SWAP_UINT32_T

//@} End of byte swapping functions.
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#ifndef XR_RESTRICT_RETURN
#define XR_RESTRICT_RETURN __attribute__((malloc))
#endif//XR_RESTRICT_RETURN

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO

#endif //#ifndef XR_INTERNAL_DEFINES_GCC_H
