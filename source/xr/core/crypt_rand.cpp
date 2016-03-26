// ######################################################################################### - FILE
/*!
\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/

#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_RANDOM_CRYPT_RAND_H
#include "xr/core/random/crypt_rand.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif


#if defined(XR_PLATFORM_WINDOWS)
// CryptGenRandom needlessly initializes a whole RSA engine.
// An alternative RtlGenRandom generates equivalent random numbers, but is more
// efficient if you do not use the windows Cryptography API for anything else.
#define XR_USE_WINDOWS_CryptGenRandom 0
XR_DISABLE_ALL_WARNINGS()
#include <windows.h>
XR_RESTORE_ALL_WARNINGS()
#   if XR_USE_WINDOWS_CryptGenRandom
#include <Wincrypt.h>
#   else

// #define magic needed to fix calling convention due to broken windows header
// This fixed an unresolved external for "cdecl SystemFunction036" which is
// actually "stdcall SystemFunction036". Note that SystemFunction036 is
// just the resource name of the RtlGenRandom function in Advapi32.dll
// see http://msdn.microsoft.com/en-us/library/aa387694%28v=vs.85%29.aspx
#define SystemFunction036 NTAPI SystemFunction036
#include <NTSecAPI.h>
#undef SystemFunction036

#include <NTSecAPI.h>
#   endif

#pragma comment(lib, "advapi32.lib")

#else  // For unix systems, use /dev/random
#include <fcntl.h>
#include <unistd.h>
#endif

namespace xr { namespace Core {

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void CryptRandFill(XR_OUT_BYTES(size)void * buffer, size_t size)
{
#if defined(XR_PLATFORM_WINDOWS)
#   if XR_USE_WINDOWS_CryptGenRandom
    HCRYPTPROV   hCryptProv;
    BOOL retValue = CryptAcquireContext(&hCryptProv, nullptr, nullptr, PROV_RSA_FULL, 0);
    XR_ASSERT_ALWAYS_NE(retValue, FALSE);

    retValue = CryptGenRandom(hCryptProv, size, (BYTE*)buffer);
    XR_ASSERT_ALWAYS_NE(retValue, FALSE);
#   else
    BOOL retValue = RtlGenRandom(buffer, (ULONG)size);
    XR_UNUSED(retValue);
    XR_ASSERT_ALWAYS_NE(retValue, FALSE);
#   endif

#else
    // posix platforms use /dev/random
    int fdRand = open("/dev/random", O_RDONLY);
    int count = read(fdRand, buffer, size );
    XR_ASSERT_ALWAYS_EQ((size_t)count, size);

    int retValue = close(fdRand);
    XR_ASSERT_ALWAYS_EQ( retValue, 0);
#endif
}

}}//namespace

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
