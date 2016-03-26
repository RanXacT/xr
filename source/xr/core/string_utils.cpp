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
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#include <stdio.h>  // _vsnprintf_s
#include <string.h>
#include <stdlib.h> // atol
#include <wchar.h>

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult VStringPrintf(
    XR_OUT_STRING(sizeInChars) char *str,
    size_t sizeInChars,
    XR_IN_STRING const char *format,
    va_list ap)
{
    if(sizeInChars == 0)
    {
        return kFailure;
    }

#if defined(XR_COMPILER_MICROSOFT)
    // This function ensure the string is nullptr terminated.
    int chars = ::_vsnprintf_s(str, sizeInChars, _TRUNCATE, format, ap) >= 0;
    return chars <= (int)sizeInChars && chars > 0 ? kSuccess : kFailure;
#elif defined(XR_COMPILER_GCC)
    // In the case of overflow the string is not terminated!
    int chars = ::vsnprintf(str, sizeInChars, format, ap);

    if( chars >= (int)sizeInChars || chars < 0)
    {
        str[sizeInChars-1] = '\0';
        return kFailure;
    }

    return kSuccess;
#else
#			error "Need to find non-standard function \"vsnprintf\" on new compiler"
#endif
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult VStringPrintf(
    XR_OUT_STRING(sizeInChars) wchar_t *str,
    size_t sizeInChars,
    XR_IN_STRING const wchar_t *format,
    va_list ap)
{
    if(sizeInChars == 0)
    {
        return kFailure;
    }

#if defined(XR_COMPILER_MICROSOFT)
    int chars = ::_vsnwprintf_s(str, sizeInChars, _TRUNCATE, format, ap);
    return chars <= (int)sizeInChars && chars >= 0 ? kSuccess : kFailure;
#elif defined(XR_PLATFORM_DARWIN)
    int chars = ::vswprintf(str, sizeInChars, format, ap);

    if( chars >= (int)sizeInChars || chars < 0)
    {
        str[sizeInChars-1] = L'\0';
        return kFailure;
    }
    return true;
#elif defined(XR_COMPILER_GCC)
    int chars = ::vsnwprintf(str, sizeInChars, format, ap);

    if( chars >= (int)sizeInChars || chars < 0)
    {
        str[sizeInChars-1] = L'\0';
        return kFailure;
    }

    return kSuccess;
#else
#			error "Need to find non-standard function \"vsnprintf\" on new compiler"
#endif
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult StringPrintf(
    XR_OUT_STRING(sizeInChars) char *str,
    size_t sizeInChars,
    XR_IN_STRING const char *format,
    ...)
{
    va_list ap;
    va_start(ap,format);
    StandardResult retVal = ::xr::Core::VStringPrintf(str, sizeInChars, format, ap);
    va_end(ap);
    return retVal;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult StringPrintf(
    XR_OUT_STRING(sizeInChars) wchar_t *str,
    size_t sizeInChars,
    XR_IN_STRING const wchar_t *format,
    ...)
{
    va_list ap;
    va_start(ap,format);
    StandardResult retVal = ::xr::Core::VStringPrintf(str, sizeInChars, format, ap);
    va_end(ap);
    return retVal;
}
#if !defined(XR_COMPILER_GCC)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompareNoCase(XR_IN_STRING const char *str1, XR_IN_STRING const char * str2)
{
#if defined(XR_COMPILER_MICROSOFT)
    return _stricmp(str1, str2);
#else
#           error "Need to find non-standard function \"StringCompareNoCase\" on new compiler"
#endif
}
#endif
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompareNoCase(XR_IN_STRING const wchar_t *str1, XR_IN_STRING const wchar_t * str2)
{
#if defined(XR_COMPILER_MICROSOFT)
    return _wcsicmp(str1, str2);
#elif defined(XR_COMPILER_GCC)
    return _wcsicmp(str1, str2);
#else
#           error "Need to find non-standard function \"StringCompareNoCase\" on new compiler"
#endif
}

#if !defined(XR_COMPILER_MICROSOFT) && !defined(XR_COMPILER_GCC)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompare(XR_IN_STRING const char *str1, XR_IN_STRING const char * str2)
{
    return ::strcmp(str1, str2);
}
#endif

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
int StringCompare(XR_IN_STRING const wchar_t *str1, XR_IN_STRING const wchar_t * str2)
{
    return wcscmp(str1, str2);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
bool StringToBool(XR_IN_STRING const char * str, bool defaultValue)
{
    if(str == nullptr || str[0] == '\0')
    {
        return defaultValue;
    }
    else if(xr::Core::StringCompareNoCase(str,"true") == 0)
    {
        return true;
    }
    else if(xr::Core::StringCompareNoCase(str,"false") == 0)
    {
        return false;
    }
    else if(xr::Core::StringCompareNoCase(str,"1") == 0)
    {
        return true;
    }
    if(xr::Core::StringCompareNoCase(str,"0") == 0)
    {
        return false;
    }
    else if(xr::Core::StringCompareNoCase(str,"yes") == 0)
    {
        return true;
    }
    else if(xr::Core::StringCompareNoCase(str,"no") == 0)
    {
        return false;
    }

    return defaultValue;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
size_t StringToSizet(XR_IN_STRING const char * str, size_t defaultValue)
{
    size_t temp = defaultValue;
    if(str != nullptr && str[0] != '\0')
    {
        temp = size_t(atol(str));
    }
    return temp;
}
#if !defined(XR_COMPILER_MICROSOFT) && !defined(XR_COMPILER_GCC)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
size_t StringLengthWithNull( XR_IN_STRING const char * str )
{
    // strlen does not include nullptr terminator
    // http://en.wikipedia.org/wiki/Strlen
    return strlen(str) + 1;
}
#endif
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
size_t StringLengthWithNull( XR_IN_STRING const wchar_t * str )
{
    // strlen does not include nullptr terminator
    // http://en.wikipedia.org/wiki/Strlen
    return wcslen(str) + 1;
}
#if !defined(XR_COMPILER_GCC)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void StringCopy( XR_OUT_STRING(destSizeInChars) char *dest, XR_IN_STRING const char * src, size_t destSizeInChars )
{
#if defined(XR_COMPILER_MICROSOFT)
    strncpy_s(dest, destSizeInChars, src, _TRUNCATE);
#else
    // This needlessly fills the remaining dest buffer with 0's
    // as per C spec.
    strncpy(dest, src, destSizeInChars);
    // but it won't guarantee a nullptr terminator... also as per
    // C spec.
    dest[destSizeInChars-1] = 0;
#endif
}
#endif
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void StringCopy( XR_OUT_STRING(destSizeInChars) wchar_t *dest, XR_IN_STRING const wchar_t * src, size_t destSizeInChars )
{
#if defined(XR_COMPILER_MICROSOFT)
    wcsncpy_s(dest, destSizeInChars, src, _TRUNCATE);
#else
    // This needlessly fills the remaining dest buffer with 0's
    // as per C spec.
    wcsncpy(dest, src, destSizeInChars);
    // but it won't guarantee a nullptr terminator... also as per
    // C spec.
    dest[destSizeInChars-1] = 0;
#endif
}
}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
