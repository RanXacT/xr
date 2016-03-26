// ######################################################################################### - FILE
/*! \file
Windows based information

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_INTERNAL_WINDOWS_DEFINES_VC_H
#define XR_INTERNAL_WINDOWS_DEFINES_VC_H
#pragma once

#if !defined( _WIN32 )
#error "This file is intended for Windows platform"
#endif

// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// OS information
#define XR_PLATFORM_WINDOWS   1
#define XR_PLATFORM_NAME      "Windows"
#define XR_PLATFORM_LITTLE_ENDIAN 1
#define XR_EOL "\n"
//@}


#endif //#ifndef XR_INTERNAL_WINDOWS_DEFINES_VC_H