// ######################################################################################### - FILE
/*! \file

\brief Standard defines useful for managing architecture differences

\page defines Platform Abstraction Defines
The platform abstraction defines (in xr/defines.h) provide basic means of
discerning target platform / compiler, as well as providing consistent means
of coding in that environment. Below you will see a list of the defines

Defines the following system values
\code
XR_PLATFORM_*
XR_CPU_*
XR_COMPILER_*
XR_PLATFORM_PTR_SIZE
XR_PLATFORM_LITTLE_ENDIAN | XR_PLATFORM_BIG_ENDIAN
XR_EOL -- End of line in target system
\endcode

Defines the following byte swapping macros:
\code
XR_SWAP_UINT16_T(v)
XR_SWAP_UINT32_T(v)
XR_SWAP_UINT64_T(v)
XR_LITTLE_ENDIAN_TO_NATIVE_UINT16_T(value)
XR_LITTLE_ENDIAN_TO_NATIVE_UINT32_T(value)
XR_LITTLE_ENDIAN_TO_NATIVE_UINT64_T(value)
XR_BIG_ENDIAN_TO_NATIVE_UINT16_T(value)
XR_BIG_ENDIAN_TO_NATIVE_UINT32_T(value)
XR_BIG_ENDIAN_TO_NATIVE_UINT64_T(value)
\endcode

Defines the following alignment macros (See defines.cpp for usage):
\code
XR_ALIGN_PREFIX( min_align)
XR_ALIGN_POSTFIX(min_align)
XR_ALIGN_OF(type)
XR_OFFSET_OF(type, member)
\endcode

Align Exrmple:
\code
// For a struct / class / type
XR_ALIGN_PREFIX(32)
struct AlignmentTesterInner {
    char c[4];
}
XR_ALIGN_POSTFIX(32);

// Or for a member / variable:
struct AlignmentTesterInner {
    XR_ALIGN_PREFIX(32) char c[4] XR_ALIGN_POSTFIX(32);
}
\endcode

Offset Of Exrmple:
\code
struct AlignmentTesterOuter {
    char c[1];
    AlignmentTesterInner test;
};
...
size_t offset = XR_OFFSET_OF(AlignmentTesterOuter, test)
...
\endcode

Defines the following hints to the compiler (where they exist)
\code
XR_INLINE
XR_FORCE_INLINE
XR_NO_INLINE
XR_UNUSED(variable)
XR_HOT
XR_LIKELY
XR_UNLIKELY
XR_RESTRICT_RETURN
XR_NO_RETURN
XR_WARN_IF_RETURN_UNUSED
XR_NO_SIDE_EFFECTS_PREFIX
XR_NO_SIDE_EFFECTS_POSTFIX
XR_PURE_INTERFACE
\endcode

Defines the following utility values
\code
XR_CURRENT_FILE
XR_CURRENT_LINE
XR_CURRENT_FUNCTION
XR_CURRENT_FUNCTION_DECORATED
XR_PREPROCESSOR_JOIN(A, B)
XR_ESCAPE_STRING_HELPER( complex_string )
\endcode

Defines a "halt" primitive. This is not intended to be used directly in normal circumstances.
\code
XR_HALT()
\endcode

Defines the following standard range values:
\code
XR_INT8_MIN
XR_INT8_MAX
XR_INT16_MIN
XR_INT16_MAX
XR_INT32_MIN
XR_INT32_MAX
XR_INT64_MIN
XR_INT64_MAX
XR_INTMAX_MIN
XR_INTMAX_MAX,
XR_UINT8_MIN
XR_UINT8_MAX
XR_UINT16_MIN
XR_UINT16_MAX
XR_UINT32_MIN
XR_UINT32_MAX
XR_UINT64_MIN
XR_UINT64_MAX
XR_UINTMAX_MIN
XR_UINTMAX_MAX,
XR_INTPTR_MIN
XR_INTPTR_MAX
XR_UINTPTR_MIN
XR_UINTPTR_MAX,
XR_SIZE_MIN
XR_SIZE_MAX
\endcode

Defines the following macros (for postfixing proper type specifiers on literals)
\code
XR_INT8_C(literal)
XR_INT16_C(literal)
XR_INT32_C(literal)
XR_INT64_C(literal)
XR_INTMAX_C(literal)
XR_UINT8_C(literal)
XR_UINT16_C(literal)
XR_UINT32_C(literal)
XR_UINT64_C(literal)
XR_UINTMAX_C(literal)
\endcode

Exrmple:
\code
XR_INT16_C(0x1234)
\endcode

Printf format specifiers
\code
XR_INT8_PRINT
XR_INT16_PRINT
XR_INT32_PRINT
XR_INT64_PRINT
XR_UINT8_PRINT
XR_UINT16_PRINT
XR_UINT32_PRINT
XR_UINT64_PRINT
XR_INTMAX_PRINT
XR_UINTMAX_PRINT
XR_INT8_PRINTx
XR_INT16_PRINTx
XR_INT32_PRINTx
XR_INT64_PRINTx
XR_UINT8_PRINTx
XR_UINT16_PRINTx
XR_UINT32_PRINTx
XR_UINT64_PRINTx
XR_INTMAX_PRINTx
XR_UINTMAX_PRINTx
XR_INT8_PRINTX
XR_INT16_PRINTX
XR_INT32_PRINTX
XR_INT64_PRINTX
XR_UINT8_PRINTX
XR_UINT16_PRINTX
XR_UINT32_PRINTX
XR_UINT64_PRINTX
XR_INTMAX_PRINTX
XR_UINTMAX_PRINTX
\endcode

Scanf format specifiers (where available). Note that scanning 8 bit values is
not possible. If you need to scan an 8 bit value you'll need to manually pull
the byte out and typecast it.
\code
//XR_INT8_SCAN     --- Not Possible
XR_INT16_SCAN
XR_INT32_SCAN
XR_INT64_SCAN
//XR_UINT8_SCAN    --- Not Possible
XR_UINT16_SCAN
XR_UINT32_SCAN
XR_UINT64_SCAN
XR_INTMAX_SCAN  XR_INT64_SCAN
XR_UINTMAX_SCAN XR_UINT64_SCAN
//XR_INT8_SCANx     --- Not Possible
XR_INT16_SCANx
XR_INT32_SCANx
XR_INT64_SCANx
//XR_UINT8_SCANx    --- Not Possible
XR_UINT16_SCANx
XR_UINT32_SCANx
XR_UINT64_SCANx
XR_INTMAX_SCANx  XR_INT64_SCANx
XR_UINTMAX_SCANx XR_UINT64_SCANx
//XR_INT8_SCANX   --- Not Possible
XR_INT16_SCANX
XR_INT32_SCANX
XR_INT64_SCANX
//XR_UINT8_SCANX  --- Not Possible
XR_UINT16_SCANX
XR_UINT32_SCANX
XR_UINT64_SCANX
XR_INTMAX_SCANX
XR_UINTMAX_SCANX
\endcode

Exrmple:
\code
XR_LOG_WARNING_FORMATTED(&sSystemLogHandle, "Error from CreateFile %" XR_UINT32_PRINTX "", GetLastError() );
\endcode

Defines the following types (or pulls them from stdint.h)
\code
int8_t; int16_t; int32_t; int64_t; intptr_t; intmax_t
uint8_t; uint16_t; uint32_t; uint64_t; uintptr_t; uintmax_t
size_t
\endcode

\author Daniel Craig \par Copyright 2016, All Rights reserved.
 */
