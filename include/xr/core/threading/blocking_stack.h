// ######################################################################################### - FILE
/*! \file
Stack which blocks upon free and empty conditions. This is not always
necessary, but it is useful in many conditions. It is meant to be basic and
fast. If you want a more traditional stack, use STL.


This is not a lockless stack, but it remains in user space unless there is
contention. A lockless stack would require a significant amount of additional
work and would be slower in low contention situations. The issue is that you
need to update the head as well as the value and without a linked list they
are not contiguous. With a LL, then you need memory unless you force a common
base class which prevents repeated items and makes integral types more
cumbersome, but integral types are the most common types used.

If there we only supported pointers and excluded nullptr pointers we could do it
lockless-ly in truth by using an increment to deal with the head and a busy wait
on the value returning to nullptr (or a known invalid quantity). so atomic inc
followed  by potentially repeated atomic CAS. However in this case in the waiting
on empty stack situation one would still need to deal with the OS.

\note the implementation of stack and queue is practically identical a common
      base class is probably warranted, but not implemented yet.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_CONTAINERS_BLOCKING_STACK_H
#define XR_CORE_CONTAINERS_BLOCKING_STACK_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

#ifndef XR_CORE_DETAIL_QS_BASE_H
#include "xr/core/detail/qs_base.h"
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

// ***************************************************************************************** - TYPE
/*!  Blocking Stack implementation. Calling Pop when there are no
        entries will cause the calling thread to block until such time as
        entries are available. Conversely calling Push when it is full
        will cause the calling thread to block until an entry is available.
        There is no guarantee of fairness in terms of order during contention.
        */
// ***************************************************************************************** - TYPE
template<typename T>
class BlockingStack: private detail::QSBase{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until the items can be inserted. */
    // ------------------------------------------------------------------------------------  MEMBER
    BlockingStack(size_t stackEntries, const char * name = "Stack") : detail::QSBase(sizeof(T) * stackEntries, stackEntries, name), kStackSize(stackEntries)
    {
        mContents = (T*)GetBuffer();
        mTop = &mContents[kStackSize - 1];
    }
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~BlockingStack(){ }
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until an entry is available and return it. */
    // ------------------------------------------------------------------------------------  MEMBER
    T Pop();
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until an all items can be obtained. */
    // ------------------------------------------------------------------------------------  MEMBER
    void Pop(XR_OUT_COUNT(count) T * itemList, size_t count);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until the item can be inserted . */
    // ------------------------------------------------------------------------------------  MEMBER
    void Push(const T & item);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until the items can be inserted. */
    // ------------------------------------------------------------------------------------  MEMBER
    void Push(XR_IN_COUNT(count) const T * itemList, size_t count);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \internal used for testing purposes */
    // ------------------------------------------------------------------------------------  MEMBER
    inline size_t UnsafeGetAvailableCount()
    {
        return QSBase::UnsafeGetAvailableCount();
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \internal used for testing purposes */
    // ------------------------------------------------------------------------------------  MEMBER
    inline size_t UnsafeGetFreeCount()
    {
        return QSBase::UnsafeGetFreeCount();
    }
private:
    /// \internal Prevent assignment.
    BlockingStack & operator=( const BlockingStack & );

    // These are non standard until C++ 0x, but ensure the values do not have a constructor
    static_assert( std::is_pod<T>::value || std::is_integral<T>::value, "Contained types must be POD or integral");

    // ------------------------------------------------------------------------------------  MEMBER
    /*! \internal */
    // ------------------------------------------------------------------------------------  MEMBER
    T PopInternal();
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \internal */
    // ------------------------------------------------------------------------------------  MEMBER
    void PushInternal(const T &item);
    // ------------------------------------------------------------------------------------  MEMBER
    /// next ready instance to be consumed (head)
    // ------------------------------------------------------------------------------------  MEMBER
    T * mTop;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Useful constant
    // ------------------------------------------------------------------------------------  MEMBER
    const size_t kStackSize;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Array of entries
    // ------------------------------------------------------------------------------------  MEMBER
    T *mContents;
};

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
T BlockingStack<T>::PopInternal()
{
    return *(++mTop);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
void BlockingStack<T>::PushInternal(const T &item)
{
    *mTop-- = item;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
T BlockingStack<T>::Pop()
{
    T returnValue;
    RemovalLock();

    returnValue = PopInternal();

    RemovalUnLock(1);

    return returnValue;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
void BlockingStack<T>::Pop(XR_OUT_COUNT(count) T * itemList, size_t count)
{
    T * itemCurrent = itemList;
    while( count > 0)
    {
        size_t avialableCount = RemovalLock();

        // Loop count is the lesser of the available and the requested counts.
        const size_t loopCount = avialableCount < count ? avialableCount : count;

        for(size_t i = 0; i < loopCount; i++)
        {
            *itemCurrent++ = PopInternal( );
        }

        RemovalUnLock(loopCount);

        // Dec the insert count.
        count -= loopCount;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
void BlockingStack<T>::Push(const T &item)
{
    InsertionLock();
    PushInternal(item);
    InsertionUnLock(1);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
void BlockingStack<T>::Push(XR_IN_COUNT(count) const T * itemList, size_t count)
{
    const T * itemCurrent = itemList;
    while( count > 0)
    {
        size_t avialableCount = InsertionLock();

        // Loop count is the lesser of the available and the requested counts.
        const size_t loopCount = avialableCount < count ? avialableCount : count;

        for(size_t i = 0; i < loopCount; i++)
        {
            PushInternal( *itemCurrent++ );
        }

        InsertionUnLock(loopCount);

        // Dec the insert count.
        count -= loopCount;
    }
}
}}
#endif //#ifndef XR_CORE_CONTAINERS_BLOCKING_STACK_H
