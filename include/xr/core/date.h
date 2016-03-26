// ######################################################################################### - FILE
/*!
    \page coredate Time / Date Subsystem
    The Date subsystem manages seconds level time / date functions. These should
    have any issues with drift, etc that higher frequency timers often run into.

    \file
    \copydoc coredate

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_DATE_H
#define XR_CORE_DATE_H

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
/// Units not defined platform independently. No timezone included.
/// Use date functions to digest into human readable format.
// ***************************************************************************************** - TYPE
typedef int64_t DateStamp;

// --------------------------------------------------------------------------------------  FUNCTION
/// Return a TimeStamp for use with provided time functions
// --------------------------------------------------------------------------------------  FUNCTION
DateStamp Now();

// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
enum DatePrintType {
    kDateUTC,  ///< This returns GMT / UTC time.
    kDateLocal ///< This uses local time including daylight savings etc.
};

// --------------------------------------------------------------------------------------  FUNCTION
/// This creates a string for the passed time using a standard
/// strftime format string. This is intended for logging not for user
/// display (which would require a localized representation).
/// \sa http://www.cplusplus.com/reference/clibrary/ctime/strftime/
// --------------------------------------------------------------------------------------  FUNCTION
void ToString(DatePrintType type,
                char * outString,
                size_t outStringSizeChars,
                const char * format,
                DateStamp ds)
#if defined(XR_COMPILER_GCC)
                __attribute__ ((format (strftime, 4, 0)))
#endif
                ;

/*-------------------------------------------------------------------*/
// TODO: Create wide char function that prints locale correct date /
// time which allows date, time or both to be printed
// impl should use wcsftime along with %c, %x and %X.
/*-------------------------------------------------------------------*/


}}
#endif //#ifndef XR_CORE_DATE_H
