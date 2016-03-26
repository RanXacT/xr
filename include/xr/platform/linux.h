// ######################################################################################### - FILE
/*! \file
Linux defines

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_INTERNAL_LINUX_DEFINES_H
#define XR_INTERNAL_LINUX_DEFINES_H
#pragma once

// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#include <stdint.h>
#include <unistd.h>
// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// Platform Stuff:

// Common Darwin base.
#define XR_PLATFORM_LINUX    1
#define XR_EOL "\r\n"
#define XR_PLATFORM_NAME      "LINUX"

//@}

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
// Platform Endianness and conversion support
// Right now only little endian platforms are used. More sophisticated logic
// should be placed here once support grows.
#define XR_PLATFORM_LITTLE_ENDIAN 1

#endif //#ifndef XR_INTERNAL_LINUX_DEFINES_H
