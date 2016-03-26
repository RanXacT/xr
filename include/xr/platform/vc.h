// ######################################################################################### - FILE
/*! \file
    \brief Defines specific to visual studio builds.
    This includes support for intel compiler in Visual studio emulation mode.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_INTERNAL_DEFINES_VC_H
#define XR_INTERNAL_DEFINES_VC_H
#pragma once

// useful _MSC_VER values (for reference):
//  1100: VS  5.0        [NOT SUPPORTED]
//  1200: VS  6.0        [NOT SUPPORTED]
//  1300: VS  7.0 (2002) [NOT SUPPORTED]
//  1310: VS  7.1 (2003) [NOT SUPPORTED]
//  1400: VS  8.0 (2005) [NOT SUPPORTED]
//  1500: VS  9.0 (2008) [NOT SUPPORTED]
//  1600: VS 10.0 (2010) [NOT SUPPORTED]
//  1700: VS 11.0 (2012) [NOT SUPPORTED]
//  1800: VS 12.0 (2013) [NOT SUPPORTED]
//  1800: VS 12.0 (2013) [NOT SUPPORTED]
//  1900: VS 14.0 (2015)

#if !defined( _MSC_VER )
#error "This file is intended for Windows / MSVC platform"
#endif

//@{
/// \internal
// These help in case anyone includes windows.h later.
// windows.h is about 2.2MB of preprocessed code!
// with these defines, it's about 1MB (still crazy though)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
//@}

//These are only required if you build with /Wall
#pragma warning(disable: 4514) // unreferenced inline function has been removed
#pragma warning(disable: 4710) // 'std::_Exception_ptr std::_Exception_ptr::_Current_exception(void)' : function not inlined
#pragma warning(disable: 4820) // bytes padding added after data member
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE
#define NOMINMAX 1
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Platform Stuff (most easily identified by predefined macros from compiler):
#if defined(_M_X64)
// Define both x86 and x64 in this case. (not ideal, but it is common practice)
#   define XR_CPU_X86            1
#   define XR_CPU_X64            1
#   define XR_CPU_NAME           "x64"
#   define XR_PLATFORM_PTR_SIZE  8
#   define XR_PLATFORM_UINT64_T_ALIGNMENT 8
#elif defined(_M_IX86)
#   define XR_CPU_X86            1
#   define XR_CPU_NAME           "x86"
#   define XR_PLATFORM_PTR_SIZE  4
#   define XR_PLATFORM_UINT64_T_ALIGNMENT 8
#else
#   error "Unknown processor type encountered"
#   define XR_PLATFORM_PTR_SIZE  4
#endif
//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{

// if this is intel compiler is emulating Microsoft, define XR_COMPILER_INTEL as well.
#if defined(__INTEL_COMPILER)
#define XR_COMPILER_INTEL     1
#define XR_COMPILER_NAME "ICC-MSVC++"
#endif

/// Compiler description
#define XR_COMPILER_MICROSOFT 1

#ifndef XR_COMPILER_NAME
#define XR_COMPILER_NAME "MSVC++"
#endif//XR_COMPILER_NAME

//@}

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_PUSH_SUPRESS_MSVC_WARNINGS
#define XR_PUSH_SUPRESS_MSVC_WARNINGS(xxx) __pragma(warning(push)) __pragma(warning(disable:xxx))
#endif
#ifndef XR_POP_SUPRESS_MSVC_WARNINGS
#define XR_POP_SUPRESS_MSVC_WARNINGS()     __pragma(warning(pop))
#endif

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#ifndef XR_DISABLE_ALL_WARNINGS
#define XR_DISABLE_ALL_WARNINGS() __pragma(warning(push, 0))
#endif//XR_DISABLE_ALL_WARNINGS
#ifndef XR_RESTORE_ALL_WARNINGS
#define XR_RESTORE_ALL_WARNINGS() __pragma(warning(pop))
#endif//XR_RESTORE_ALL_WARNINGS
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO

// Lets NOT include any system headers here. They are huge really, and often
// unnecessary.

/* byteswap functions */
#if _MSC_VER >= 1900
#include <sal.h>
extern "C" {
	_Check_return_ unsigned short   __cdecl _byteswap_ushort(_In_ unsigned short   _Number);
	_Check_return_ unsigned long    __cdecl _byteswap_ulong(_In_ unsigned long    _Number);
	_Check_return_ unsigned __int64 __cdecl _byteswap_uint64(_In_ unsigned __int64 _Number);
};
#else 
extern "C" {
	unsigned short   __cdecl _byteswap_ushort(_In_ unsigned short   _Number);
	unsigned long    __cdecl _byteswap_ulong(_In_ unsigned long    _Number);
	unsigned __int64 __cdecl _byteswap_uint64(_In_ unsigned __int64 _Number);
};
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#ifndef XR_ALIGN_PREFIX
#define XR_ALIGN_PREFIX(  min_align ) __declspec(align(min_align))
#endif//XR_ALIGN_PREFIX
#ifndef XR_ALIGN_POSTFIX
#define XR_ALIGN_POSTFIX( min_align )
#endif//XR_ALIGN_POSTFIX
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#define XR_ALIGN_OF(type)   __alignof(type)
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO

#ifndef XR_INLINE
#define XR_INLINE           inline
#endif//XR_INLINE

#ifndef XR_FORCE_INLINE
#define XR_FORCE_INLINE     __forceinline
#endif//XR_FORCE_INLINE

#ifndef XR_NO_INLINE
#define XR_NO_INLINE        __declspec(noinline)
#endif//XR_NO_INLINE

#ifndef XR_ANALYSIS_ASSUME
#define XR_ANALYSIS_ASSUME(expr)  __analysis_assume(expr)
#endif//XR_ANALYSIS_ASSUME
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_COMPILER_SUPPORTS_CONSTEXPR
#define XR_COMPILER_SUPPORTS_CONSTEXPR (_MSC_VER >= 1900)
#endif//XR_COMPILER_SUPPORTS_CONSTEXPR
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// Standard Macro
#define XR_CURRENT_FILE         __FILE__
#define XR_CURRENT_FILE_WIDE    XR_PREPROCESSOR_JOIN(L,XR_CURRENT_FILE)
/// Standard Macro
#define XR_CURRENT_LINE         __LINE__
/// I.e. "xr::Foo"
#define XR_CURRENT_FUNCTION     __FUNCTION__
#define XR_CURRENT_FUNCTION_WIDE     XR_PREPROCESSOR_JOIN(L,XR_CURRENT_FUNCTION)
/// I.e. "xr::Foo(int, const char *)"
#define XR_CURRENT_FUNCTION_DECORATED __FUNCDNAME__
#define XR_CURRENT_FUNCTION_DECORATED_WIDE XR_PREPROCESSOR_JOIN(L,XR_CURRENT_FUNCTION_DECORATED)
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// Breaks the current process. Equivalent to __asm { int 3 }
#define XR_HALT()                 __debugbreak()
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// literal value type specifiers
#   define XR_INT8_C(v)   v##i8
#   define XR_INT16_C(v)  v##i16
#   define XR_INT32_C(v)  v##i32
#   define XR_INT64_C(v)  v##i64
#   define XR_UINT8_C(v)  v##ui8
#   define XR_UINT16_C(v) v##ui16
#   define XR_UINT32_C(v) v##ui32
#   define XR_UINT64_C(v) v##ui64
#   define XR_INTMAX_C    XR_INT64_C
#   define XR_UINTMAX_C   XR_UINT64_C

#if XR_PLATFORM_PTR_SIZE == 8
#   define XR_INTPTR_C       XR_INT64_C
#   define XR_UINTPTR_C     XR_UINT64_C
#   define XR_SIZE_C        XR_UINT64_C
#else
#   define XR_INTPTR_C       XR_INT32_C
#   define XR_UINTPTR_C     XR_UINT32_C
#   define XR_SIZE_C        XR_UINT32_C
#endif // XR_PLATFORM_PTR_SIZE == 8

#   define XR_CHAR8_C(v) v##i8
#   define XR_CHAR16_C(v) v##ui16
#   define XR_CHAR32_C(v) v##ui32
#   define XR_WCHAR_C(v) v##ui16

// Prevent redefinition
#define _STDINT

