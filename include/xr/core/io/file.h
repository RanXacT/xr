// ######################################################################################### - FILE
/*!
    \page corefile File loading abstraction
    This loads a full file synchronously. This is intended for debug or special
    purpose use. It is a fairly low value abstraction but does help in the case
    of properly logging file errors and by efficiently obtaining the file size
    (which is tricky to do portably). For general purpose IO see the IO
    services which provide higher level functionality for an asset system.

    \note There is no intent to provide a full file system abstraction, which
    is a large undertaking (and would significantly duplicate freely available
    code from Boost, Poco, APR, or a number of other high quality libraries. If
    you need a full file system abstraction, use one of those (although you
    probably don't need a full file system abstraction).

    \file
    \copydoc corefile

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_IO_FILE_H
#define XR_CORE_IO_FILE_H

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
/// This reads a whole file using binary mode. (no text translation provided)
/// file name must be in utf-8 format to deal with internationalization issues.
/// Be sure to deallocate the outBuffer when it is no longer needed.
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult FileReadImmediate(const char * fileName, void ** outBuffer, size_t * outSize, const char * allocName = nullptr);


}}
#endif //#ifndef XR_CORE_IO_FILE_H