// ######################################################################################### - FILE
// Guard */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#define XR_DEFINES_H

#if defined( _MSC_VER )
#pragma once
#endif

// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_CONFIG_H
#include "xr/config.h"
#endif//XR_CONFIG_H

// ######################################################################################### - FILE
/* Public Macros */
/* All of these are only defined if not provided with more specific versions
   by the config, OS or compiler defines Above
 */
// ######################################################################################### - FILE
// -----------------------------------------------------------------------------------------  MACRO
/// Join macro must be nested to properly support __LINE__ This seems to be common
/// practice in both GCC and MSVC++ code
#ifndef XR_PREPROCESSOR_JOIN
#define XR_PREPROCESSOR_JOIN(a, b) XR_PREPROCESSOR_JOIN_INTERNAL(a,b)
/// Needs to be done recursively to support __LINE__ use
/// \internal
#define XR_PREPROCESSOR_JOIN_INTERNAL(a,b) XR_PREPROCESSOR_JOIN_INTERNAL2(a,b)
/// Needs to be done recursively to support __LINE__ use
/// \internal
#define XR_PREPROCESSOR_JOIN_INTERNAL2(a,b) a##b
#endif//XR_PREPROCESSOR_JOIN
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// Macro to escape a string for you. Great for putting XML in code as well as
/// escaping complicated strings / paths etc
#ifndef XR_ESCAPE_STRING_HELPER
#define XR_ESCAPE_STRING_HELPER(complex_string) #complex_string
#endif//XR_ESCAPE_STRING_HELPER