//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// C99 inttypes.h define wrappers http://www.opengroup.org/onlinepubs/000095399/basedefs/inttypes.h.html
/// These define format specifiers for standard integer types.
#   define XR_INT8_PRINT    "d"
#   define XR_INT16_PRINT   "hd"
#   define XR_INT32_PRINT   "I32d"
#   define XR_INT64_PRINT   "I64d"
#   define XR_UINT8_PRINT   "u"
#   define XR_UINT16_PRINT  "hu"
#   define XR_UINT32_PRINT  "I32u"
#   define XR_UINT64_PRINT  "I64u"
#   define XR_INTMAX_PRINT  XR_INT64_PRINT
#   define XR_UINTMAX_PRINT XR_UINT64_PRINT
#   define XR_INT8_PRINTx   "x"
#   define XR_INT16_PRINTx  "hx"
#   define XR_INT32_PRINTx  "I32x"
#   define XR_INT64_PRINTx  "I64x"
#   define XR_UINT8_PRINTx   "x"
#   define XR_UINT16_PRINTx  "hx"
#   define XR_UINT32_PRINTx  "I32x"
#   define XR_UINT64_PRINTx  "I64x"
#   define XR_INTMAX_PRINTx  XR_INT64_PRINTx
#   define XR_UINTMAX_PRINTx XR_UINT64_PRINTx
#   define XR_INT8_PRINTX   "X"
#   define XR_INT16_PRINTX  "hX"
#   define XR_INT32_PRINTX  "I32X"
#   define XR_INT64_PRINTX  "I64X"
#   define XR_UINT8_PRINTX   "X"
#   define XR_UINT16_PRINTX  "hX"
#   define XR_UINT32_PRINTX  "I32X"
#   define XR_UINT64_PRINTX  "I64X"
#   define XR_INTMAX_PRINTX  XR_INT64_PRINTX
#   define XR_UINTMAX_PRINTX XR_UINT64_PRINTX
//# define XR_INT8_SCAN     --- Not Possible
#   define XR_INT16_SCAN   "hd"
#   define XR_INT32_SCAN   "d"
#   define XR_INT64_SCAN   "I64d"
//# define XR_UINT8_SCAN    --- Not Possible
#   define XR_UINT16_SCAN  "hu"
#   define XR_UINT32_SCAN  "u"
#   define XR_UINT64_SCAN  "I64u"
#   define XR_INTMAX_SCAN  XR_INT64_SCAN
#   define XR_UINTMAX_SCAN XR_UINT64_SCAN
//# define XR_INT8_SCANx     --- Not Possible
#   define XR_INT16_SCANx  "hx"
#   define XR_INT32_SCANx  "x"
#   define XR_INT64_SCANx  "I64x"
//# define XR_UINT8_SCANx    --- Not Possible
#   define XR_UINT16_SCANx  "hx"
#   define XR_UINT32_SCANx  "x"
#   define XR_UINT64_SCANx  "I64x"
#   define XR_INTMAX_SCANx  XR_INT64_SCANx
#   define XR_UINTMAX_SCANx XR_UINT64_SCANx
// #    define XR_INT8_SCANX   --- Not Possible
#   define XR_INT16_SCANX  "hX"
#   define XR_INT32_SCANX  "X"
#   define XR_INT64_SCANX  "I64X"
// #    define XR_UINT8_SCANX  --- Not Possible
#   define XR_UINT16_SCANX  "hX"
#   define XR_UINT32_SCANX  "X"
#   define XR_UINT64_SCANX  "I64X"
#   define XR_INTMAX_SCANX  XR_INT64_SCANX
#   define XR_UINTMAX_SCANX XR_UINT64_SCANX


#if XR_PLATFORM_PTR_SIZE == 8
#   define XR_INTPTR_PRINT        XR_INT64_PRINT
#   define XR_UINTPTR_PRINT       XR_UINT64_PRINT
#   define XR_SIZE_PRINT          XR_UINT64_PRINT
#   define XR_INTPTR_PRINTx       XR_INT64_PRINTx
#   define XR_UINTPTR_PRINTx      XR_UINT64_PRINTx
#   define XR_SIZE_PRINTx         XR_UINT64_PRINTx
#   define XR_INTPTR_PRINTX       XR_INT64_PRINTX
#   define XR_UINTPTR_PRINTX      XR_UINT64_PRINTX
#   define XR_SIZE_PRINTX         XR_UINT64_PRINTX
#   define XR_INTPTR_SCAN         XR_INT64_SCAN
#   define XR_UINTPTR_SCAN        XR_UINT64_SCAN
#   define XR_SIZE_SCAN           XR_UINT64_SCAN
#   define XR_INTPTR_SCANx        XR_INT64_SCANx
#   define XR_UINTPTR_SCANx       XR_UINT64_SCANx
#   define XR_SIZE_SCANx          XR_UINT64_SCANx
#   define XR_INTPTR_SCANX        XR_INT64_SCANX
#   define XR_UINTPTR_SCANX       XR_UINT64_SCANX
#   define XR_SIZE_SCANX          XR_UINT64_SCANX
#else
#   define XR_INTPTR_PRINT       XR_INT32_PRINT
#   define XR_UINTPTR_PRINT     XR_UINT32_PRINT
#   define XR_SIZE_PRINT        XR_UINT32_PRINT
#   define XR_INTPTR_PRINTx       XR_INT32_PRINTx
#   define XR_UINTPTR_PRINTx     XR_UINT32_PRINTx
#   define XR_SIZE_PRINTx        XR_UINT32_PRINTx
#   define XR_INTPTR_PRINTX       XR_INT32_PRINTX
#   define XR_UINTPTR_PRINTX     XR_UINT32_PRINTX
#   define XR_SIZE_PRINTX        XR_UINT32_PRINTX
#   define XR_INTPTR_SCAN       XR_INT32_SCAN
#   define XR_UINTPTR_SCAN     XR_UINT32_SCAN
#   define XR_SIZE_SCAN        XR_UINT32_SCAN
#   define XR_INTPTR_SCANx       XR_INT32_SCANx
#   define XR_UINTPTR_SCANx     XR_UINT32_SCANx
#   define XR_SIZE_SCANx        XR_UINT32_SCANx
#   define XR_INTPTR_SCANX       XR_INT32_SCANX
#   define XR_UINTPTR_SCANX     XR_UINT32_SCANX
#   define XR_SIZE_SCANX        XR_UINT32_SCANX
#endif // XR_PLATFORM_PTR_SIZE == 8
//@}

