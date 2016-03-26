// ######################################################################################### - FILE
/*! \file
OSX defines (darwin)

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_INTERNAL_DARWIN_DEFINES_VC_H
#define XR_INTERNAL_DARWIN_DEFINES_VC_H
#pragma once

#if !defined( __APPLE__ )
#error "This file is intended for Apple / Mac platforms"
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#include <stdint.h>
#include <TargetConditionals.h>
#include <libkern/OSByteOrder.h> // for byte swapping internals.
#include <unistd.h>
// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Platform Stuff:

// Common Darwin base. (both OSX and iOS)
#define XR_PLATFORM_DARWIN    1
// Unix line endings
#define XR_EOL "\r\n"


// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// Now split out the environment based on defines


// iOS simulator
#if TARGET_IPHONE_SIMULATOR
#define XR_PLATFORM_IOS              1
#define XR_PLATFORM_IOS_SIMULATOR    1
#define XR_PLATFORM_NAME      "iOSSimulator"

// iOS
#elif TARGET_OS_IPHONE
#define XR_PLATFORM_IOS       1
#define XR_PLATFORM_NAME      "iOS"

// OSX
#elif TARGET_OS_MAC
#define XR_PLATFORM_OSX       1
#define XR_PLATFORM_NAME      "OSX"

#else
#error "Unknown target!!!"
#endif

//@}

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Platform Endianness and conversion support
#if __BIG_ENDIAN__
#define XR_PLATFORM_BIG_ENDIAN 1
#elif __LITTLE_ENDIAN__
#define XR_PLATFORM_LITTLE_ENDIAN 1
#else
#error "Expected Endianness define"
#endif

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
#define XR_SWAP_UINT16_T(value)  _OSSwapInt16(value)
#define XR_SWAP_UINT32_T(value)  _OSSwapInt32(value)
#define XR_SWAP_UINT64_T(value)  _OSSwapInt64(value)
//@}

// ######################################################################################### - FILE
// typedefs

// ######################################################################################### - FILE
// private functions

// ######################################################################################### - FILE
// public functions

#endif //#ifndef XR_INTERNAL_WINDOWS_DEFINES_VC_H
