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
#ifndef XR_CORE_TIME_H
#include "xr/core/time.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif

#if defined(XR_PLATFORM_WINDOWS)
XR_DISABLE_ALL_WARNINGS()
#include <windows.h>
XR_RESTORE_ALL_WARNINGS()
#elif defined(XR_PLATFORM_DARWIN)
#include <mach/mach_time.h>
#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

#if defined(XR_PLATFORM_WINDOWS)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int64_t GetFrequency()
{
    LARGE_INTEGER li;
    if(QueryPerformanceFrequency(&li) == 0)
    {
        XR_ALWAYS_UNEXPECTED_FM( "QueryPerformanceCounter failed %lx", GetLastError());
        li.QuadPart = 0;
    }

    return li.QuadPart;
}

// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
class FrequencyHelper{
public:
    FrequencyHelper() : timeBaseFrequency_i64(GetFrequency()), timeBaseFrequency_double( double(timeBaseFrequency_i64) )
    {
    }

    const int64_t timeBaseFrequency_i64;
    const double  timeBaseFrequency_double;
private:
    FrequencyHelper & operator = (FrequencyHelper other);
};

#if defined(XR_COMPILER_MICROSOFT)
__pragma(warning(push))
__pragma(warning(disable: 4075)) //  initializers put in unrecognized initialization area
#pragma init_seg( ".CRT$XCB" )
static FrequencyHelper Initializer;
__pragma(warning(pop))
#elif defined(XR_COMPILER_GCC)
static FrequencyHelper __attribute__((init_priority (101))) Initializer;
#else
FrequencyHelper Initializer;
#endif

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
TimeStamp GetTimeStamp()
{
    LARGE_INTEGER li;

    if(QueryPerformanceCounter(&li) == 0)
    {
        XR_ALWAYS_UNEXPECTED_FM( "QueryPerformanceCounter failed %lx", GetLastError());
        li.QuadPart = 0;
    }

    return li.QuadPart;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int64_t TimeStampToMicroSeconds(TimeStamp timeStamp)
{
    return (timeStamp * 1000000) / Initializer.timeBaseFrequency_i64;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int64_t TimeStampToMilliSeconds(TimeStamp timeStamp)
{
    return (timeStamp * 1000) / Initializer.timeBaseFrequency_i64;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
double TimeStampToSeconds(TimeStamp timeStamp)
{
    return double(timeStamp) / Initializer.timeBaseFrequency_double;
}

#elif defined(XR_PLATFORM_DARWIN)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
TimeStamp GetTimeStamp()
{
    return mach_absolute_time();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int64_t TimeStampToMicroSeconds(TimeStamp timeStamp)
{
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);

    // basic equation results in nano seconds.
    return ((int64_t(timeStamp) * info.numer)) / (info.denom * 1000);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int64_t TimeStampToMilliSeconds(TimeStamp timeStamp)
{
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);

    // basic equation results in milli seconds.
    return ((int64_t(timeStamp) * info.numer)) / (info.denom * 1000000);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
double TimeStampToSeconds(TimeStamp timeStamp)
{
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);

    // basic equation results in nano seconds.
    double nanoSeconds = double(timeStamp) *double(info.numer) / double(info.denom);

    return nanoSeconds / 1000000000.0;
}

#else
// Linux might use sys_clock_gettime(CLOCK_MONOTONIC, tp), or CLOCK_MONOTONIC_HR
// Could also use gettimeofday(&time, nullptr); (if CLOCK_MONOTONIC isn't good)
#   error "Need to implement TimeStamp on new platform."

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
TimeStamp GetTimeStamp()
{
    return 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int64_t TimeStampToMicroSeconds(TimeStamp timeStamp)
{
    return 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
int64_t TimeStampToMilliSeconds(TimeStamp timeStamp)
{
    return 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
double TimeStampToSeconds(TimeStamp timeStamp)
{
    return 0.0;
}

#endif

}}//namespace xr::Core

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