// -----------------------------------------------------------------------------------------  MACRO
#if defined(__STDC_LIMIT_MACROS)
//@{
/// C99 Defines: http://en.wikipedia.org/wiki/Stdint.h
/// (Since MSVC++ does not provide stdint.h)
/// Min / Max equivalents
#   define INT8_MIN     XR_INT8_MIN
#   define INT8_MAX     XR_INT8_MAX
#   define INT16_MIN    XR_INT16_MIN
#   define INT16_MAX    XR_INT16_MAX
#   define INT32_MIN    XR_INT32_MIN
#   define INT32_MAX    XR_INT32_MAX
#   define INT64_MIN    XR_INT64_MIN
#   define INT64_MAX    XR_INT64_MAX
#   define UINT8_MAX    XR_UINT8_MAX
#   define UINT16_MAX   XR_UINT16_MAX
#   define UINT32_MAX   XR_UINT32_MAX
#   define UINT64_MAX   XR_UINT64_MAX
#   define INTPTR_MIN   XR_INTPTR_MIN
#   define INTPTR_MAX   XR_INTPTR_MAX
#   define UINTPTR_MAX  XR_UINTPTR_MAX
#if !defined(SIZE_MAX)
#   define SIZE_MAX     XR_SIZE_MAX
#endif // !defined(SIZE_MAX)
//@}
#endif // defined(__STDC_LIMIT_MACROS)
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#if defined(__STDC_CONSTANT_MACROS)
//@{
/// Constant decorations
#   define INT8_C(v)   XR_INT8_C(v)
#   define INT16_C(v)  XR_INT16_C(v)
#   define INT32_C(v)  XR_INT32_C(v)
#   define INT64_C(v)  XR_INT64_C(v)

#   define UINT8_C(v)  XR_UINT8_C(v)
#   define UINT16_C(v) XR_UINT16_C(v)
#   define UINT32_C(v) XR_UINT32_C(v)
#   define UINT64_C(v) XR_UINT64_C(v)

#   define INTMAX_C   XR_INTMAX_C
#   define UINTMAX_C  XR_UINTMAX_C
//@}
#endif // defined(__STDC_CONSTANT_MACROS)

// ######################################################################################### - FILE
#if _MSC_VER < 1900
// typedefs
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// C99 Types: http://en.wikipedia.org/wiki/Stdint.h
/// (Since MSVC++ does not provide stdint.h)
// Standard ints / uints
typedef __int8            int8_t;
typedef unsigned __int8  uint8_t;
typedef __int16           int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32           int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64           int64_t;
typedef unsigned __int64 uint64_t;
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// at "least" int's
typedef  int8_t           int_least8_t;
typedef uint8_t          uint_least8_t;
typedef  int16_t          int_least16_t;
typedef uint16_t         uint_least16_t;
typedef  int32_t          int_least32_t;
typedef uint32_t         uint_least32_t;
typedef  int64_t          int_least64_t;
typedef uint64_t         uint_least64_t;
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// Fast Ints
typedef  int8_t           int_fast8_t;
typedef uint8_t          uint_fast8_t;
typedef  int16_t          int_fast16_t;
typedef uint16_t         uint_fast16_t;
typedef  int32_t          int_fast32_t;
typedef uint32_t         uint_fast32_t;
typedef  int64_t          int_fast64_t;
typedef uint64_t         uint_fast64_t;
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// max size int's
typedef  int64_t          intmax_t;
typedef uint64_t         uintmax_t;
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// sized char types
typedef char           char8_t;
typedef unsigned short char16_t;
typedef unsigned int   char32_t;
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// Pointer Shims
#if XR_PLATFORM_PTR_SIZE == 8
    typedef int64_t       intptr_t;
    typedef uint64_t     uintptr_t;
