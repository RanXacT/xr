// ######################################################################################### - FILE
/*! \file

Internal management of Queue / Stack

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_DETAIL_QS_BASE_H
#define XR_CORE_DETAIL_QS_BASE_H

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

namespace detail
{
class QSProtector;
// ***************************************************************************************** - TYPE
/// Common base class allows allocation and assert in constructor to not
/// force an include from the header to other headers.
// ***************************************************************************************** - TYPE
class QSBase
{
protected:
    // ------------------------------------------------------------------------------------  MEMBER
    /// internal constructor
    // ------------------------------------------------------------------------------------  MEMBER
    QSBase(size_t size, size_t count, const char * name);
    ~QSBase();
    // ------------------------------------------------------------------------------------  MEMBER
    /// Returns the number of entries available to insert
    // ------------------------------------------------------------------------------------  MEMBER
    size_t InsertionLock();
    // ------------------------------------------------------------------------------------  MEMBER
    /// Pass in the number of entries actually inserted
    // ------------------------------------------------------------------------------------  MEMBER
    void InsertionUnLock( size_t numInserted );
    // ------------------------------------------------------------------------------------  MEMBER
    /// Returns the number of entries that can be removed
    // ------------------------------------------------------------------------------------  MEMBER
    size_t RemovalLock( );
    // ------------------------------------------------------------------------------------  MEMBER
    /// pass in the number of entries actually removed
    // ------------------------------------------------------------------------------------  MEMBER
    void RemovalUnLock( size_t numRemoved );
    // ------------------------------------------------------------------------------------  MEMBER
    /// Kick listeners in the case where a thread is leaving the pool and more work remains
    // ------------------------------------------------------------------------------------  MEMBER
    void Kick();
    // ------------------------------------------------------------------------------------  MEMBER
    /// used for testing purposes
    // ------------------------------------------------------------------------------------  MEMBER
    size_t UnsafeGetAvailableCount();
    // ------------------------------------------------------------------------------------  MEMBER
    /// used for testing purposes
    // ------------------------------------------------------------------------------------  MEMBER
    size_t UnsafeGetFreeCount();
    // ------------------------------------------------------------------------------------  MEMBER
    /// used by derived classes to get allocated buffer
    // ------------------------------------------------------------------------------------  MEMBER
    void * GetBuffer(){return mBuffer;}
private:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Buffer allocated in constructor.
    // ------------------------------------------------------------------------------------  MEMBER
    void * mBuffer;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Protection implementation allocated in constructor.
    // ------------------------------------------------------------------------------------  MEMBER
    QSProtector * mProtector;
};
} // detail
}} // namespace
#endif //#ifndef XR_CORE_DETAIL_QS_BASE_H
