// ######################################################################################### - FILE
/*! \file
XR core implementation for xr::COM::IUnkown classes. This provides a base
object and macros to ease implementation of com objects using XR::Core
functionality.

\par Exrmple Code:
\code

// Interface definition.
class ITestInterface : public xr::COM::IUnknown
{
public:
    XR_COM_CLASS_ID(0x11111111); // Should use random number.
    virtual void Run() = 0;
}

// Concrete implementation uses xr::Core::IUnknownHelper1 to manage IUnknown
// implementation. Concrete class need not worry about those details.
class MyTestInterface() : public xr::Core::IUnknownHelper1<MyTestInterface, ITestInterface>
{
    XR_COM_CLASS_ID(0x22222222); // Should use random number.

    // If you define a constructor you must call the parent class constructor.
    // the helper defines a typedef of the type as "base_type" to make this easier.
    MyTestInterface(): base_type() {}

    void Run()
    {
    }
}
\endcode

\par Multiple Inheritance
This poses a couple additional challenges, but they can mostly be resolved internally.
The only issue that involves client code is that type casting to the concrete type
is not trivial, and doing so directly (or via xr::COM::IUnknown::interface_cast<>)
will result in a compiler error. To resolve this the base helper class defines a
static "QueryImplementation" call which is accessible in the concrete type.
\code
// Concrete implementation uses xr::Core::IUnknownHelper1 to manage IUnknown
// implementation. Concrete class need not worry about those details.
class MultiTest() public xr::Core::IUnknownHelper3<MyTestInterface, ITestInterface1, ITestInterface2, ITestInterface3>
{
    XR_COM_CLASS_ID(0x1034567);

    // Impl...
}

void main()
{
    xr::COM::IUnknown * u = ReturnsAnUnknown();

    // interface casts work to any implemented interface.
    ITestInterface1 * t1 = u->interface_cast<ITestInterface1>();
    ITestInterface2 * t2 = u->interface_cast<ITestInterface2>();
    ITestInterface3 * t3 = u->interface_cast<ITestInterface3>();

    // Interface cast FAILS to concrete class if it uses multiple inheritance.

    MultiTest * t = u->interface_cast<MultiTest>(); // Compiler Error!
    t->Release();

    // Use this function in that case or whenever casting to a concrete class
    // Note that this still calls QueryInterface which calls AddRef.
    MultiTest * t = MultiTest::QueryImplementation(u);      // Success!
    t->Release();
}

\endcode
    \sa http://en.wikipedia.org/wiki/Reference_counting
    \sa xr::Core::intrusive_ptr
\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_UNKNOWN_H
#define XR_CORE_UNKNOWN_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif
// This include is for IUnknownRefCounted which can be moved to it's own header
// if desired (and break this header dependency)
#ifndef XR_COM_UNKNOWN_H
#include "xr/com/unknown.h"
#endif
#ifndef XR_CORE_REFCOUNT_H
#include "xr/core/refcount.h"
#endif
// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE
/// Defines a class ID, the macro is not necessary, but is more readable that
/// a random enum.
#define XR_COM_CLASS_ID(typeID) enum { kIID = typeID }

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE
// Forward declaration does not require actual use of Core Library
// it just allows optional use of xr::Core::intrusive_ptr.
template<typename T>
class intrusive_ptr;
// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/*! \brief base class to ease COM based (IUnknown) reference counting.
    IUnknownHelper0 is intended for objects that only implement IUnknown
    and their concrete type.
*/
// ***************************************************************************************** - TYPE
template <typename ConcreteType, class RefCountImpl = RefCounted>
class IUnknownHelper0 : public RefCountImpl, public xr::COM::IUnknown {
protected:
    typedef IUnknownHelper0<ConcreteType> base_type;
    IUnknownHelper0() : RefCountImpl(){}
    virtual ~IUnknownHelper0() {}
public:
    /// Add a reference to this object
    void AddRef() const XR_OVERRIDE
    {
        RefCountImpl::AddRef();
    }
    /// Release a reference to this object (may delete the object)
    void Release() const XR_OVERRIDE
    {
        RefCountImpl::Release();
    }
    /// Unsafe call to get current reference count. Intended for testing.
    size_t    UnsafeGetRefCount() const
    {
        return RefCountImpl::UnsafeGetRefCount();
    }
    /// IUnknown implementation function
    xr::COM::IUnknown * QueryInterface(::xr::COM::InterfaceID id) XR_OVERRIDE
    {
        if(id == ConcreteType::kIID || id == xr::COM::IUnknown::kIID)
        {
            this->AddRef();
            return this;
        }
        return nullptr;
    }
    /// Helper function to cast from IUnknown to concrete object
    /// this is only required for multiply inherited objects, but is
    /// provided in all cases so code can be written consistently since
    /// this call will always work.
    static ConcreteType * QueryImplementation(xr::COM::IUnknown * u)
    {
        return static_cast<ConcreteType*>(u->QueryInterface(ConcreteType::kIID));
    }
};
// ***************************************************************************************** - TYPE
/*! \brief base class to ease COM based (IUnknown) reference counting.
IUnknownHelper1 is intended for objects that implement IUnknown, a second
interface and their concrete type.
*/
// ***************************************************************************************** - TYPE
template <typename ConcreteType, typename interface0, class RefCountImpl = RefCounted>
class IUnknownHelper1 : public RefCountImpl, public interface0{
protected:
    typedef IUnknownHelper1<ConcreteType, interface0> base_type;
    IUnknownHelper1() : RefCountImpl(), interface0() {}
    virtual ~IUnknownHelper1() {}
public:
    /// Add a reference to this object
    void AddRef() const XR_OVERRIDE
    {
        RefCountImpl::AddRef();
    }
    /// Release a reference to this object (may delete the object)
    void Release() const XR_OVERRIDE
    {
        RefCountImpl::Release();
    }
    /// Unsafe call to get current reference count. Intended for testing.
    size_t    UnsafeGetRefCount() const
    {
        return RefCountImpl::UnsafeGetRefCount();
    }
    /// IUnknown implementation function
    xr::COM::IUnknown * QueryInterface(::xr::COM::InterfaceID id) XR_OVERRIDE
    {
        if(id == ConcreteType::kIID || id == xr::COM::IUnknown::kIID)
        {
            this->AddRef();
            return this;
        }
        else if(id == interface0::kIID)
        {
            this->AddRef();
            return static_cast<interface0 *>(this);
        }
        return nullptr;
    }
    /// Helper function to cast from IUnknown to concrete object
    /// this is only required for multiply inherited objects, but is
    /// provided in all cases so code can be written consistently since
    /// this call will always work.
    static ConcreteType * QueryImplementation(xr::COM::IUnknown * u)
    {
        return static_cast<ConcreteType*>(u->QueryInterface(ConcreteType::kIID));
    }
};
// ***************************************************************************************** - TYPE
/*! \brief base class to ease COM based (IUnknown) reference counting.
IUnknownHelper2 is intended for objects that implement IUnknown, 2 other
interfaces and their concrete type.
*/
// ***************************************************************************************** - TYPE
template <typename ConcreteType, typename interface0, typename interface1, class RefCountImpl = RefCounted>
class IUnknownHelper2 : public RefCountImpl, public interface0, public interface1 {
protected:
    typedef IUnknownHelper2<ConcreteType, interface0, interface1> base_type;
    IUnknownHelper2() : RefCountImpl(), interface0(), interface1() {}
    virtual ~IUnknownHelper2() {}
public:
    /// Add a reference to this object
    void AddRef() const XR_OVERRIDE
    {
        RefCountImpl::AddRef();
    }
    /// Release a reference to this object (may delete the object)
    void Release() const XR_OVERRIDE
    {
        RefCountImpl::Release();
    }
    /// Unsafe call to get current reference count. Intended for testing.
    size_t    UnsafeGetRefCount() const
    {
        return RefCountImpl::UnsafeGetRefCount();
    }
    /// IUnknown implementation function
    xr::COM::IUnknown * QueryInterface(::xr::COM::InterfaceID id) XR_OVERRIDE
    {
        if(id == ConcreteType::kIID || id == xr::COM::IUnknown::kIID || id == interface0::kIID)
        {
            this->AddRef();
            return static_cast<interface0*>(this);
        }
        else if(id == interface1::kIID)
        {
            this->AddRef();
            return static_cast<interface1 *>(this);
        }
        return nullptr;
    }
    /// Have to make pass through functions to handle resolving which IUknown to use.
    //@{
    template<typename T>
    T * interface_cast()
    {
        return  static_cast<interface0*>(this)->interface_cast<T>();
    }
    template<typename T>
    const T * interface_cast() const
    {
        return  static_cast<interface0*>(this)->interface_cast<T>();
    }
    template<typename T>
    void interface_cast(xr::Core::intrusive_ptr<T> &ip)
    {
        ip.AssumeReference( static_cast<T*>( static_cast<interface0*>(this)->QueryInterface(T::kIID)));
    }
    //@}
    /// Helper function to cast from IUnknown to concrete object
    /// this is only required for multiply inherited objects, but is
    /// provided in all cases so code can be written consistently since
    /// this call will always work.
    static ConcreteType * QueryImplementation(xr::COM::IUnknown * u)
    {
        return (ConcreteType*) (interface0*) u->QueryInterface(ConcreteType::kIID);
    }
};
// ***************************************************************************************** - TYPE
/*! \brief base class to ease COM based (IUnknown) reference counting.
IUnknownHelper3 is intended for objects that implement IUnknown, 3 other
interfaces and their concrete type.
*/
// ***************************************************************************************** - TYPE
template <typename ConcreteType, typename interface0, typename interface1, typename interface2, class RefCountImpl = RefCounted>
class IUnknownHelper3 : public RefCountImpl, public interface0, public interface1, public interface2 {
protected:
    typedef IUnknownHelper3<ConcreteType, interface0, interface1, interface2> base_type;
    IUnknownHelper3() : RefCountImpl(), interface0(), interface1(), interface2() {}
    virtual ~IUnknownHelper3() {}
public:
    /// Add a reference to this object
    void AddRef() const XR_OVERRIDE
    {
        RefCountImpl::AddRef();
    }
    /// Release a reference to this object (may delete the object)
    void Release() const XR_OVERRIDE
    {
        RefCountImpl::Release();
    }
    /// Unsafe call to get current reference count. Intended for testing.
    size_t    UnsafeGetRefCount() const
    {
        return RefCountImpl::UnsafeGetRefCount();
    }
    /// IUnknown implementation function
    xr::COM::IUnknown * QueryInterface(::xr::COM::InterfaceID id) XR_OVERRIDE
    {
        if(id == ConcreteType::kIID || id == xr::COM::IUnknown::kIID || id == interface0::kIID)
        {
            this->AddRef();
            return static_cast<interface0*>(this);
        }
        else if(id == interface1::kIID)
        {
            this->AddRef();
            return static_cast<interface1 *>(this);
        }
        else if(id == interface2::kIID)
        {
            this->AddRef();
            return static_cast<interface2 *>(this);
        }
        return nullptr;
    }
    /// Have to make pass through functions to handle resolving which IUknown to use.
    //@{
    template<typename T>
    T * interface_cast()
    {
        return  static_cast<interface0*>(this)->interface_cast<T>();
    }
    template<typename T>
    const T * interface_cast() const
    {
        return  static_cast<interface0*>(this)->interface_cast<T>();
    }
    template<typename T>
    void interface_cast(xr::Core::intrusive_ptr<T> &ip)
    {
        ip.AssumeReference( static_cast<T*>( static_cast<interface0*>(this)->QueryInterface(T::kIID)));
    }
    //@}
    /// Helper function to cast from IUnknown to concrete object
    /// this is only required for multiply inherited objects, but is
    /// provided in all cases so code can be written consistently since
    /// this call will always work.
    static ConcreteType * QueryImplementation(xr::COM::IUnknown * u)
    {
        return (ConcreteType*) (interface0*) u->QueryInterface(ConcreteType::kIID);
    }
};