#ifndef XR_COMPILER_DOXYGEN
// -----------------------------------------------------------------------------------------  MACRO
// Operating system defines
#if defined(_WIN32)
#   include "xr/platform/windows.h"
#elif (defined(__MACH__) && defined( __APPLE__ ))
#   include "xr/platform/osx.h"
#elif defined(__linux__)
#   include "xr/platform/linux.h"
#else
#   error "Unidentified or Unsupported Platform"
#endif// OS
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// Compiler defines
#if defined( _MSC_VER )
#   include "xr/platform/vc.h"
#elif defined( __GNUC__  )
#   include "xr/platform/gcc.h"
#else
#   error "Unidentified or Unsupported Compiler"
#endif// Compiler

// Make sure it's all done right.
#ifndef XR_CPU_NAME
#   error "XR_CPU_NAME should be defined"
#endif
#ifndef XR_PLATFORM_PTR_SIZE
#   error "XR_PLATFORM_PTR_SIZE should be defined"
#endif

#else
//for XR_COMPILER_DOXYGEN, comment macros defined in platform headers

/// This is the name of the CPU, "x86", "x64", "ARM"
#define XR_CPU_NAME "Doxygen"
/// This is the size of pointers on this platform
#define XR_PLATFORM_PTR_SIZE 4
/// This is the name of the current file (typically __FILE__)
#define XR_CURRENT_FILE
/// This is the line in the current file (typically __LINE__)
#define XR_CURRENT_LINE
/// This is the undecorated name of the enclosing function
/// Where supported by the compiler
#define XR_CURRENT_FUNCTION
/// This is the decorated name of the enclosing function (with arguments).
/// Where supported by the compiler
#define XR_CURRENT_FUNCTION_DECORATED
/// Defined on platforms which are little endian
#define XR_PLATFORM_LITTLE_ENDIAN 1
/// Defined on platforms which are big endian
#define XR_PLATFORM_BIG_ENDIAN 1
/// Defines a halt instruction (a debug break where supported)
#define XR_HALT()
//@{
/// Byte swap a value
#define XR_SWAP_UINT16_T()
#define XR_SWAP_UINT32_T()
#define XR_SWAP_UINT64_T()
//@}

#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// \def XR_ALIGN_OF
/// This seeks to see how far it the object gets padded next to an item with 1
/// byte alignment
#ifndef XR_ALIGN_OF
#define XR_ALIGN_OF(objectName) (sizeof( struct {char a; objectName b;} ) - sizeof(objectName))
#endif//XR_ALIGN_OF
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// offset of is notoriously picky on different compilers. This is because non-POD
/// data can in cases not possibly provide a consistent offset. Sadly this prevents
/// use of offset of on a number of objects for which the operation still makes sense.
/// This could use the old trick of &(nullptr)->fieldname, but some compilers complain
/// about the explicit dereference of nullptr... This cheats by adding and subtracting
/// a constant. Not the best, but it works on many compilers.
#ifndef XR_OFFSET_OF
#define XR_OFFSET_OF(objectName, fieldName) ( \
    ((size_t)(& (  ((objectName*)(0x10))->fieldName     ) )-0x10 ))
#endif//XR_OFFSET_OF
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/*! Conversion from specific endianness to native endianness. */
// -----------------------------------------------------------------------------------------  MACRO
#if defined(XR_PLATFORM_LITTLE_ENDIAN)

#define XR_LITTLE_ENDIAN_TO_NATIVE_UINT16_T(value) (value)
#define XR_LITTLE_ENDIAN_TO_NATIVE_UINT32_T(value) (value)
#define XR_LITTLE_ENDIAN_TO_NATIVE_UINT64_T(value) (value)
#define XR_BIG_ENDIAN_TO_NATIVE_UINT16_T(value)    XR_SWAP_UINT16_T(value)
#define XR_BIG_ENDIAN_TO_NATIVE_UINT32_T(value)    XR_SWAP_UINT32_T(value)
#define XR_BIG_ENDIAN_TO_NATIVE_UINT64_T(value)    XR_SWAP_UINT64_T(value)

