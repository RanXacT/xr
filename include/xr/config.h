// ######################################################################################### - FILE
/*! \file
   \brief basic compile time configuration

\author Daniel Craig \par Copyright 2016, All Rights reserved.
 */
// ######################################################################################### - FILE
// Guard
#ifndef XR_CONFIG_H
#define XR_CONFIG_H
#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

#ifndef XR_TEST_FEATURES_ENABLED
/// Enable Testing functionality.
#   define XR_TEST_FEATURES_ENABLED    1
#endif

#ifndef XR_DEBUG_FEATURES_ENABLED
/// Enable Debug features
#   define XR_DEBUG_FEATURES_ENABLED   1
#endif

#ifndef XR_LOG_MINIMUM_LEVEL
/// Configure logging. Set XR_LOG_MINIMUM_LEVEL to XR_LOG_LEVEL_FATAL
/// to get minimal possible logging, but even released applications
/// may benefit from additional log messages (albeit most likely not
/// all of them). One may find it useful to leave some in for field
/// reporting...
#   if   XR_DEBUG_FEATURES_ENABLED
#       define XR_LOG_MINIMUM_LEVEL     XR_LOG_LEVEL_TRACE
#   else
#       define XR_LOG_MINIMUM_LEVEL     XR_LOG_LEVEL_INFO
#   endif
#endif

#ifndef XR_PROFILE_FEATURES_ENABLED
/// Enable Profiling features
#   define XR_PROFILE_FEATURES_ENABLED 1
#endif


// Add additional configuration options here.

#endif //#ifndef XR_CONFIG_H