// ***************************************************************************************** - TYPE
/*! \brief base class to ease COM based (IUnknown) reference counting.
IUnknownHelper4 is intended for objects that implement IUnknown, 4 other
interfaces and their concrete type.
*/
// ***************************************************************************************** - TYPE
template <typename ConcreteType, typename interface0, typename interface1, typename interface2, typename interface3, class RefCountImpl = RefCounted>
class IUnknownHelper4 : public RefCountImpl, public interface0, public interface1, public interface2, public interface3 {
protected:
    typedef IUnknownHelper4<ConcreteType, interface0, interface1, interface2, interface3> base_type;

    IUnknownHelper4() : RefCountImpl(), interface0(), interface1(), interface2(), interface3() {}
    virtual ~IUnknownHelper4() {}
public:
    /// Add a reference to this object
    void AddRef() const XR_OVERRIDE
    {
        RefCountImpl::AddRef();
    }
    /// Release a reference to this object (may delete the object)
    void Release() const XR_OVERRIDE
    {
        RefCountImpl::Release();
    }
    /// Unsafe call to get current reference count. Intended for testing.
    size_t    UnsafeGetRefCount() const
    {
        return RefCountImpl::UnsafeGetRefCount();
    }
    /// IUnknown implementation function
    xr::COM::IUnknown * QueryInterface(::xr::COM::InterfaceID id) XR_OVERRIDE
    {
        if(id == ConcreteType::kIID || id == xr::COM::IUnknown::kIID || id == interface0::kIID)
        {
            this->AddRef();
            return static_cast<interface0*>(this);
        }
        switch(id)
        {
        case interface1::kIID:        this->AddRef(); return static_cast<interface1 *>(this);
        case interface2::kIID:        this->AddRef(); return static_cast<interface2 *>(this);
        case interface3::kIID:        this->AddRef(); return static_cast<interface3 *>(this);
        default: return nullptr;
        }
    }
    /// Have to make pass through functions to handle resolving which IUknown to use.
    //@{
    template<typename T>
    T * interface_cast()
    {
        return  static_cast<interface0*>(this)->interface_cast<T>();
    }
    template<typename T>
    const T * interface_cast() const
    {
        return  static_cast<interface0*>(this)->interface_cast<T>();
    }
    template<typename T>
    void interface_cast(xr::Core::intrusive_ptr<T> &ip)
    {
        ip.AssumeReference( static_cast<T*>( static_cast<interface0*>(this)->QueryInterface(T::kIID)));
    }
    //@}
    /// Helper function to cast from IUnknown to concrete object
    /// this is only required for multiply inherited objects, but is
    /// provided in all cases so code can be written consistently since
    /// this call will always work.
    static ConcreteType * QueryImplementation(xr::COM::IUnknown * u)
    {
        return (ConcreteType*) (interface0*) u->QueryInterface(ConcreteType::kIID);
    }
};
}} // namespace
#endif //#ifndef XR_CORE_UNKNOWN_H
