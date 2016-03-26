// ######################################################################################### - FILE
/*! \file


\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_STRING_UTILS_H
#define XR_CORE_STRING_UTILS_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

#include <stdarg.h>

#if defined(XR_COMPILER_MICROSOFT)
extern "C" {
#if _MSC_VER >= 1700
    extern _Check_return_ int    __cdecl strcmp(_In_z_ const char *,_In_z_ const char *);
    extern _Check_return_ size_t __cdecl strlen(_In_z_ const char *);
#else
    extern int    __cdecl strcmp(const char *,const char *);
    extern size_t __cdecl strlen(const char *);
#endif
#if !defined(XR_COMPILER_INTEL)
    // Tell the compiler to make these intrinsics (or else they will be function calls)
#   pragma intrinsic(strcmp)
#   pragma intrinsic(strlen)
#endif
}
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
namespace xr { namespace Core {
// --------------------------------------------------------------------------------------  FUNCTION
//@{
/**
* Non standard vsnprintf variant which guarantees a null character
* except if passed size is 0.
*
* @param buf The buffer to write to
* @param sizeInChars The size of the buffer in characters (not bytes!)
* @param format The format string
* @param ap The arguments to use to fill out the format string.
* @return true=string fully printed, false = string truncated
*/
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult VStringPrintf(
    XR_OUT_STRING(sizeInChars) char *str,
    size_t sizeInChars,
    XR_IN_STRING const char *format,
    va_list ap);
StandardResult VStringPrintf(
    XR_OUT_STRING(sizeInChars) wchar_t *str,
    size_t sizeInChars,
    XR_IN_STRING const wchar_t *format,
    va_list ap);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
//@{
/**
* Non standard snprintf variant which guarantees a null character
* except if passed size is 0.
*
* @param buf The buffer to write to
* @param sizeInChars The size of the buffer in characters (not bytes!)
* @param format The format string
* @param format arguments
* @return true=string fully printed, false = string truncated
*/
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult StringPrintf(
    XR_OUT_STRING(sizeInChars) char *str,
    size_t sizeInChars,
    XR_IN_STRING const char *format,
    ...);
StandardResult StringPrintf(
    XR_OUT_STRING(sizeInChars) wchar_t *str,
    size_t sizeInChars,
    XR_IN_STRING const wchar_t *format,
    ...);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief StringCompareNoCase wrapper to abstract naming differences per platform.
    http://msdn.microsoft.com/en-us/library/ms235365.aspx
*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompareNoCase(XR_IN_STRING const char *str1, XR_IN_STRING const char * str2) XR_NO_SIDE_EFFECTS_POSTFIX;
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompareNoCase(XR_IN_STRING const wchar_t *str1, XR_IN_STRING const wchar_t * str2) XR_NO_SIDE_EFFECTS_POSTFIX;
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief String Compare function.

    Wraps strcmp and the native wide char function, generally wcscmp
    \sa http://www.cplusplus.com/reference/clibrary/cstring/strcmp/
    \sa http://en.wikibooks.org/wiki/C_Programming/C_Reference/wchar.h/wcscmp

*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompare(
    XR_IN_STRING const char *dest,
    XR_IN_STRING const char * src)XR_NO_SIDE_EFFECTS_POSTFIX;
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompare(
    XR_IN_STRING const wchar_t *dest,
    XR_IN_STRING const wchar_t * src)XR_NO_SIDE_EFFECTS_POSTFIX;
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief truncating string copy function.

    Similar to strncpy, but ensures the dest string is null
    terminated (which is not guaranteed by the c library function).
    \sa http://www.cplusplus.com/reference/clibrary/cstring/strncpy/
*/
// --------------------------------------------------------------------------------------  FUNCTION
//@{
template <size_t elemCount>
inline void StringCopy(
    XR_OUT_STRING(elemCount) char (&dst)[elemCount],
    XR_IN_STRING const char * src);

