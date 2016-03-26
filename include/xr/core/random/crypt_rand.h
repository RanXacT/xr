// ######################################################################################### - FILE
/*! \file

This returns cryptographically secure random numbers from the underlying OS.
These could be pseudo random numbers or true random numbers based on hardware
and OS and are only as secure as the operating system provides.

http://en.wikipedia.org/wiki/Cryptographically_secure_pseudorandom_number_generator

http://en.wikipedia.org/wiki//dev/random

http://en.wikipedia.org/wiki/CryptGenRandom


\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_RANDOM_CRYPT_RAND_H
#define XR_CORE_RANDOM_CRYPT_RAND_H

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

    // --------------------------------------------------------------------------------------  FUNCTION
    /*! Fill a buffer with Cryptographically random data.
     */
    // --------------------------------------------------------------------------------------  FUNCTION
    void CryptRandFill(XR_OUT_BYTES(size) void * buffer, size_t size);

}}
#endif //#ifndef XR_CORE_RANDOM_CRYPT_RAND_H
