// ######################################################################################### - FILE
/*! \file

Implements a simple hash map and hash set with memory usage as the greatest concern. It
uses linear probing and assumes a general max load of roughly 75%. It is optimized for 
insertion and iteration speed, and does no allocations in a normal insert (when properly 
reserved or primed).

Note that this is best used where the workload is known or repeatable, or where the hash is 
filled once. Otherwise this will re-hash the system quite frequently, which can offset the 
performance improvements.

\note that the hash map does not presently implement stl iterators / pair<>, iteration
can only be done via member functions. 
\note the "Remove" function is highly inefficient and is provided for non critical path use

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_CONTAINERS_HASH
#define XR_CORE_CONTAINERS_HASH

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
#include <utility>
#include <type_traits>
#include <algorithm> // std::max
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
template <typename value_type>
struct DefaultEmpty
{
    typedef typename std::add_const<value_type>::type const_value_type;
    typedef typename std::add_lvalue_reference<value_type>::type value_type_ref;
    typedef typename std::add_lvalue_reference<const_value_type>::type const_value_type_ref;

    inline value_type    operator()()              const { return (value_type)0;  }
    inline bool operator()(const_value_type_ref a) const { return a == 0; }
};
// ***************************************************************************************** - TYPE
/// 
// ***************************************************************************************** - TYPE
template <typename value_type>
struct DefaultEmpty<value_type *>
{
    typedef typename std::add_const<value_type>::type const_value_type;

    inline value_type *  operator()()           const { return nullptr;  }
    inline bool operator()(const_value_type* a) const { return a == nullptr; }
};
// ***************************************************************************************** - TYPE
/// 
// ***************************************************************************************** - TYPE
template <typename value_type>
struct DefaultEqualsTo
{
    typedef typename std::add_const<value_type>::type const_value_type;
    typedef typename std::add_lvalue_reference<value_type>::type value_type_ref;
    typedef typename std::add_lvalue_reference<const_value_type>::type const_value_type_ref;

    inline bool operator()(const_value_type_ref a, const_value_type_ref b) const { return a == b; }
};
// ***************************************************************************************** - TYPE
/// 
// ***************************************************************************************** - TYPE
template <typename value_type>
struct DefaultHashFunction
{
    typedef typename std::add_const<value_type>::type const_value_type;
    typedef typename std::add_lvalue_reference<value_type>::type value_type_ref;
    typedef typename std::add_lvalue_reference<const_value_type>::type const_value_type_ref;

    inline size_t operator()(const_value_type_ref a) const { return (size_t) a; }
};

// ***************************************************************************************** - TYPE
/// 
// ***************************************************************************************** - TYPE
enum HashInsertStatus {
    kInsertWasNew,
    kInsertReplacedExisting
};

// ***************************************************************************************** - TYPE
/// 
// ***************************************************************************************** - TYPE
template < typename key_type, typename value_type, class hash_functor = DefaultHashFunction<key_type>, class equals_functor = DefaultEqualsTo<key_type>,class empty_functor = DefaultEmpty<key_type> >
class HashMap
{
    static const size_t kDefaultInitialSize = 10;
public:
    typedef uintptr_t hash_type;
    typedef HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor> this_type;
    typedef typename std::add_const<key_type>::type const_key_type;
    typedef typename std::add_lvalue_reference<key_type>::type key_type_ref;
    typedef typename std::add_rvalue_reference<key_type>::type key_type_rref;
    typedef typename std::add_lvalue_reference<const_key_type>::type const_key_type_ref;
    typedef typename std::add_const<value_type>::type const_value_type;
    typedef typename std::add_lvalue_reference<value_type>::type value_type_ref;
    typedef typename std::add_rvalue_reference<value_type>::type value_type_rref;
    typedef typename std::add_lvalue_reference<const_value_type>::type const_value_type_ref;

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    HashMap(IAllocator & a = GetGeneralAllocator());
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~HashMap();
    // ------------------------------------------------------------------------------------  MEMBER
    /// If you want the previous value for any reason pass in a pointer and this will copy it 
    /// into the passed pointer.
    // ------------------------------------------------------------------------------------  MEMBER
    HashInsertStatus Insert(const_key_type_ref key, const_value_type_ref value, value_type * const outPrevValue = nullptr);
    // ------------------------------------------------------------------------------------  MEMBER
    /// This prevents the value copy on insert. pass returned pointer in for placement new
    /// and or init in place.
    // ------------------------------------------------------------------------------------  MEMBER
    value_type * Insert(const_key_type_ref key);
    // ------------------------------------------------------------------------------------  MEMBER
    /// The remove function is quite slow and requires a rehash of (up to) the entire container.
    // ------------------------------------------------------------------------------------  MEMBER
    void Remove(const_key_type_ref key);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    value_type * Find(const_key_type_ref Key);
    const_value_type * Find(const_key_type_ref Key) const;
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
    template <typename T> void ForEach(T lambdaOrFunctor) const;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    template <typename T> void ForEach(T lambdaOrFunctor);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    template <typename T> value_type * FindIf(T lambdaOrFunctor);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    template <typename T> const_value_type * FindIf(T lambdaOrFunctor) const;
private:
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void InsertMoveInternal(key_type_rref key, value_type_rref value);

    value_type   * mValues;
    key_type     * mKeys;
    size_t        mCapacity;
    size_t        mCount;
    uint32_t      mCollisionsRehashThreshold;
    uint32_t      mCurrentMaxCollisions;
    empty_functor  mEmpty;
    equals_functor mEquals;
    hash_functor   mHash;
    IAllocator   & mAllocator;

    // Prevent assignment
    this_type & operator= (const this_type & other);
};

// ***************************************************************************************** - TYPE
/// 
// ***************************************************************************************** - TYPE
template < typename key_type, class empty_functor = DefaultEmpty<key_type>, class equals_functor = DefaultEqualsTo<key_type>, class hash_functor = DefaultHashFunction<key_type> >
class HashSet
{
    static const size_t kDefaultInitialSize = 10;
public:
    typedef uintptr_t hash_type;
    typedef HashSet<key_type,empty_functor,equals_functor,hash_functor> this_type;
    typedef typename std::add_const<key_type>::type const_key_type;
    typedef typename std::add_lvalue_reference<key_type>::type key_type_ref;
    typedef typename std::add_rvalue_reference<key_type>::type key_type_rref;
    typedef typename std::add_lvalue_reference<const_key_type>::type const_key_type_ref;

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    HashSet(IAllocator & a = GetGeneralAllocator());
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~HashSet();
    // ------------------------------------------------------------------------------------  MEMBER
    /// If you want the previous value for any reason pass in a pointer and this will copy it 
    /// into the passed pointer.
    // ------------------------------------------------------------------------------------  MEMBER
    HashInsertStatus Insert(const_key_type_ref key);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    bool Find(const_key_type_ref  Key) const;
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
    template <typename T> void ForEach(T lambdaOrFunctor) const;
private:
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void InsertMoveInternal(key_type_rref key);

    key_type     * mKeys;
    size_t        mCapacity;
    size_t        mCount;
    uint32_t      mCollisionsRehashThreshold;
    uint32_t      mCurrentMaxCollisions;
    empty_functor  mEmpty;
    equals_functor mEquals;
    hash_functor   mHash;
    IAllocator    &mAllocator;

    // Prevent assignment
    this_type & operator= (const this_type & other);
};
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::HashMap(IAllocator & a) 
    : mValues(nullptr)
    , mKeys(nullptr)
    , mCapacity(0)
    , mCount(0)
    , mCollisionsRehashThreshold(0)
    , mCurrentMaxCollisions(0)
    , mEmpty()
    , mEquals()
    , mHash()
    , mAllocator(a)
{
    static_assert( std::is_pod<key_type>::value || std::is_move_assignable<key_type>::value, "Can only pass pod types (or movable types) as keys." );
    static_assert( std::is_pod<value_type>::value || std::is_move_assignable<value_type>::value, "Can only pass pod types (or movable types) as values." );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
void HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::Clear( bool releaseMem ) 
{
    if(releaseMem)
    {
        if(mValues != nullptr)
        {
            mAllocator.Free(mValues, 0, XR_FILE_LINE);
            mValues = nullptr;
        }
        if(mKeys != nullptr)
        {
            mAllocator.Free(mKeys, 0, XR_FILE_LINE);
            mKeys = nullptr;
        }
        mCapacity = 0;
        mCollisionsRehashThreshold = 0;
    }
    else
    {
        for(size_t i = 0; i < mCapacity; ++i)
        {
            mKeys[i] = mEmpty();
        }
    }
    mCount = 0;
    mCurrentMaxCollisions = 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::~HashMap() 
{
    Clear(true);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
void HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::InsertMoveInternal(key_type_rref key, value_type_rref value)
{
    const hash_type h0 = mHash(key);

    // Note that we do not worry about max collisions here. This is called internally during expansion
    // and expansion during expansion is not simple. The next Insert that causes a collision issue will 
    // expand again.
    for( uint32_t collitionCount = 0; ; ++collitionCount)
    {
        hash_type hc = h0 + collitionCount;
        size_t   i  = hc % mCapacity;
        if( mEmpty(mKeys[i]) )
        {
            mKeys[i] = std::move(key);
            mValues[i] = std::move(value);
            ++mCount;

            mCurrentMaxCollisions = std::max(++collitionCount, mCurrentMaxCollisions);
            return;
        }
        XR_ASSERT_DEBUG_FALSE(mEquals(mKeys[i], key));
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
void HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::Reserve(size_t expectedCount)
{
    const size_t newCapacity = GetHigherPrime( expectedCount += expectedCount/4 );

    if(newCapacity > mCapacity)
    {
        key_type         *oldKeys     = mKeys;
        value_type       *oldValues   = mValues;
        const size_t      oldCapacity = mCapacity;
        const size_t      oldCount    = mCount;

        mCapacity = newCapacity;
        // Investigate combining into single alloc.
        mKeys   = (key_type*)mAllocator.Alloc( sizeof(key_type) * mCapacity, XR_ALIGN_OF(key_type), xr::Core::kMemNormal, "SHM::Keys", XR_FILE_LINE );
        mValues = (value_type*)mAllocator.Alloc( sizeof(value_type) * mCapacity, XR_ALIGN_OF(value_type), xr::Core::kMemNormal, "SHM::Values", XR_FILE_LINE );

        mCollisionsRehashThreshold = std::max<uint32_t>(5, (uint32_t)(mCapacity/64));
        mCurrentMaxCollisions = 0;

        Clear(false);
        for(size_t i = 0; i < oldCapacity; ++i)
        {
            if(!mEmpty(oldKeys[i]))
            {
                InsertMoveInternal(std::move(oldKeys[i]), std::move(oldValues[i]));
            }
        }
        XR_ASSERT_DEBUG_EQ(oldCount, mCount);

        if(oldValues != nullptr)
            mAllocator.Free(oldValues, 0, XR_FILE_LINE);
        if(oldKeys != nullptr)
            mAllocator.Free(oldKeys, 0, XR_FILE_LINE);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
HashInsertStatus HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::Insert(const_key_type_ref key, const_value_type_ref value, value_type * const outPrevValue)
{
    const hash_type h0 = mHash(key);

    for(;;)
    {
        for( uint32_t collitionCount = 0; collitionCount < mCollisionsRehashThreshold; ++collitionCount)
        {
            hash_type hc = h0 + collitionCount;
            size_t   i  = hc % mCapacity;
            if( mEmpty(mKeys[i]) )
            {
                mKeys[i] = key;
                mValues[i] = value;
                ++mCount;

                mCurrentMaxCollisions = std::max(++collitionCount, mCurrentMaxCollisions);
                return kInsertWasNew;
            }
            if(mEquals(mKeys[i], key))
            {
                if(outPrevValue != nullptr)
                {
                    *outPrevValue = mValues[i];
                }

                mKeys[i] = key;
                mValues[i] = value;

                mCurrentMaxCollisions = std::max(++collitionCount, mCurrentMaxCollisions);
                return kInsertReplacedExisting;
            }
        }

        Reserve(mCapacity);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
value_type * HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::Insert(const_key_type_ref key)
{
    const hash_type h0 = mHash(key);

    value_type * p = nullptr;
    for(;;)
    {
        for( uint32_t collitionCount = 0; collitionCount < mCollisionsRehashThreshold; ++collitionCount)
        {
            hash_type hc = h0 + collitionCount;
            size_t   i  = hc % mCapacity;
            if( mEmpty(mKeys[i]) )
            {
                ++mCount;
                mKeys[i] = key;
                p = &mValues[i];

                mCurrentMaxCollisions = std::max(++collitionCount, mCurrentMaxCollisions);
                return p;
            }
            else if( mEquals(mKeys[i], key))
            {
                mKeys[i] = key;
                p = &mValues[i];

                mCurrentMaxCollisions = std::max(++collitionCount, mCurrentMaxCollisions);
                return p;
            }
        }

        Reserve(mCapacity);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
void HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::Remove(const_key_type_ref key)
{
    // Nothing here.
    if(mCount == 0)
    {
        return;
    }

    const hash_type h0 = mHash(key);
    for( size_t collitionCount = 0; collitionCount < mCurrentMaxCollisions; ++collitionCount)
    {
        const hash_type hc = h0 + collitionCount;
        const size_t i = hc % mCapacity;
        if(mEquals(key, mKeys[i]))
        {
            // Mark the empty
            size_t rollbackTo = i;
            mKeys[i] = mEmpty();
            --mCount;

            // Now try to collapse things down after the removal. Due to linear probing 
            // this is accomplished by shifting everything down until we hit an empty slot
            // or until we roll all the way back around.
            for( size_t counter = 1; counter < mCapacity; ++counter)
            {
                const size_t rollbackFrom = (i+counter) % mCapacity;

                if(mEmpty(mKeys[rollbackFrom]))
                {
                    // Nothing left to shift back
                    break;
                }
                const size_t belongsAt = mHash( mKeys[rollbackFrom] ) % mCapacity;

                // No wrap is simple.
                if( belongsAt > rollbackTo && belongsAt <= rollbackFrom)
                {
                    // Can't roll it behind where it belongs.
                    continue;
                }

                // Handle wrapping.
                if( rollbackFrom < rollbackTo)
                {
                    if( belongsAt <= rollbackFrom || belongsAt > rollbackTo )
                    {
                        // Can't roll it behind where it belongs (wrapped).
                        continue;
                    }
                }

                mKeys[rollbackTo]   = mKeys[rollbackFrom];
                mValues[rollbackTo]   = mValues[rollbackFrom];
                mKeys[rollbackFrom] = mEmpty();
                rollbackTo = rollbackFrom;
                
            }

            return;
        }
        if(mEmpty(mKeys[i]))
        {
            // Not found.
            return;
        }
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
template <typename T>
void HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::ForEach(T lambdaOrFunctor) const
{
    for(size_t i = 0; i < mCapacity; ++i)
    {
        if(!mEmpty(mKeys[i]))
        {
            const_key_type_ref k = mKeys[i];
            const_value_type_ref v = mValues[i];
            lambdaOrFunctor(k, v);
        }
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
template <typename T>
void HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::ForEach(T lambdaOrFunctor)
{
    for(size_t i = 0; i < mCapacity; ++i)
    {
        if(!mEmpty(mKeys[i]))
        {
            const_key_type_ref k = mKeys[i];
            lambdaOrFunctor(k, mValues[i]);
        }
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
template <typename T>
value_type * HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::FindIf(T lambdaOrFunctor)
{
    for(size_t i = 0; i < mCapacity; ++i)
    {
        if(!mEmpty(mKeys[i]))
        {
            const_key_type_ref k = mKeys[i];
            const_value_type_ref v = mValues[i];
            if(lambdaOrFunctor(k, v))
                return &mValues[i];
        }
    }
    return nullptr;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
template <typename T>
typename HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::const_value_type * HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::FindIf(T lambdaOrFunctor) const
{
    for(size_t i = 0; i < mCapacity; ++i)
    {
        if(!mEmpty(mValues[i]))
        {
            const_key_type_ref   k     = mKeys[i];
            const_value_type_ref v     = mValues[i];
            if(lambdaOrFunctor(k, v))
                return &mValues[i];
        }
    }
    return nullptr;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
typename HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::const_value_type * HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::Find(const_key_type_ref  key) const
{
    if(mCapacity == 0)
    {
        return nullptr;
    }

    const hash_type h0 = mHash(key);
    for( size_t collitionCount = 0; collitionCount < mCurrentMaxCollisions; ++collitionCount)
    {
        const hash_type hc = h0 + collitionCount;
        const size_t i = hc % mCapacity;
        if(mEquals(key, mKeys[i]))
        {
            return &mValues[i];
        }
        if(mEmpty(mKeys[i]))
        {
            return nullptr;
        }
    }

    return nullptr;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, typename value_type, class hash_functor, class equals_functor, class empty_functor>
value_type * HashMap<key_type,value_type,hash_functor,equals_functor,empty_functor>::Find(const_key_type_ref  key)
{
    if(mCapacity == 0)
    {
        return nullptr;
    }

    const hash_type h0 = mHash(key);
    for( size_t collitionCount = 0; collitionCount < mCurrentMaxCollisions; ++collitionCount)
    {
        const hash_type hc = h0 + collitionCount;
        const size_t i = hc % mCapacity;
        if(mEquals(key, mKeys[i]))
        {
            return &mValues[i];
        }
        if(mEmpty(mKeys[i]))
        {
            return nullptr;
        }
    }

    return nullptr;
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, class empty_functor, class equals_functor, class hash_functor>
HashSet<key_type,empty_functor,equals_functor,hash_functor>::HashSet(IAllocator & a) 
    : mKeys(nullptr)
    , mCapacity(0)
    , mCount(0)
    , mCollisionsRehashThreshold(0)
    , mCurrentMaxCollisions(0)
    , mEmpty()
    , mEquals()
    , mHash()
    , mAllocator(a)
{
    static_assert( std::is_pod<key_type>::value, "Can only pass pod types as keys." );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, class empty_functor, class equals_functor, class hash_functor>
void HashSet<key_type,empty_functor,equals_functor,hash_functor>::Clear( bool releaseMem ) 
{
    if(releaseMem)
    {
        if(mKeys != nullptr)
        {
            mAllocator.Free(mKeys, 0, XR_FILE_LINE);
            mKeys = nullptr;
        }
        mCapacity = 0;
        mCollisionsRehashThreshold = 0;
    }
    else
    {
        for(size_t i = 0; i < mCapacity; ++i)
        {
            mKeys[i] = mEmpty();
        }
    }
    mCount = 0;
    mCurrentMaxCollisions = 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, class empty_functor, class equals_functor, class hash_functor>
HashSet<key_type,empty_functor,equals_functor,hash_functor>::~HashSet() 
{
    Clear(true);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, class empty_functor, class equals_functor, class hash_functor>
void HashSet<key_type,empty_functor,equals_functor,hash_functor>::InsertMoveInternal(key_type_rref key)
{
    const hash_type h0 = mHash(key);
    
    for( uint32_t collitionCount = 0; ; ++collitionCount)
    {
        hash_type hc = h0 + collitionCount;
        size_t   i  = hc % mCapacity;
        if( mEmpty(mKeys[i]) )
        {
            mKeys[i] = std::move(key);
            ++mCount;

            mCurrentMaxCollisions      = std::max(++collitionCount, mCurrentMaxCollisions);
            return;
        }
        XR_ASSERT_DEBUG_FALSE(mEquals(mKeys[i], key));
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, class empty_functor, class equals_functor, class hash_functor>
void HashSet<key_type,empty_functor,equals_functor,hash_functor>::Reserve(size_t expectedCount)
{
    // According to wikipedia the efficiency of linear probing decreases quickly around 70-80%
    // targets a 75% load (to next prime, which decreases load).
    const size_t newCapacity = GetHigherPrime( expectedCount + expectedCount/4 );
    if(newCapacity > mCapacity)
    {
        key_type        * oldKeys     = mKeys;
        const size_t      oldCapacity = mCapacity;
        const size_t      oldCount    = mCount;

        mCapacity = newCapacity;
        // Investigate combining into single alloc.
        mKeys   = (key_type*)mAllocator.Alloc( sizeof(key_type) * mCapacity, XR_ALIGN_OF(key_type), xr::Core::kMemNormal, "SHM::Keys", XR_FILE_LINE );

        mCollisionsRehashThreshold = std::max<uint32_t>(5, (uint32_t)(mCapacity/64));
        mCurrentMaxCollisions =  0;

        Clear(false);
        for(size_t i = 0; i < oldCapacity; ++i)
        {
            if(!mEmpty(oldKeys[i]))
            {
                InsertMoveInternal(std::move(oldKeys[i]));
            }
        }
        XR_ASSERT_DEBUG_EQ(oldCount, mCount);

        if(oldKeys != nullptr)
            mAllocator.Free(oldKeys, 0, XR_FILE_LINE);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, class empty_functor, class equals_functor, class hash_functor>
HashInsertStatus HashSet<key_type,empty_functor,equals_functor,hash_functor>::Insert(const_key_type_ref key)
{
    const hash_type h0 = mHash(key);

    for(;;)
    {
        for( uint32_t collitionCount = 0; collitionCount < mCollisionsRehashThreshold; ++collitionCount)
        {
            hash_type hc = h0 + collitionCount;
            size_t   i  = hc % mCapacity;
            if( mEmpty(mKeys[i]) )
            {
                mKeys[i] = key;
                ++mCount;

                mCurrentMaxCollisions = std::max(++collitionCount, mCurrentMaxCollisions);
                return kInsertWasNew;
            }
            if(mEquals(mKeys[i], key))
            {
                mKeys[i] = key;
                mCurrentMaxCollisions = std::max(++collitionCount, mCurrentMaxCollisions);
                return kInsertReplacedExisting;
            }
        }

        Reserve(mCapacity);
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, class empty_functor, class equals_functor, class hash_functor>
template <typename T>
void HashSet<key_type,empty_functor,equals_functor,hash_functor>::ForEach(T lambdaOrFunctor) const
{
    for(size_t i = 0; i < mCapacity; ++i)
    {
        if(!mEmpty(mKeys[i]))
        {
            const_key_type k     = mKeys[i];
            lambdaOrFunctor(k);
        }
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename key_type, class empty_functor, class equals_functor, class hash_functor>
bool HashSet<key_type,empty_functor,equals_functor,hash_functor>::Find(const_key_type_ref  key) const
{
    if(mCapacity == 0)
    {
        return false;
    }

    const hash_type h0 = mHash(key);
    for( size_t collitionCount = 0; collitionCount < mCurrentMaxCollisions; ++collitionCount)
    {
        const hash_type hc = h0 + collitionCount;
        const size_t i = hc % mCapacity;
        if(mEquals(key, mKeys[i]))
        {
            return true;
        }
        if(mEmpty(mKeys[i]))
        {
            return false;
        }
    }

    return nullptr;
}

}}
#endif //include guard
