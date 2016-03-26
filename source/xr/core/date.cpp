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
#ifndef XR_CORE_DATE_H
#include "xr/core/date.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif

#if XR_COMPILER_MINGW
// Mingw is being difficult and will not expose *_s functions unless you define
// this. (We use the _s functions on windows and _r functions on unix to
// get thread safe variants of the functions.
#define MINGW_HAS_SECURE_API 1
#endif

#include <time.h>
// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
DateStamp Now()
{
    ::time_t ds;
    time(&ds);
    return (DateStamp)ds;
}

#if defined(XR_PLATFORM_WINDOWS)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void ToString(DatePrintType type,
                char * outString,
                size_t outStringSizeChars,
                const char * format,
                DateStamp ds)
{
    tm t;
    if(type == kDateUTC)
    {
        _gmtime64_s(&t, &ds);
    }
    else if(type == kDateLocal)
    {
        _localtime64_s(&t, &ds);
    }
    else
    {
        XR_DEBUG_FAIL();
		if(outStringSizeChars > 0)
		{
			outString[0] = '\0';
		}
		return;
    }

    strftime(outString, outStringSizeChars, format, &t);

}
#else
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void ToString(DatePrintType type,
                    char * outString,
                    size_t outStringSizeChars,
                    const char * format,
                    DateStamp d64)
{
    // in OSX this is 32 bits, perhaps also linux, not sure.
    time_t ds = (time_t)d64;
    struct tm t;
    if(type == kDateUTC)
    {
        gmtime_r(&ds, &t);
    }
    else if(type == kDateLocal)
    {
        localtime_r(&ds, &t);
    }
    else
    {
        XR_DEBUG_FAIL();
		if(outStringSizeChars > 0)
		{
			outString[0] = '\0';
		}
		return;
    }

    strftime(outString, outStringSizeChars, format, &t);

}

#endif

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
