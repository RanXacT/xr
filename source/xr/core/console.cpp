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
#ifndef XR_CORE_CONSOLE_H
#include "xr/core/console.h"
#endif
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif
#include <stdio.h>
#include <wchar.h>
#if defined(XR_PLATFORM_WINDOWS)
XR_DISABLE_ALL_WARNINGS()
#include <windows.h>
XR_RESTORE_ALL_WARNINGS()
#endif
// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE
// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {
// --------------------------------------------------------------------------------------  FUNCTION
// Prints to console
// --------------------------------------------------------------------------------------  FUNCTION
const size_t kConsoleMaxFormattedStringChars = 1024;
void ConsolePrintf(
    ConsoleFile conType,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const char * format, ...)
{
    FILE * f = (conType == kConsoleStdOut) ? stdout : stderr;
    // Only format the string once.
    char buffer[kConsoleMaxFormattedStringChars];

    va_list ap;
    va_start(ap,format);
    ::xr::Core::VStringPrintf(buffer, kConsoleMaxFormattedStringChars, format, ap);
    va_end(ap);

#if XR_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO info;
    memset(&info, 0, sizeof(info));
    if (f == stderr)
    {
        // retrieve settings
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info);

        // extract background colors
        WORD background = info.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        // reset foreground only to our desired color
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY | background);
    }
#else
    // http://ascii-table.com/ansi-escape-sequences-vt-100.php
    // http://www.cplusplus.com/forum/unices/36461/
    if (f == stderr)
    {
        fputs( "\033[0;31m", f);
    }
#endif
    // Log it.
    fputs(buffer, f);
    fflush(f);

#if XR_PLATFORM_WINDOWS
    OutputDebugStringA(buffer);

    if (f == stderr)
    {
        // restore previous settings
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
    }

#else
    if (f == stderr)
    {
        fputs( "\033[0m", f);
    }
#endif
}
// --------------------------------------------------------------------------------------  FUNCTION
// Prints to console
// --------------------------------------------------------------------------------------  FUNCTION
void ConsolePrintf(
    ConsoleFile conType,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const wchar_t * format, ...)
{
    FILE * f = (conType == kConsoleStdOut) ? stdout : stderr;
    // Only format the string once.
    wchar_t buffer[kConsoleMaxFormattedStringChars];

    va_list ap;
    va_start(ap,format);
    ::xr::Core::VStringPrintf(buffer, kConsoleMaxFormattedStringChars, format, ap);
    va_end(ap);

#if XR_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO info;
    memset(&info, 0, sizeof(info));
    if (f == stderr)
    {
        // retrieve settings
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info);

        // extract background colors
        WORD background = info.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        // reset foreground only to our desired color
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY | background);
    }
#endif
    // Log it.
    fputws(buffer, f);
    fflush(f);

#if XR_PLATFORM_WINDOWS
    OutputDebugStringW(buffer);

    if (f == stderr)
    {
        // restore previous settings
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
    }

#endif
}
// --------------------------------------------------------------------------------------  FUNCTION
// Prints to console
// --------------------------------------------------------------------------------------  FUNCTION
void vConsolePrintf(
    ConsoleFile conType,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const char * format, va_list ap)
{
    FILE * f = (conType == kConsoleStdOut) ? stdout : stderr;
    // Only format the string once.
    char buffer[kConsoleMaxFormattedStringChars];

    ::xr::Core::VStringPrintf(buffer, kConsoleMaxFormattedStringChars, format, ap);

#if XR_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO info;
    memset(&info, 0, sizeof(info));
    if (f == stderr)
    {
        // retrieve settings
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info);

        // extract background colors
        WORD background = info.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        // reset foreground only to our desired color
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY | background);
    }
#else
    if (f == stderr)
    {
        fputs( "\033[0;31m", f);
    }
#endif
    // Log it.
    fputs(buffer, f);
    fflush(f);

#if XR_PLATFORM_WINDOWS
    OutputDebugStringA(buffer);

    if (f == stderr)
    {
        // restore previous settings
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
    }
#else
    if (f == stderr)
    {
        fputs( "\033[0m", f);
    }
#endif

}

// --------------------------------------------------------------------------------------  FUNCTION
// Prints to console
// --------------------------------------------------------------------------------------  FUNCTION
void vConsolePrintf(
    ConsoleFile conType,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const wchar_t * format, va_list ap)
{
    FILE * f = (conType == kConsoleStdOut) ? stdout : stderr;
    // Only format the string once.
    wchar_t buffer[kConsoleMaxFormattedStringChars];

    ::xr::Core::VStringPrintf(buffer, kConsoleMaxFormattedStringChars, format, ap);

#if XR_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO info;
    memset(&info, 0, sizeof(info));
    if (f == stderr)
    {
        // retrieve settings
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info);

        // extract background colors
        WORD background = info.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        // reset foreground only to our desired color
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY | background);
    }
#endif
    // Log it.
    fputws(buffer, f);
    fflush(f);

#if XR_PLATFORM_WINDOWS
    OutputDebugStringW(buffer);

    if (f == stderr)
    {
        // restore previous settings
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), info.wAttributes);
    }

#endif

}


}}//namespace

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
