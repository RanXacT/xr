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
#ifndef XR_CORE_IO_FILE_H
#include "xr/core/io/file.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_LOG_H
#include "xr/core/log.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif

#if XR_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif
// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

static LogHandle sFileLogHandle("xr.file");

#if defined(XR_PLATFORM_WINDOWS)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult FileReadImmediate(const char * fileName, void ** outBuffer, size_t * outSize, const char * allocName)
{
    HANDLE h = CreateFileA(
        fileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr);

    if(h == INVALID_HANDLE_VALUE)
    {
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s Could not be opened. GetLastError:0x%lX", fileName, GetLastError());
        return kFailure;
    }

    LARGE_INTEGER largeSize;
    BOOL b = GetFileSizeEx(h, &largeSize);

    if(b == 0)
    {
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s size failed. GetLastError:0x%lX", fileName, GetLastError());
        CloseHandle(h);
        return kFailure;
    }
    *outSize = (size_t)largeSize.QuadPart;

#if XR_PLATFORM_PTR_SIZE == 4
    // 32 bit version can only deal with files so large.
    if(largeSize.QuadPart > XR_INT64_C( 1 << 21 ) )
    {
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s Too large for 32 bit program to operate on", fileName);
        CloseHandle(h);
        return kFailure;
    }
#endif

    *outBuffer = XR_ALLOC(*outSize, allocName == nullptr ? "FileReadImmediate" : allocName);

    // Loss of data intended here. This function asserts above that the size is < 4GB.
    // (and retuens an error).
    DWORD bytesToRead = (DWORD)*outSize;
    DWORD bytesRead   = 0;
    b = ReadFile(h, *outBuffer, bytesToRead, &bytesRead , nullptr);
    if(b == 0)
    {
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s Read failed. GetLastError:0x%lX", fileName, GetLastError());
        CloseHandle(h);
        XR_FREE(*outBuffer);
        return kFailure;
    }

    if(bytesRead != bytesToRead)
    {
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s Read stopped early. GetLastError:0x%lX", fileName, GetLastError());
        CloseHandle(h);
        XR_FREE(*outBuffer);
        return kFailure;
    }

	CloseHandle(h);
    return kSuccess;
}
#else
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult FileReadImmediate(const char * fileName, void ** outBuffer, size_t * outSize, const char * allocName)
{
    int fd = open(fileName, O_RDONLY);
    if(fd == -1)
    {
        int err = errno;
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s Open error. %d", fileName, err);
        return kFailure;
    }

    struct stat buf;
    int err = fstat(fd, &buf);

    if(err < 0)
    {
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s Stat error. %d", fileName, err);
        return kFailure;
    }

    uint64_t size = buf.st_size;

#if XR_PLATFORM_PTR_SIZE == 4
    // 32 bit version can only deal with files so large.
    if(size > XR_UINT64_C( 1 << 21 ) )
    {
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s Too large for 32 bit program to operate on", fileName);
        close(fd);
        return kFailure;
    }
#endif

    int bytes = (int) size;
    *outSize  =(size_t) size;

    *outBuffer = XR_ALLOC(*outSize, allocName == nullptr ? "FileReadImmediate" : allocName);

    int num = read(fd, *outBuffer, bytes);
    if(num != bytes)
    {
        XR_LOG_DEBUG_FORMATTED(&sFileLogHandle, "File %s not read fully", fileName);
        close(fd);
        XR_FREE(*outBuffer);
        return kFailure;
    }

	close(fd);
    return kSuccess;
}
#endif

}}//namespace
// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