#else
    typedef __w64  int32_t  intptr_t;
    typedef __w64 uint32_t uintptr_t;
#endif // XR_PLATFORM_PTR_SIZE == 8
//@}
#else 
#include <stdint.h>
typedef char           char8_t;
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Type ranges
#define XR_INT8_MIN      int8_t( -128)
#define XR_INT8_MAX      int8_t(  127)
#define XR_INT16_MIN     int16_t(-32768)
#define XR_INT16_MAX     int16_t( 32767)
#define XR_INT32_MAX     2147483647i32
#define XR_INT32_MIN     (0i32-(XR_INT32_MAX)-1i32)  // intel compiler does not like using (-2147483648i32) directly, seems like a compiler bug
#define XR_INT64_MAX     9223372036854775807i64
#define XR_INT64_MIN     (0i64-(XR_INT64_MAX)-1i64)  // intel compiler does not like using (-9223372036854775808i64) directly, seems like a compiler bug
#define XR_UINT8_MIN     uint8_t( 0x00ui8)
#define XR_UINT8_MAX     uint8_t( 0xFFui8)
#define XR_UINT16_MIN    uint16_t(0x0000ui16)
#define XR_UINT16_MAX    uint16_t(0xFFFFui16)
#define XR_UINT32_MIN    uint32_t(0x00000000ui32)
#define XR_UINT32_MAX    uint32_t(0xFFFFFFFFui32)
#define XR_UINT64_MIN    uint64_t(0x0000000000000000ui64)
#define XR_UINT64_MAX    uint64_t(0xFFFFFFFFFFFFFFFFui64)

#define XR_INTMAX_MIN     XR_UINT64_MIN
#define XR_INTMAX_MAX     XR_UINT64_MAX
#define XR_UINTMAX_MIN    XR_UINT64_MIN
#define XR_UINTMAX_MAX    XR_UINT64_MAX

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

#define XR_CHAR8_IS_SIGNED 1
#define XR_WCHAR_IS_UNSIGNED 1

#ifndef _NATIVE_WCHAR_T_DEFINED
#define XR_WCHAR_IS_NATIVE    1
#else
#define XR_WCHAR_IS_NATIVE    0
#endif

#define XR_WCHAR_SIZE         2

#define XR_CHAR16_IS_UNSIGNED 1
#define XR_CHAR32_IS_UNSIGNED 1

#define XR_WCHAR_MIN     __wchar_t(XR_UINT16_MIN)
#define XR_WCHAR_MAX     __wchar_t(XR_UINT16_MAX)
#define XR_CHAR8_MIN     char8_t(XR_INT8_MIN)
#define XR_CHAR8_MAX     char8_t(XR_INT8_MAX)
#define XR_CHAR16_MIN    char16_t(XR_WCHAR_MIN)
#define XR_CHAR16_MAX    char16_t(XR_WCHAR_MAX)
#define XR_CHAR32_MIN    char32_t(XR_UINT32_MIN)
#define XR_CHAR32_MAX    char32_t(XR_UINT32_MAX)

#define XR_STR_WCHAR_T(str) L##str
#define XR_STR_CHAR16_T(str) ((const char16_t*)(L##str))

//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Platform Endianness and conversion support
#if defined(__INTEL_COMPILER)

    // Intel Compiler has its own intrinsics
    // ushort version returns a long on intel compiler.. bug?!?!
    // work around by explicitly typecasting result as uint16_t
#   if !defined(XR_SWAP_UINT16_T)
#       define  XR_SWAP_UINT16_T(value)  uint16_t( _byteswap_ushort(value) )
#   endif
#   if !defined(XR_SWAP_UINT32_T)
#       define  XR_SWAP_UINT32_T(value)  static_cast<uint32_t>( _bswap(value) )   /* intel compiler intrinsic */
#   endif
#   if !defined(XR_SWAP_UINT64_T)
#       define  XR_SWAP_UINT64_T(value)  static_cast<uint64_t>( _bswap64(value)) /* intel compiler intrinsic */
#   endif

