

#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif

#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif

#ifndef XR_CORE_LOG_H
#include "xr/core/log.h"
#endif

#ifndef XR_CORE_CONSOLE_H
#include "xr/core/console.h"
#endif

#ifndef XR_CORE_DATE_H
#include "xr/core/date.h"
#endif

#include <stdio.h>

#ifdef XR_PLATFORM_WINDOWS
#include <windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#ifdef XR_PLATFORM_DARWIN
#include <mach-o/dyld.h>
#endif


#if _MSC_VER
#define FORCE_CDECL __cdecl 
#else 
#define FORCE_CDECL 
#endif


int FORCE_CDECL main(int argc, char* argv[])
{
    char dateStr[16];
    int retVal = 0;
    XR_UNUSED(argc);
    XR_UNUSED(argv);

    xr::Core::ConsolePrintf(xr::Core::kConsoleStdOut,
        "System: %s %s-%" XR_INT32_PRINT "bit %s" XR_EOL, 
        XR_PLATFORM_NAME, 
        XR_CPU_NAME, 
        int32_t(XR_PLATFORM_PTR_SIZE * 8), 
        XR_COMPILER_NAME
        );
    
    char path[FILENAME_MAX];

#ifdef XR_PLATFORM_WINDOWS
    if(GetModuleFileNameA(nullptr, path, FILENAME_MAX) != 0)
    {
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdOut,
            "exe : %s" XR_EOL,
            path
            );
    }
#elif defined(XR_PLATFORM_DARWIN)
    uint32_t bufsize = FILENAME_MAX;
    if(_NSGetExecutablePath( path, &bufsize) == 0)
    {
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdOut,
                                "exe : %s" XR_EOL,
                                path
                                );
    }
#else
    readlink("/proc/self/exe", path, FILENAME_MAX - 1);
    xr::Core::ConsolePrintf(xr::Core::kConsoleStdOut,
                            "exe : %s" XR_EOL,
                            path
                            );

#endif

    if (GetCurrentDir(path, FILENAME_MAX))
    {
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdOut,
            "pwd : %s" XR_EOL,
            path
            );
    }


    
    size_t testCount = 2;
    for(size_t counter = 0; counter < testCount; ++counter)
    {
        xr::Core::DateStamp tStart = xr::Core::Now();
        xr::Core::ToString(xr::Core::kDateLocal, dateStr, 16, "%X", tStart);

        xr::Core::ConsolePrintf(xr::Core::kConsoleStdOut,
            "%s Starting Tests... ## %" XR_SIZE_PRINT " ##" XR_EOL, dateStr, ++counter);

#if defined(XR_TEST_FEATURES_ENABLED)
        if(xr::Core::Test::RunAll() != xr::Core::kSuccess)
        {
            retVal = -1;
        }
#endif // #if defined(XR_TEST_FEATURES_ENABLED)

        xr::Core::LogSystemShutdown();

        xr::Core::DateStamp tEnd = xr::Core::Now();

        xr::Core::ToString(xr::Core::kDateLocal, dateStr, 16, "%X", tEnd);

        if(retVal == 0)
        {
            xr::Core::ConsolePrintf(xr::Core::kConsoleStdOut,
                "%s All tests Completed successfully" XR_EOL, dateStr);
        }
        else
        {
            xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
                "%s Test Failure" XR_EOL, dateStr);
            break;
        }
    }
    
    fflush(stdout);
    fflush(stderr);
    return retVal;
}


