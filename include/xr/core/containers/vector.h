// ######################################################################################### - FILE
/*! \file

Implements a simple vector class. This works with standard range based for semantics and 
should be functional in stl based sort functions.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_CONTAINERS_VECTOR
#define XR_CORE_CONTAINERS_VECTOR

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_MEM_UTILS_H
#include "xr/core/mem_utils.h"
#endif
#include <utility>
#include <type_traits>
// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {
    size_t GetHigherPrime(size_t x);
}}
// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/// 
// ***************************************************************************************** - TYPE
template < typename value_type >
class Vector
{
public:
    // helper typedefs
    typedef Vector<value_type> this_type;
    typedef typename std::add_const<value_type>::type const_value_type;
    typedef typename std::add_lvalue_reference<value_type>::type value_type_ref;
    typedef typename std::add_lvalue_reference<const_value_type>::type const_value_type_ref;

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    Vector(IAllocator & a = GetGeneralAllocator());
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~Vector();
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void Insert(const_value_type_ref value);
    // ------------------------------------------------------------------------------------  MEMBER
    // Insert function to avoid copies. Use with for placement new, or just populate explicitly
    // ------------------------------------------------------------------------------------  MEMBER
    value_type * Insert();
    // ------------------------------------------------------------------------------------  MEMBER
    // pointer should be to the value in the vector (this is not a search)
    // ------------------------------------------------------------------------------------  MEMBER
    void Remove(const_value_type *value);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void Clear(bool releaseMemory = false);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void Reserve(size_t expectedEntryCount);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline size_t Count() {return mCount;}
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline const_value_type * GetArray() {return mValues;}
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    template <typename T> void ForEach(T lambdaOrFunctor) const;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    template <typename T> void ForEach(T lambdaOrFunctor);
    // ------------------------------------------------------------------------------------  MEMBER
    /// interoperability with stl.
    // ------------------------------------------------------------------------------------  MEMBER
    inline value_type * begin()             { return &mValues[0]; }
    inline const_value_type * begin() const { return &mValues[0]; }
    inline value_type * end()               { return &mValues[mCount]; }
    inline const_value_type * end() const   { return &mValues[mCount]; }

private:
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void ReSize(size_t expectedCount);

    size_t        mCount;
    size_t        mCapacity;
    value_type  * mValues;
    IAllocator  & mAllocator;
    // Prevent assignment
    this_type & operator= (const this_type & other);
};

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
Vector<value_type>::Vector(IAllocator&a) 
    : mCount(0)
    , mCapacity(0)
    , mValues(nullptr)
    , mAllocator(a)
{
    static_assert( std::is_pod<value_type>::value, "Can only pass pod types as values." );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Vector<value_type>::Clear( bool releaseMem ) 
{
    if(releaseMem)
    {
        if(mValues != nullptr)
        {
            mAllocator.Free(mValues, 0, XR_FILE_LINE);
            mValues = nullptr;
        }
        mCapacity = 0;
    }
    mCount = 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
Vector<value_type>::~Vector() 
{
    Clear(true);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Vector<value_type>::Reserve(size_t expectedCount)
{
    const size_t newCapacity = expectedCount;
    if(newCapacity > mCapacity)
    {
        value_type      * oldValues   = mValues;
        const size_t      oldCapacity = mCapacity;

        mCapacity = newCapacity;
        mValues = (value_type*)mAllocator.Alloc(sizeof(value_type) * newCapacity, XR_ALIGN_OF(value_type), xr::Core::kMemNormal, "Vector::Values", XR_FILE_LINE);

        MemCopy8(mValues, oldValues, sizeof(value_type) * oldCapacity);

        if(oldValues != nullptr)
            mAllocator.Free(oldValues, 0, XR_FILE_LINE);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Vector<value_type>::Insert(const_value_type_ref value)
{
    if(mCount >= mCapacity)
    {
        Reserve( ::xr::Core::GetHigherPrime( mCapacity ) );
    }
    
    mValues[mCount] = value;
    ++mCount;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
value_type * Vector<value_type>::Insert()
{
    if(mCount >= mCapacity)
    {
        Reserve( ::xr::Core::GetHigherPrime( mCapacity ) );
    }

    value_type * p = &mValues[mCount];
    ++mCount;
    return p;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Vector<value_type>::Remove(const_value_type *value)
{
    const_value_type *b = begin();
    value_type *i = const_cast<value_type*>(value);
    const_value_type *e = end();
    if(i >= b && i < e)
    {
        --e;
        for(; i < e; ++i)
        {
            i[0] = std::move(i[1]);
        }
        --mCount;
    }
    else
    {
        XR_ALWAYS_UNEXPECTED();
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
template < typename T>
void Vector<value_type>::ForEach(T lambdaOrFunctor) const
{
    for(size_t i = 0; i < mCount; ++i)
    {
        const_value_type_ref v = mValues[i];
        lambdaOrFunctor(v);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
template < typename T>
void Vector<value_type>::ForEach(T lambdaOrFunctor)
{
    for(size_t i = 0; i < mCount; ++i)
    {
        lambdaOrFunctor(mValues[i]);
    }
}

}}
#endif //include guard