#else // defined(__INTEL_COMPILER)

// Visual studio has a pragma to turn these functions into intrinsics
#   pragma intrinsic(_byteswap_uint64)
#   pragma intrinsic(_byteswap_ulong)
#   pragma intrinsic(_byteswap_ushort)
#   if !defined(XR_SWAP_UINT16_T)
#       define  XR_SWAP_UINT16_T(value)  _byteswap_ushort(value)
#   endif
#   if !defined(XR_SWAP_UINT32_T)
#       define  XR_SWAP_UINT32_T(value)  _byteswap_ulong(value)
#   endif
#   if !defined(XR_SWAP_UINT64_T)
#       define  XR_SWAP_UINT64_T(value)  _byteswap_uint64(value)
#   endif

#endif // defined(__INTEL_COMPILER)
//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#ifndef XR_PURE_INTERFACE
#define XR_PURE_INTERFACE __declspec(novtable)
#endif//XR_PURE_INTERFACE
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#ifndef XR_RESTRICT_RETURN
#define XR_RESTRICT_RETURN __declspec(restrict)
#endif//XR_PURE_INTERFACE
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#ifndef XR_NO_SIDE_EFFECTS_PREFIX
#define XR_NO_SIDE_EFFECTS_PREFIX __declspec(noalias)
#endif//XR_NO_SIDE_EFFECTS_PREFIX

/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
// warning C4481: nonstandard extension used: override specifier 'override'
#pragma warning(disable: 4481)
#ifndef XR_OVERRIDE
#define XR_OVERRIDE override
#endif//XR_OVERRIDE
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_FINAL
#define XR_FINAL sealed
#endif//XR_FINAL
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// See docs in defines.h
#ifndef XR_NO_RETURN
#define XR_NO_RETURN __declspec(noreturn)
#endif//XR_PURE_INTERFACE

#if _MSC_VER >= 1700
// Static analysis functionality.
#include <sal.h>
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_IN
#define XR_IN _In_
#endif//XR_IN
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_IN_COUNT
#define XR_IN_COUNT(elementCount) _In_count_(elementCount)
#endif//XR_IN_COUNT
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_IN_BYTES
#define XR_IN_BYTES(byteCount) _In_bytecount_(byteCount)
#endif//XR_IN_BYTES
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_IN_STRING
#define XR_IN_STRING _In_z_
#endif//XR_IN_STRING
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_OUT
#define XR_OUT _Out_
#endif//XR_OUT
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_OUT_COUNT
#define XR_OUT_COUNT(elementCount) _Out_cap_(elementCount)
#endif//XR_OUT_COUNT
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_OUT_BYTES
#define XR_OUT_BYTES(byteCount) _Out_bytecap_(byteCount)
#endif//XR_OUT_BYTES
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_OUT_STRING
#define XR_OUT_STRING(sizeInChars) _Out_writes_z_(sizeInChars)
#endif//XR_OUT_STRING
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_OUT_INVALIDATE
#define XR_OUT_INVALIDATE _Post_ptr_invalid_
#endif//XR_OUT_INVALIDATE
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_IN_OPTIONAL
#define XR_IN_OPTIONAL _In_opt_
#endif//XR_IN_OPTIONAL
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_IN_OPTIONAL_STRING
#define XR_IN_OPTIONAL_STRING _In_opt_z_
#endif//XR_IN_OPTIONAL_STRING
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_RETURN_COUNT
#define XR_RETURN_COUNT(elementCount) _Must_inspect_result_ _Ret_cap_(elementCount)
#endif//XR_RETURN_COUNT
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#ifndef XR_RETURN_BYTES
#if _MSC_VER == 1600
// To get annotation to actually work need, to use a private define
#define XR_RETURN_BYTES(byteCount) _Ret_bytecap_(byteCount) _Deref_ret1_impl_(_$notvalid)
#elif _MSC_VER == 1700 || _MSC_VER == 1800 || _MSC_VER == 1900
#define XR_RETURN_BYTES(byteCount) _Must_inspect_result_ _Ret_bytecap_(byteCount)
#else
#error "test this on new compiler."
#endif
//_Ret_bytecap_(byteCount)
#endif//XR_RETURN_BYTES

#endif

#endif //#ifndef XR_INTERNAL_DEFINES_VC_H