template <size_t elemCount>
inline void StringCopy(
    XR_OUT_STRING(elemCount) wchar_t (&dst)[elemCount],
    XR_IN_STRING const wchar_t * src);

void StringCopy(
    XR_OUT_STRING(destSizeInChars) char *dest,
    XR_IN_STRING const char * src,
    size_t destSizeInChars);
void StringCopy(
    XR_OUT_STRING(destSizeInChars) wchar_t *dest,
    XR_IN_STRING const wchar_t * src,
    size_t destSizeInChars);
//@}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Simple function to parse a bool from a string */
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
bool StringToBool(XR_IN_STRING const char * str, bool defaultValue) XR_NO_SIDE_EFFECTS_POSTFIX;

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Simple function to parse a bool from a string */
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
size_t StringToSizet(XR_IN_STRING const char * str, size_t defaultValue) XR_NO_SIDE_EFFECTS_POSTFIX;
// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Simple function to return the number of characters including the null character */
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
size_t StringLengthWithNull(XR_IN_STRING const char * str) XR_NO_SIDE_EFFECTS_POSTFIX;
// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Simple function to return the number of characters including the null character */
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
size_t StringLengthWithNull(XR_IN_STRING const wchar_t * str) XR_NO_SIDE_EFFECTS_POSTFIX;


// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template <size_t elemCount>
void StringCopy(XR_OUT_STRING(elemCount) char (&dest)[elemCount], const char * src)
{
    StringCopy(dest, src, elemCount);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template <size_t elemCount>
void StringCopy(XR_OUT_STRING(elemCount) wchar_t (&dest)[elemCount], const wchar_t * src)
{
    StringCopy(dest, src, elemCount);
}



#if defined(XR_COMPILER_MICROSOFT)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE
XR_NO_SIDE_EFFECTS_PREFIX
size_t StringLengthWithNull( XR_IN_STRING const char * str )
{
    // strlen does not include nullptr terminator
    // http://en.wikipedia.org/wiki/Strlen
    return ::strlen(str) + 1;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_FORCE_INLINE
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompare(XR_IN_STRING const char *str1, XR_IN_STRING const char * str2)
{
    return ::strcmp(str1, str2);
}
#endif

// ***************************************************************************************** - TYPE
// Functor for String Equality (useful for stl/sort functions)
// ***************************************************************************************** - TYPE
struct StringEqualsFunctor
{
    bool operator()(const char* a, const char * b) const
    {
        return xr::Core::StringCompare(a,b) == 0;
    }
};

// ***************************************************************************************** - TYPE
// Functor for (Simple) String Hashing (useful for stl/sort functions)
// ***************************************************************************************** - TYPE
struct StringHashFunctorFnv
{
    size_t operator()(const char* p) const
    {
        size_t c, result = 2166136261U;  // FNV1 hash. Perhaps the best string hash.
        while((c = (uint8_t)*p++) != 0)  // Using '!=' disables compiler warnings.
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};

#if defined(XR_COMPILER_GCC)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline
XR_NO_SIDE_EFFECTS_PREFIX
size_t StringLengthWithNull( XR_IN_STRING const char * str )
{
    // strlen does not include nullptr terminator
    // http://en.wikipedia.org/wiki/Strlen
    return __builtin_strlen(str) + 1;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompare(XR_IN_STRING const char *str1, XR_IN_STRING const char * str2)
{
    return __builtin_strcmp(str1, str2);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline
void StringCopy( XR_OUT_STRING(destSizeInChars) char *dest, XR_IN_STRING const char * src, size_t destSizeInChars )
{
    // This needlessly fills the remaining dest buffer with 0's
    // as per C spec.
    __builtin_strncpy(dest, src, destSizeInChars);
    // but it won't guarantee a nullptr terminator... also as per
    // C spec.
    dest[destSizeInChars-1] = 0;
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompareNoCase(XR_IN_STRING const char *str1, XR_IN_STRING const char * str2)
{
    return __builtin_strcasecmp(str1, str2);
}
#endif
}}
#endif //#ifndef XR_CORE_STRING_UTILS_H
