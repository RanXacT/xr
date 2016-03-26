// ######################################################################################### - FILE
/*! \file

Implements a simple stack class, it can be used as a simple slist as well. This works with 
standard range based for semantics and should be functional in stl based sort functions.

This is not thread safe. It also does not normally free memory, but removed items are placed
into a free pool to be reused.

Note that each link is individually allocated, but removing from the middle is does a linear 
search, and insertions are not implemented in the middle, and swapping (for sorting) isn't setup 
either.

I wonder what place this serves in truth as is. 
To function as a useful list it needs proper iterators for inserting / removing from the 
middle, but to be an efficient system it needs value buckets (with separate metadata) to 
optimize use of memory and cache when dealing with alignments and various size objects.
Current implementation is functional but in between. Given that general purpose list exists in 
STL. I suggest moving this to more of a buckets memory / cache efficient implementation, doing 
so should not significantly change the API or break compatibility.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_CONTAINERS_STACK_H
#define XR_CORE_CONTAINERS_STACK_H

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
XR_PUSH_SUPRESS_MSVC_WARNINGS(6001); // Using uninitialized memory '*mFreeHead' (Bogus)

namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/// This is a singly linked list. It can operate as a generic list in terms of adding
/// or can act as a stack. traversal follows lifo order as with a stack.
// ***************************************************************************************** - TYPE
template < typename value_type >
class Stack
{
public:
    typedef Stack<value_type> this_type;
    typedef typename std::add_const<value_type>::type const_value_type;
    typedef typename std::add_lvalue_reference<value_type>::type value_type_ref;
    typedef typename std::add_lvalue_reference<const_value_type>::type const_value_type_ref;

private:
    // ************************************************************************************* - TYPE
    /// Internal type to hold the value and *next
    // ************************************************************************************* - TYPE
    struct link_type
    {
        value_type  mValue;
        link_type  * mNext;
    };

public:

    // ************************************************************************************* - TYPE
    /// Internal type to interop with stl functions and range based for
    // ************************************************************************************* - TYPE
    struct iterator
    {
        typedef typename ::std::add_lvalue_reference<value_type>::type reference;

        inline iterator(link_type * l) : mLink(l)   {} 
        inline reference operator *()               { return mLink->mValue; }
        inline iterator& operator++()               { mLink = mLink->mNext; return *this; }
        inline bool operator != (const iterator &b) { return mLink != b.mLink; }

    private:
        link_type * mLink;
    };
    // ************************************************************************************* - TYPE
    /// Internal type to interop with stl functions and range based for
    // ************************************************************************************* - TYPE
    struct const_iterator
    {
        typedef typename std::add_const< typename std::add_lvalue_reference<value_type>::type>::type creference;

        inline const_iterator(link_type * l) : mLink(l) {} 
        inline creference operator *()              { return mLink->mValue; }
        inline const_iterator& operator++()         { mLink = mLink->mNext; return *this; }
        inline bool operator != (const const_iterator &b) { return mLink != b.mLink; }
    private:
        const link_type * mLink;
    };

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    Stack(IAllocator & a = GetGeneralAllocator());
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~Stack();

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void Push(const_value_type_ref value);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    value_type * Push();
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    const_value_type_ref Top();
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    value_type Pop();

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void Insert(const_value_type_ref value);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    value_type * Insert();
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    bool Contains(const_value_type_ref value);

    // ------------------------------------------------------------------------------------  MEMBER
    /// This searches for the first matching value and removes it.
    /// todo: add a version which takes an iterator to prevent search
    // ------------------------------------------------------------------------------------  MEMBER
    void Remove(const_value_type_ref value);

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
    template <typename T> void ForEach(T lambdaOrFunctor);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    iterator begin()                { return iterator(mHead); }
    const_iterator begin() const    { return const_iterator(mHead); }
    iterator end()                  { return iterator(nullptr); }
    const_iterator end() const      { return const_iterator(nullptr); }
private:
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void InsertFree(link_type * link);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    void ReSize(size_t expectedCount);

    size_t        mCount;
    size_t        mCapacity;
    link_type   * mHead;
    link_type   * mFreeHead;
    IAllocator  & mAllocator;

    // Prevent assignment
    this_type & operator= (const this_type & other);
};

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
Stack<value_type>::Stack(IAllocator & a) 
    : mCount(0)
    , mCapacity(0)
    , mHead(nullptr)
    , mFreeHead(nullptr)
    , mAllocator(a)
{
    static_assert( std::is_pod<value_type>::value, "Can only pass pod types as values." );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Stack<value_type>::Clear( bool releaseMem ) 
{
    if(releaseMem)
    {
        while(mHead != nullptr)
        {
            void * temp = mHead;
            mHead = mHead->mNext;
            mAllocator.Free(temp, 0, XR_FILE_LINE);
        }
        mHead = nullptr;

        while(mFreeHead != nullptr)
        {
            void * temp = mFreeHead;
            mFreeHead = mFreeHead->mNext;
            mAllocator.Free(temp, 0, XR_FILE_LINE);
        }
        mFreeHead = nullptr;
        mCapacity = 0;
    }
    mCount = 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
Stack<value_type>::~Stack() 
{
    Clear(true);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Stack<value_type>::Reserve(size_t expectedCount)
{
    for(size_t i = mCapacity; mCapacity < expectedCount; ++mCapacity)
    {
        link_type * temp = (link_type*)mAllocator.Alloc(sizeof(link_type), XR_ALIGN_OF(link_type), xr::Core::kMemNormal, "stack::link_type", XR_FILE_LINE);
        temp->mNext = mFreeHead;
        mFreeHead = temp;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Stack<value_type>::Insert(const_value_type_ref value)
{
    if(mFreeHead == nullptr)
    {
        mFreeHead = (link_type*)mAllocator.Alloc(sizeof(link_type), XR_ALIGN_OF(link_type), xr::Core::kMemNormal, "stack::link_type", XR_FILE_LINE);
        mFreeHead->mNext = nullptr;
    }
    
    link_type * next = mFreeHead;
    mFreeHead = mFreeHead->mNext;

    next->mValue = value;
    next->mNext = mHead;
    mHead = next;

    ++mCount;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Stack<value_type>::InsertFree(link_type * link)
{
    link->mNext = mFreeHead;
    mFreeHead = link;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
value_type * Stack<value_type>::Insert()
{
    if(mFreeHead == nullptr)
    {
        mFreeHead = (link_type*)mAllocator.Alloc(sizeof(link_type), XR_ALIGN_OF(link_type), xr::Core::kMemNormal, "stack::link_type", XR_FILE_LINE);
        mFreeHead->mNext = nullptr;
    }

    link_type * next = mFreeHead;
    mFreeHead = mFreeHead->mNext;

    value_type * p = &next->mValue;
    next->mNext = mHead;
    mHead = next;

    ++mCount;
    return p;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Stack<value_type>::Push(const_value_type_ref value)
{
    Insert(value);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
value_type * Stack<value_type>::Push()
{
    return Insert();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
typename Stack<value_type>::const_value_type_ref Stack<value_type>::Top()
{
    return mHead->mValue;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
value_type Stack<value_type>::Pop()
{
    link_type * l = mHead;
    value_type v = std::move(mHead->mValue);
    mHead = mHead->mNext;
    InsertFree(l);
    --mCount;
    return v;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
bool Stack<value_type>::Contains(const_value_type_ref value)
{
    const link_type * c = mHead;
    while (c != nullptr)
    {
        const_value_type_ref v = c->mValue;
        if (v == value)
        {
            return true;
        }
        c = c->mNext;
    }
    return false;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
template < typename T>
void Stack<value_type>::ForEach(T lambdaOrFunctor) const
{
    const link_type * c = mHead;
    while(c != nullptr)
    {
        const_value_type_ref v = c->mValue;
        lambdaOrFunctor(v);
        c = c->mNext;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
template < typename T>
void Stack<value_type>::ForEach(T lambdaOrFunctor)
{
    const link_type * c = mHead;
    while(c != nullptr)
    {
        lambdaOrFunctor(c->mValue);
        c = c->mNext;
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template < typename value_type>
void Stack<value_type>::Remove(const_value_type_ref value)
{
    if(mHead->mValue == value)
    {
        link_type * l = mHead;
        mHead = mHead->mNext;
        InsertFree(l);
        --mCount;
        return;
    }

    link_type * p = mHead;
    link_type * c = mHead->mNext;
    while(c != nullptr)
    {
        if(c->mValue == value)
        {
            // unlink.
            p->mNext = c->mNext;
            InsertFree(c);
            --mCount;
            return;
        }
        p = c;
        c = c->mNext;
    }

    // Nothing to remove
    return;
}

}}

XR_POP_SUPRESS_MSVC_WARNINGS();
#endif //include guard