#define XR_NATIVE_TO_LITTLE_ENDIAN_UINT16_T(value) (value)
#define XR_NATIVE_TO_LITTLE_ENDIAN_UINT32_T(value) (value)
#define XR_NATIVE_TO_LITTLE_ENDIAN_UINT64_T(value) (value)
#define XR_NATIVE_TO_BIG_ENDIAN_UINT16_T(value)    XR_SWAP_UINT16_T(value)
#define XR_NATIVE_TO_BIG_ENDIAN_UINT32_T(value)    XR_SWAP_UINT32_T(value)
#define XR_NATIVE_TO_BIG_ENDIAN_UINT64_T(value)    XR_SWAP_UINT64_T(value)

#elif defined(XR_PLATFORM_BIG_ENDIAN)

#define XR_LITTLE_ENDIAN_TO_NATIVE_UINT16_T(value) XR_SWAP_UINT16_T(value)
#define XR_LITTLE_ENDIAN_TO_NATIVE_UINT32_T(value) XR_SWAP_UINT32_T(value)
#define XR_LITTLE_ENDIAN_TO_NATIVE_UINT64_T(value) XR_SWAP_UINT64_T(value)
#define XR_BIG_ENDIAN_TO_NATIVE_UINT16_T(value)   (value)
#define XR_BIG_ENDIAN_TO_NATIVE_UINT32_T(value)   (value)
#define XR_BIG_ENDIAN_TO_NATIVE_UINT64_T(value)   (value)

#define XR_NATIVE_TO_LITTLE_ENDIAN_UINT16_T(value) XR_SWAP_UINT16_T(value)
#define XR_NATIVE_TO_LITTLE_ENDIAN_UINT32_T(value) XR_SWAP_UINT32_T(value)
#define XR_NATIVE_TO_LITTLE_ENDIAN_UINT64_T(value) XR_SWAP_UINT64_T(value)
#define XR_NATIVE_TO_BIG_ENDIAN_UINT16_T(value)    (value)
#define XR_NATIVE_TO_BIG_ENDIAN_UINT32_T(value)    (value)
#define XR_NATIVE_TO_BIG_ENDIAN_UINT64_T(value)    (value)

#else

#error "Error: Endianness not defined for this platform!?"
#endif

#ifndef XR_WCHAR_IS_NATIVE
#error "Need to know more about the platform"
#endif
#ifndef XR_WCHAR_SIZE
#error "Need to know more about the platform"
#endif

