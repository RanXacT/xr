// ######################################################################################### - FILE
/*!

\page com COM Overview

The COM functionality of the system is basic but operable. The COM API
depends only on the platform headers and is easily included in DLL's etc.

\par
Generally speaking COM interfaces would derive from xr::COM::IUnknown and interface
implentations should derive from xr::Core::IUnknownRefCounted (which provides
AddRef / Release functions). Implementations must provide a QueryInterface
function, but macros are provided to make that easier.

\copydoc xr::COM::IUnknown


\file \copydoc com

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_COM_UNKNOWN_H
#define XR_COM_UNKNOWN_H

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
namespace xr { namespace Core {
        // Forward declaration does not require actual use of Core Library
        // it just allows optional use of xr::Core::intrusive_ptr.
        template<typename T>
        class intrusive_ptr;
    }
}
// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace COM {

/// Typedef for COM InterfaceID's, this could be changed in the future
/// but should remain signed to deal inter operate with enums used to manage
/// static IID's.
typedef int32_t InterfaceID;

// ***************************************************************************************** - TYPE
/*! Basic COM interface.
    IUknown is used to safely pass opaque types around at runtime. It allows
    intermediaries to operate on objects with no compile time knowledge of
    them. It allows also safe runtime type casting to interfaces. Generally
    COM is used for high level API's to abstract implementation of an
    interface from (direct or indirect) users of the interface. Indirect
    users referencing the type as IUnknown and Direct users safely casting
    to the desired interface when needed.


    \par Differences with standard COM (Minor)
    \li QueryInterface returns a void * (or nullptr)
    \li Interface ID is smaller (it's size is easily changed).
    \li AddRef / Release do not return the reference count. This is because
        reference counts are generally not safe to use.
    \li AddRef / Release are "const" functions. This is to help promote
        const correctness.

    \par Reference Counting.
        Generally speaking users are intended to use xr::Core:RefCounted
        or xr::Core::NonThreadSafeRefCounted to implement reference
        counting. Also use of xr::Core::intrusive_ptr for references is
        expected. These objects are designed to work together and make
        COM implementation and usage simple.

    \par Core functionality
        The core library provides helper classes to make implementing com
        objects quite trivial.
    \sa xr::Core::IUnknownHelper0

    \endcode
    \sa http://en.wikipedia.org/wiki/IUnknown
    */
// ***************************************************************************************** - TYPE
class IUnknown {
public:
    virtual ~IUnknown() {}
    // ------------------------------------------------------------------------------------  MEMBER
    /// This function returns
    /// This function must internally call AddRef before returning the pointer.
    /// The caller must then call Release() on the returned pointer once it
    /// is done with it.
    virtual xr::COM::IUnknown * QueryInterface(InterfaceID id) = 0;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Add a reference to this object
    virtual void AddRef() const = 0;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Release a reference to this object (may delete the object)
    virtual void Release() const = 0;
    // ------------------------------------------------------------------------------------  MEMBER
    /// static function to get the interface ID of this object
    enum
    {
        kIID = 1
    };
    // ------------------------------------------------------------------------------------  MEMBER
    /// Helper function for const correctness.
    inline const xr::COM::IUnknown * QueryInterface(InterfaceID id) const
    {
        IUnknown* const_this = const_cast<IUnknown*>(this);
        return const_this->QueryInterface(id);
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// This helper does the type cast for you, but you still must
    /// explicitly call Release(), or pass to intrusive_ptr without
    /// it calling AddRef.
    /// \sa xr::Core::intrusive_ptr
    /// \sa xr::Core::intrusive_ptr::AssumeReference
    /// \sa xr::Core::intrusive_ptr::intrusive_ptr
    //@{
    template<typename T>
    T * interface_cast()
    {
        return static_cast<T*>( QueryInterface(T::kIID) );
    }
    template<typename T>
    const T * interface_cast() const
    {
        return static_cast<const T*>( QueryInterface(T::kIID) );
    }
    //@}
    // ------------------------------------------------------------------------------------  MEMBER
    /// This function ensures proper reference counting is automatically.
    /// maintained. No extra reference counts needed.
    template<typename T>
    void interface_cast(xr::Core::intrusive_ptr<T> &ip)
    {
        ip.AssumeReference( static_cast<T*>( QueryInterface(T::kIID)));
    }
};

}} // namespace
#endif //#ifndef XR_COM_UNKNOWN_H
