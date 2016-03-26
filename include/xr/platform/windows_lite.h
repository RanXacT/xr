
// ######################################################################################### - FILE
/*! Forward declaraions for common windows types. (to prevent windows.h
    inclusion for basic things). Can be used from msvc, intel, or mingw
*/
// ######################################################################################### - FILE
//-----------------------------------------------------------------------------
/*!@{
Common Defines from windows.h, but windows.h is huge! See msdn docs for
details on any of these functions / values. This allows basic access windows
functionality without directly including windows.h. It is mainly intended for
internal use so that we never internally touch windows.h proper.

NOTE: This is not intended to be used in a file that also #includes windows.h
      If you are going to include windows.h elsewhere in your source, then just
      include it instead of this.

FWIW windows.h even with "WIN32_LEAN_AND_MEAN" and "VC_EXTRALEAN" is over 1MB
of preprocessed source code to be parsed / managed. (probably much more then
the source file you are compiling.
*/

#ifndef _WINDOWS_

#ifdef __cplusplus
extern "C" {
#endif

#if !XR_PLATFORM_WINDOWS
#error "This file should only be used for windows builds!!"
#endif

#if XR_COMPILER_MICROSOFT || XR_COMPILER_GCC

#ifndef CALLBACK
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define WINAPIV     __cdecl
#define APIENTRY    WINAPI
#define APIPRIVATE  __stdcall
#define PASCAL      __stdcall
#define DECLSPEC_IMPORT __declspec(dllimport)
#endif

#else
#error These need to be filled in.
#endif

#ifndef WINBASEAPI
#define WINBASEAPI   DECLSPEC_IMPORT
#endif

// --------------------------------------------------------------------------------------  FUNCTION
// Profiling (performance counters)
// --------------------------------------------------------------------------------------  FUNCTION
// Don't define this only declare it.
union _LARGE_INTEGER;

WINBASEAPI int WINAPI QueryPerformanceCounter(union _LARGE_INTEGER *lpPerformanceCount);
WINBASEAPI int WINAPI QueryPerformanceFrequency(union _LARGE_INTEGER *lpFrequency);
// --------------------------------------------------------------------------------------  FUNCTION
// Threading
// --------------------------------------------------------------------------------------  FUNCTION
WINBASEAPI int WINAPI GetThreadPriority(void *);
WINBASEAPI int WINAPI SetThreadPriority(void * hThread, int nPriority );
WINBASEAPI void* WINAPI GetCurrentThread( void );
WINBASEAPI void WINAPI Sleep(unsigned long dwMilliseconds);
// --------------------------------------------------------------------------------------  FUNCTION
// Debug
// --------------------------------------------------------------------------------------  FUNCTION
WINBASEAPI void WINAPI OutputDebugStringA( const char * lpOutputString );

// Used for structured exception handling
WINBASEAPI int WINAPI IsDebuggerPresent( void );


WINBASEAPI void WINAPI RaiseException(
  uint32_t dwExceptionCode,
  uint32_t dwExceptionFlags,
  uint32_t nNumberOfArguments,
  const unsigned long **lpArguments
);

WINBASEAPI void WINAPI GetExitCodeThread(uintptr_t hThread, uint32_t * lpExitCode);

typedef long (WINAPI *XR_TOP_LEVEL_EXCEPTION_FILTER)(
    struct _EXCEPTION_POINTERS *ExceptionInfo
    );

WINBASEAPI
    void *
    WINAPI
    SetUnhandledExceptionFilter(
    void * lpTopLevelExceptionFilter
    );

#define EXCEPTION_EXECUTE_HANDLER       1
#define EXCEPTION_CONTINUE_SEARCH       0
#define EXCEPTION_CONTINUE_EXECUTION    -1

#ifdef __cplusplus
}
#endif

#endif
//@}
