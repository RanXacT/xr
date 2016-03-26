// ######################################################################################### - FILE
/*!
    \page coretime Timing Subsystem
    Time stamps are useful for measuring duration, but cannot be converted to
    actual time / date. Note that drift can occur on time stamps. If you want
    actual date / time, or more deterministic duration for long values, use
    the Date functions which are accurate to seconds (as opposed to
    microseconds)

 \p Overview
    In general this is the highest resolution timer available on the system,
    it is expected to be (although not guaranteed to be) valid to micro
    second resolution. These timings should be used statistically with
    large sample sizes given the drift and the issues with hibernation
    multi-cpu synchronization and potentially other platform specific issues.

    \li \ref xr::Time

    \code
    xr::Time::TimeStamp tsBegin = xr::Time::GetTimeStamp();
    // Do Some work
    xr::Time::TimeStamp tsEnd = xr::Time::GetTimeStamp();

    int64_t uSeconds = xr::Time::TimeStampToMicroSeconds(tsBegin, tsEnd);
    \endcode


    \file
    \copydoc coretime

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_TIME_H
#define XR_CORE_TIME_H

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
namespace xr { namespace Core {
    // ***************************************************************************************** - TYPE
    /// Will not wrap in our lifetime.
    // ***************************************************************************************** - TYPE
    typedef int64_t TimeStamp;
    // --------------------------------------------------------------------------------------  FUNCTION
    /// Return a TimeStamp for use with provided time functions
    // --------------------------------------------------------------------------------------  FUNCTION
    TimeStamp GetTimeStamp();
    // --------------------------------------------------------------------------------------  FUNCTION
    /// Return a int64_t with elapsed time truncated to microseconds
    // --------------------------------------------------------------------------------------  FUNCTION
    int64_t TimeStampToMicroSeconds(TimeStamp timeStamp);
    // --------------------------------------------------------------------------------------  FUNCTION
    /// Return a int64_t with elapsed time truncated to milliseconds
    // --------------------------------------------------------------------------------------  FUNCTION
    int64_t TimeStampToMilliSeconds(TimeStamp timeStamp);
    // --------------------------------------------------------------------------------------  FUNCTION
    /// Return a double with elapsed time in seconds. Use this where
    /// both high precision and no rollover support is required. (but is slower)
    // --------------------------------------------------------------------------------------  FUNCTION
    double   TimeStampToSeconds(TimeStamp timeStampend);

}}
#endif //#ifndef XR_CORE_TIME_H
