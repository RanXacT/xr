// ######################################################################################### - FILE
/*! \file
    Provides a simple and consistent API for system allocations and page
    requests.

    \par
    These functions have no state and are always available.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_MEMORY_H
#define XR_CORE_MEMORY_H

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

// ***************************************************************************************** - TYPE
/*! Memory Page functions, generally map OS specific functions for page
    mapping and protections
*/
// ***************************************************************************************** - TYPE
class Memory{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Note that not all protections exist on all systems, but general usage is guaranteed to work.
    \par implementation notes:
        If any protection is not implemented it falls down to the next in the following order.
        \li kPageAccessNone
        \li kPageAccessRead
        \li kPageAccessReadWrite  (guaranteed to exist)

    \par Execute Privileges
        Note that creation of PAGE_ACCESS_EXECUTE pages may not be permitted at all on many systems.
        In this case the application will ASSERT.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    enum PageAccess
    {
        kPageAccessNone,         ///< Page cannot be accessed (contents are retained, but just inaccessible until permissions change)
                                    ///< Use this to verify assumptions.
        kPageAccessRead,         ///< Memory is read only
        kPageAccessReadWrite,    ///< Memory is readable and writable, but not executable
        // Execute privilege modification is not exposed in any platform independent manor.
    };

    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief  Allocate System Memory for pools (allocates general memory). allocates in system pages. */
    // ------------------------------------------------------------------------------------  MEMBER
    XR_RETURN_BYTES(size)
    static void *           PageAlloc(size_t size, PageAccess eProtection = kPageAccessReadWrite);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief  changes protection on memory allocated with SystemPageAlloc. */
    // ------------------------------------------------------------------------------------  MEMBER
    static void             PageProtect(void * p, size_t size, PageAccess eProtection);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief  Allocate System Memory for pools (allocates general memory). allocates in system pages. */
    // ------------------------------------------------------------------------------------  MEMBER
    static void             PageFree(void * p, size_t size);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief  Return the page size of underlying OS. */
    // ------------------------------------------------------------------------------------  MEMBER
    static size_t           PageSize();

private:
};

}}
#endif //#ifndef XR_CORE_MEMORY_H
