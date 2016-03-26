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
#ifndef XR_CORE_INTRUSIVE_PTR_H
#define XR_CORE_INTRUSIVE_PTR_H

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
template <typename T> class intrusive_ptr;

// ***************************************************************************************** - TYPE
/*! \brief basic intrusive_ptr implementation. This is in C++ 11, (and the
    implementation is essentially the same). If C++ becomes more commonly
    adopted, then this can be replaced with std::intrusive_ptr.

    \par Differences with C++11:
    \li AddRef / Release are names intrusize_* in standard. This is
        cumbersome as the smart pointer is intended to work with COM
        IUnknown type objects which use AddRef / Release. These also
        do return the reference count in the C++ spec, but not here
        (for safety).
    \li "Get" in C++ 11 is renamed to "Get" in order to make
        clear that it is obtaining a bald pointer and that there are
        safety ramifications for this.

    \par Usage:
        Note that Refcounted objects are returned with a reference count
        of 1. This is important to prevent punch through 0 issues. The
        reason being that leaking memory is better than corrupting memory.
        Proper usage (to prevent memory leaks) can be managed by one of
        two ways.

    \code
// Dummy object for demonstration purposes.
class Foo: public xr::Core::RefCounted {};

void Test()
{
// Option A: pass immediately to intrusive_ptr with "false"
// as the "add_ref" parameter. This ensures proper Release().
// This is the safest option and in this case you need do nothing
// else in client code.
xr::Core::intrusive_ptr<Foo> bar ( XR_NEW("test") Foo(), false);

// Option B: Call Release Explicitly when done with the object.
Foo * bar2 = XR_NEW("test") Foo();

// Use bar2

// Manually call release
bar2->Release();
}
    \endcode

    \note Note that xr::COM::IUnknown::QueryInterface
            and interface_cast also pre-increment the reference count.
*/
// ***************************************************************************************** - TYPE
template <typename T>
class intrusive_ptr
{
public:
    typedef intrusive_ptr this_type;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Constructors
    //@{
    XR_FORCE_INLINE intrusive_ptr() : mP(nullptr) {}
    XR_FORCE_INLINE intrusive_ptr(T * p, bool add_ref = true) : mP(p)
    {
        if(mP != nullptr && add_ref) mP->AddRef();
    }
    XR_FORCE_INLINE intrusive_ptr(intrusive_ptr const & rhs) : mP(rhs.mP)
    {
        if(mP != nullptr) mP->AddRef();
    }
    //@}
    // ------------------------------------------------------------------------------------  MEMBER
    /// Special constructor for related types, compiler will give an error if
    /// the types are not related.
    template<typename U>
    XR_FORCE_INLINE intrusive_ptr(intrusive_ptr<U> const & rhs) : mP(rhs.get())
    {
        if(mP != nullptr) mP->AddRef();
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// destructor
    ~intrusive_ptr()
    {
        if(mP != 0) mP->Release();
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// Operators
    //@{
    XR_FORCE_INLINE intrusive_ptr & operator=(const intrusive_ptr<T> &rhs)
    {
        Set(rhs.mP);
        return *this;
    }
    XR_FORCE_INLINE intrusive_ptr & operator=(T * rhs)
    {
        Set(rhs);
        return *this;
    }
    XR_FORCE_INLINE T * operator->() const
    {
        return mP;
    }
    //@}
    // ------------------------------------------------------------------------------------  MEMBER
    /// Fancy cast to bool
    //@{
private:
    typedef T * this_type::* const unspecified_bool_type;
public:
    XR_FORCE_INLINE operator unspecified_bool_type() const
    {
        return mP == 0? 0: &this_type::mP;
    }
    //@}
    // ------------------------------------------------------------------------------------  MEMBER
    /// Less fancy "!" operator
    XR_FORCE_INLINE bool operator! () const
    {
        return mP == 0;
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// retrieve unsafe reference.
    XR_FORCE_INLINE T * Get() const
    {
        return mP;
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// This takes on a reference, but does not AddRef.
    void AssumeReference(T * p)
    {
        if(mP != nullptr)
        {
            mP->Release();
        }
        mP = p;
    }

private:
    /// Internal set function
    void Set(T * p)
    {
        T * temp = mP;
        mP = p;
        if(mP != nullptr)
        {
            mP->AddRef();
        }
        if(temp != nullptr)
        {
            temp->Release();
        }
    }

    /// Pointer to the actual pointed to object
    T * mP;
};


// --------------------------------------------------------------------------------------  FUNCTION
/// intrusive_ptr operators
//@{
// --------------------------------------------------------------------------------------  FUNCTION
template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
    return a.Get() == b.Get();
}
template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
    return a.Get() != b.Get();
}
template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, U * b)
{
    return a.Get() == b;
}
template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, U * b)
{
    return a.Get() != b;
}
template<class T, class U> inline bool operator==(T * a, intrusive_ptr<U> const & b)
{
    return a == b.Get();
}
template<class T, class U> inline bool operator!=(T * a, intrusive_ptr<U> const & b)
{
    return a != b.Get();
}

}}
#endif //#ifndef XR_CORE_REFCOUNT_H
