// ######################################################################################### - FILE
/*! \file
Thread Local storage implementation.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_THREADING_TLS_H
#define XR_CORE_THREADING_TLS_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

#include <type_traits>

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
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
//@{
/*! internal implementation of thread local storage (to keep it out of
    the header file.
*/
class ThreadLocalStorageBase{
protected:
    ThreadLocalStorageBase();
    ~ThreadLocalStorageBase();
    /// implementation functions
    uintptr_t GetValueInternal();
    /// implementation functions
    void      SetValueInternal(uintptr_t value);
private:
    uintptr_t mTlsId;
    uintptr_t mDefaultValue;
};
//@}
}

// ***************************************************************************************** - TYPE
/*! \brief Simple, typesafe thread local storage container.
    Note: that despite the ease of use, TLS should be used sparingly as it is an overhead
    incurred by all threads created, even if only a few use any value.
    \tparam T type of data. Must be <= size of uintptr_t, and not have a constructor (be POD)
    Note: This will generate an error if an object is directly used, use pointers to objects instead.
    \code
    ThreadLocalStorage<uint32_t> willWork(5);     < good!
    ThreadLocalStorage<IStorage> willFail(blah);  < bad!
    ThreadLocalStorage<IStorage*>willWork(nullptr);  < good!
    \endcode
*/
// ***************************************************************************************** - TYPE
template <typename T>
class ThreadLocalStorage : private detail::ThreadLocalStorageBase{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Template constructor only manages type error detection work is done in internal base class
    // ------------------------------------------------------------------------------------  MEMBER
    inline ThreadLocalStorage() : detail::ThreadLocalStorageBase()
    {
        //This can only contain at most sizeof(void*) bytes
        static_assert(sizeof(T) <= sizeof(uintptr_t), "Can only store values at most the size of a pointer");
        // These are non standard until C++ 0x, but ensure the values do not have a constructor
        static_assert( std::is_pod<T>::value || std::is_integral<T>::value, "Contained types must be POD or integral");
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Not virtual, this object is not intended to be derived from
    // ------------------------------------------------------------------------------------  MEMBER
    ~ThreadLocalStorage(){}
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief returns the thread local value of this object
    // ------------------------------------------------------------------------------------  MEMBER
    T    GetValue()       {return (T)(GetValueInternal());}
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Sets the thread local value of this object
    // ------------------------------------------------------------------------------------  MEMBER
    void SetValue(T value){SetValueInternal((uintptr_t)(value));}
};
}} // namespace
#endif //#ifndef XR_CORE_THREADING_TLS_H