//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_WARN_IF_RETURN_UNUSED
/// Attribute that if available on this compiler will tell warn the
/// programmer if an important return value is un-checked. Typically
/// such unused return values create a leak.
/// This must be on a prototype, not the definition.
/// exrmple:
///  int foo() XR_WARN_IF_RETURN_UNUSED;
#define XR_WARN_IF_RETURN_UNUSED
#endif//XR_WARN_IF_RETURN_UNUSED
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_UNUSED
/// Macro to tell the compiler that the passed variable is
/// intentionally unused this prevents warnings
#define XR_UNUSED(variable) ( (void)variable )
#endif//XR_UNUSED
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_NO_SIDE_EFFECTS_PREFIX
/// This macro tells the compiler that a given function has no side
/// effects and can be subject to sub expression elimination
/// usage "XR_NO_SIDE_EFFECTS_PREFIX int blah(x) XR_NO_SIDE_EFFECTS_POSTFIX;"
#define XR_NO_SIDE_EFFECTS_PREFIX
#endif//XR_NO_SIDE_EFFECTS_PREFIX
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_NO_SIDE_EFFECTS_POSTFIX
/// This macro tells the compiler that a given function has no side
/// effects and can be subject to sub expression elimination
/// usage "XR_NO_SIDE_EFFECTS_PREFIX int blah(x) XR_NO_SIDE_EFFECTS_POSTFIX;"
#define XR_NO_SIDE_EFFECTS_POSTFIX
#endif//XR_NO_SIDE_EFFECTS_POSTFIX
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// simple inline wrapper (for consistency)
#ifndef XR_INLINE
#define XR_INLINE           inline
#endif//XR_INLINE
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// Forced Inline (where compiler supported)
#ifndef XR_FORCE_INLINE
#define XR_FORCE_INLINE     inline
#endif//XR_FORCE_INLINE
/// inline disabled (where compiler supported)
#ifndef XR_NO_INLINE
#define XR_NO_INLINE
#endif//XR_NO_INLINE
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_HOT
/// This function is called a lot, or is the typical target of a
/// virtual function call.
/// usage "int blah(x) XR_HOT;"
#define XR_HOT
#endif//XR_HOT
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_OVERRIDE
/// This function should override a base class function, if it does not
/// then trigger a warning. Usage:
/// \code
/// struct A { virtual void foo() const; };
/// struct B : public A { void foo() XR_OVERRIDE; };
/// \endcode
#define XR_OVERRIDE
#endif//XR_OVERRIDE
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// Denote a virtual function implementation as non overridable.
/// struct A { virtual void foo() const; };
/// struct B : public A { void foo() XR_FINAL; };
/// struct C : public B { void foo(); };  <-- Error
#ifndef XR_FINAL
#define XR_FINAL
#endif//XR_FINAL
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_LIKELY
/// compiler should optimize for the case where (x) equates to true
/// usage "if( XR_LIKELY( x > 5 )) {  }"
#define XR_LIKELY(x)   (x)
#endif//XR_LIKELY
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_UNLIKELY
/// compiler should optimize for the case where (x) equates to false
/// usage "if( XR_UNLIKELY( x > 5 )) {  }"
#define XR_UNLIKELY(x) (x)
#endif//XR_UNLIKELY
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_PURE_INTERFACE
/// This object cannot be created individually and will not have a
/// vtable specific to the base type.
#define XR_PURE_INTERFACE
#endif//XR_PURE_INTERFACE
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_RESTRICT_RETURN
/// This normally tells the compiler that the decorated function
/// returns a pointer that will not alias any existing pointers.
/// This is usually applied to the malloc function, but applies in
/// many cases.
#define XR_RESTRICT_RETURN
#endif//XR_RESTRICT_RETURN
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_NO_RETURN
/// This tells the compiler that this function does not return, and that
/// it should not throw warnings about lack of future code (return
/// value, etc).
#define XR_NO_RETURN
#endif//XR_NO_RETURN
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_DISABLE_ALL_WARNINGS
/// This is required in visual studio to suppress warnings in system
/// headers.
#define XR_DISABLE_ALL_WARNINGS()
#endif//XR_DISABLE_ALL_WARNINGS()
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_RESTORE_ALL_WARNINGS
/// This is required in visual studio to suppress warnings in system headers.
#define XR_RESTORE_ALL_WARNINGS()
#endif//XR_RESTORE_ALL_WARNINGS
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_ANALYSIS_ASSUME
/// This is required in visual studio to suppress warnings in system
/// headers.
#define XR_ANALYSIS_ASSUME(expr)
#endif//XR_ANALYSIS_ASSUME()
//----------------------------------------------------------------------------
/// Helper function to deal with wchar_t literals. Note that wchar_t is of
/// variable size per platform (16 bit on windows, 32 bit on unix). In
/// addition technically wchar_t is UTF-16 on windows, (not UCS 2) so a
/// character can take up 2 UTF-16 characters. It is a mess but generally not
/// of specific concern (other then where the distinction between character
/// count and byte count differ).
//----------------------------------------------------------------------------
#ifndef XR_STR_WCHAR_T
#define XR_STR_WCHAR_T(str) L##str
#endif
#ifndef XR_STR_CHAR16_T
#define XR_STR_CHAR16_T(str) (u##str)
#endif
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
/// This is a helper for analysis applications.
/// IN = Input parameter (must not be nullptr)
/// IN_OPTIONAL = input parameter (may be nullptr)
/// OUT = output parameter (must not be nullptr)
/// _STRING = should be zero terminated string
/// _COUNT = number of elements (not bytes)
//@{
#ifndef XR_IN
#define XR_IN
#endif//XR_IN
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_IN_COUNT
#define XR_IN_COUNT(elementCount)
#endif//XR_IN_COUNT
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_IN_BYTES
#define XR_IN_BYTES(byteCount)
#endif//XR_IN_BYTES
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_IN_STRING
#define XR_IN_STRING
#endif//XR_IN_STRING
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_OUT
#define XR_OUT
#endif//XR_OUT
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_OUT_COUNT
#define XR_OUT_COUNT(elementCount)
#endif//XR_OUT_COUNT
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_OUT_BYTES
#define XR_OUT_BYTES(byteCount)
#endif//XR_OUT_BYTES
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_OUT_STRING
#define XR_OUT_STRING(sizeInChars)
#endif//XR_OUT_STRING
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_OUT_INVALIDATE
#define XR_OUT_INVALIDATE
#endif//XR_OUT_INVALIDATE
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_IN_OPTIONAL
#define XR_IN_OPTIONAL
#endif//XR_IN_OPTIONAL
//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_IN_OPTIONAL_STRING
#define XR_IN_OPTIONAL_STRING
#endif//XR_IN_OPTIONAL_STRING
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_RETURN_COUNT
#define XR_RETURN_COUNT(elementCount)
#endif//XR_RETURN_COUNT
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_RETURN_BYTES
#define XR_RETURN_BYTES(byteCount)
#endif//XR_RETURN_BYTES
// -----------------------------------------------------------------------------------------  MACRO
/// If 1, then constexpr is supported
// -----------------------------------------------------------------------------------------  MACRO
#ifndef XR_COMPILER_SUPPORTS_CONSTEXPR
#define XR_COMPILER_SUPPORTS_CONSTEXPR 0
#endif//XR_COMPILER_SUPPORTS_CONSTEXPR
#if XR_COMPILER_SUPPORTS_CONSTEXPR
#   define XR_CONSTEXPR constexpr
#else
#   define XR_CONSTEXPR
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/// Utility for getting the compiler to print out a value at compile time.
/// Note that this intentionally fails the build. The provided implementation
/// will result in a "constructor is private" error, but the template
/// parameter "Result" will also be printed.
/// Exrmple: XR_COMPILE_TIME_PRINT( sizeof(MyObject) );
/*! Exrmple output:
error C2248: 'internal::ValuePrinter<Result>::ValuePrinter' : cannot access private member declared in class 'internal::ValuePrinter<Result>'
with
[
    Result=16
]
*/
//@{
#ifndef XR_COMPILE_TIME_PRINT
#define XR_COMPILE_TIME_PRINT(value) ::internal::ValuePrinter< value > XR_PREPROCESSOR_JOIN(CompileTimePrint_,__LINE__)
namespace internal{
    template<size_t Result>
    class ValuePrinter
    {
    private:
        ValuePrinter();
    };
}
//@}
#endif//XR_COMPILE_TIME_SIZEOF


