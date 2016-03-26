// ######################################################################################### - FILE
/*! \file
    atomic reference counting interface. Note that reference counting is only
    used as a matter of last resort, explicit object lifetime is preferred.

    \par
    An exrmple of unknown object lifetime would be an observer pattern where
    the number of observers is unknown (and some observers may use the data
    asynchronously).

    \par
    Note that Reference count starts at 1 as returned (the person who
    creates it need not call AddRef, but must call Release).

    \sa http://en.wikipedia.org/wiki/Reference_counting
    \sa xr::Core::intrusive_ptr
\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_REFCOUNT_H
#define XR_CORE_REFCOUNT_H

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
/*! \brief base class to ease atomic reference counting.
*/
// ***************************************************************************************** - TYPE
class RefCounted{
protected:
    RefCounted();
    virtual ~RefCounted();
public:
    /// Add a reference to this object
    void AddRef() const;
    /// Release a reference to this object (may delete the object)
    void Release() const;

    /// Unsafe call to get current reference count. Intended for testing.
    inline size_t           UnsafeGetRefCount() const {return (size_t) mReferenceCount; }

private:
    mutable intptr_t mReferenceCount;

    // Needed to manage delete calls through private destructor
    template <typename T>
    friend void DeleteHelper(T * p, const char * filename, int32_t line_number);
    template <typename T>
    friend void DeleteArrayHelper(T * ptr, const char * filename, int32_t line_number);
    friend class IUnknownRefCounted;
};

// ***************************************************************************************** - TYPE
/*! \brief base class to ease basic (non thread safe) reference counting.
*/
// ***************************************************************************************** - TYPE
class NonThreadSafeRefCounted{
protected:
    NonThreadSafeRefCounted();
    virtual ~NonThreadSafeRefCounted();
public:
    /// Add a reference to this object
    void AddRef() const;
    /// Release a reference to this object (may delete the object)
    void Release() const;

    /// Unsafe call to get current reference count. Intended for testing.
    inline size_t    UnsafeGetRefCount() const {return (size_t) mReferenceCount; }

private:
    mutable intptr_t mReferenceCount;

    // Needed to manage delete calls through private destructor
    template <typename T>
    friend void DeleteHelper(T * p, const char * filename, int32_t line_number);
    template <typename T>
    friend void DeleteArrayHelper(T * ptr, const char * filename, int32_t line_number);
    friend class IUnknownRefCounted;
};

// ***************************************************************************************** - TYPE
/*! \brief base class to satisfy Reference count API, without actually using
the reference count.
*/
// ***************************************************************************************** - TYPE
class NotRefCounted{
protected:
    NotRefCounted() {}
    virtual ~NotRefCounted() {}
public:
    /// Add a reference to this object
    void AddRef() const {}
    /// Release a reference to this object (may delete the object)
    void Release() const {}

    /// Unsafe call to get current reference count. Intended for testing.
    inline size_t    UnsafeGetRefCount() const {return XR_SIZE_MAX; }
};

}}
#endif //#ifndef XR_CORE_REFCOUNT_H
