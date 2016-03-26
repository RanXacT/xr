// ######################################################################################### - FILE
/*! \file
    Basic Console interaction (printf Colored text)

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_CONSOLE_H
#define XR_CORE_CONSOLE_H

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
// For printf annotation
#include <sal.h>
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
// ***************************************************************************************** - TYPE
enum ConsoleFile {
    kConsoleStdOut,
    kConsoleStdErr,
};

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Print to console with passed parameters
    */
// --------------------------------------------------------------------------------------  FUNCTION
void ConsolePrintf(
    ConsoleFile f,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const char * format, ...)
#if defined(XR_COMPILER_GCC)
    __attribute__ ((format (printf, 2, 3)))
#endif
    ;

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief Print to console with passed parameters
    */
// --------------------------------------------------------------------------------------  FUNCTION
void ConsolePrintf(
    ConsoleFile f,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const wchar_t * format, ...)
    ;

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief ConsolePrintf but with v args.
    */
// --------------------------------------------------------------------------------------  FUNCTION
void vConsolePrintf(
    ConsoleFile f,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const char * format,
    va_list args);

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief ConsolePrintf but with v args.
    */
// --------------------------------------------------------------------------------------  FUNCTION
void vConsolePrintf(
    ConsoleFile f,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const wchar_t * format,
    va_list args);
}}
#endif //#ifndef XR_CORE_CONSOLE_H