//----------------------------------------------------------------------------
/// Allow easy modification of these. (sadly some systems duplicate __FILE__ string)
//----------------------------------------------------------------------------
#ifndef XR_FILE_LINE
#define XR_FILE_LINE XR_CURRENT_FILE, XR_CURRENT_LINE
#endif
//----------------------------------------------------------------------------
/// Macro to help with alignment
//----------------------------------------------------------------------------
//@{
#ifndef XR_ALIGN_PREFIX
#define XR_ALIGN_PREFIX( _align )
#endif
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#ifndef XR_ALIGN_POSTFIX
#define XR_ALIGN_POSTFIX( _align )
#endif
//@}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#ifndef XR_PUSH_SUPRESS_MSVC_WARNINGS
#define XR_PUSH_SUPRESS_MSVC_WARNINGS(xxx)
#endif
#ifndef XR_POP_SUPRESS_MSVC_WARNINGS
#define XR_POP_SUPRESS_MSVC_WARNINGS()
#endif

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE

namespace xr { namespace Core {
// ***************************************************************************************** - TYPE
/*! Standard Result for Success / Failure. Situations with more return types
    should define a context specific result type.
*/
// ***************************************************************************************** - TYPE
enum StandardResult
{
    kSuccess = 0,
    kFailure,
};
// --------------------------------------------------------------------------------------  FUNCTION
/// returns true if there is a debugger known to be connected.
/// note that this might not be possible to know on all platforms
// --------------------------------------------------------------------------------------  FUNCTION
bool IsDebuggerConnected();

}}// namespace

#endif //#ifndef XR_DEFINES_H
