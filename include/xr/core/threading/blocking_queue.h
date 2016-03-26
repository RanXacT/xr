// ######################################################################################### - FILE
/*! \file
Queue which blocks upon free and empty conditions. This is not always
necessary, but it is useful in many conditions. It is meant to be basic and
fast. If you want a more traditional queue, use STL.


This is not a lockless queue, but it remains in user space unless there is
contention. A lockless queue would require a significant amount of additional
work and would be slower in low contention situations. The issue is that you
need to update the head as well as the value and without a linked list they
are not contiguous. With a LL, then you need memory unless you force a common
base class which prevents repeated items and makes integral types more
cumbersome, but integral types are the most common types used.

If there we only supported pointers and excluded nullptr pointers we could do it
locklessly in truth by using an increment to deal with the head and a busy wait
on the value returning to nullptr (or a known invalid quantity). so atomic inc
followed  by potentially repeated atomic CAS. However in this case in the waiting
on empty queue situation one would still need to deal with the OS.



\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_CONTAINERS_BLOCKING_QUEUE_H
#define XR_CORE_CONTAINERS_BLOCKING_QUEUE_H

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

/*######################################################################*/
/*!  Blocking Queue implementation. Calling Dequeue when there are no
        entries will cause the calling thread to block until such time as
        entries are available. Conversely calling Enqueue when it is full
        will cause the calling thread to block until an entry is available.
        There is no guarantee of fairness in terms of order during contention.
        */
/*######################################################################*/
template<typename T>
class BlockingQueue : private detail::QSBase{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until the items can be inserted. */
    // ------------------------------------------------------------------------------------  MEMBER
    BlockingQueue(size_t queueEntries, const char * name = "Queue") : detail::QSBase(sizeof(T) * queueEntries, queueEntries, name), kQueueSize(queueEntries)
    {
        mContents = (T*)GetBuffer();
        mDQ = (mContents);
        mNQ = (mContents);
        mWrap = mContents + kQueueSize;
    }
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~BlockingQueue() { }
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until an entry is available and return it. */
    // ------------------------------------------------------------------------------------  MEMBER
    T Dequeue();
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until the items can be removed. */
    // ------------------------------------------------------------------------------------  MEMBER
    void Dequeue(XR_OUT_COUNT(count) T * itemList, size_t count);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until the item can be inserted . */
    // ------------------------------------------------------------------------------------  MEMBER
    void Enqueue(const T & item);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Block until the items can be inserted. */
    // ------------------------------------------------------------------------------------  MEMBER
    void Enqueue(XR_IN_COUNT(count) const T * itemList, size_t count);

    // ------------------------------------------------------------------------------------  MEMBER
    /*! If a thread in the pool drops out, it needs to kick other threads to prevent deadlocks. */
    // ------------------------------------------------------------------------------------  MEMBER
    inline void Kick()
    {
        QSBase::Kick();
    }

    // ------------------------------------------------------------------------------------  MEMBER
    /*! \internal Used for testing purposes */
    // ------------------------------------------------------------------------------------  MEMBER
    inline size_t UnsafeGetAvailableCount()
    {
        return QSBase::UnsafeGetAvailableCount();
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \internal Used for testing purposes */
    // ------------------------------------------------------------------------------------  MEMBER
    inline size_t UnsafeGetFreeCount()
    {
        return QSBase::UnsafeGetFreeCount();
    }
private:
    /// \internal Prevent assignment.
    BlockingQueue & operator=( const BlockingQueue & );

    // These are non standard until C++ 0x, but ensure the values do not have a constructor
    static_assert( std::is_pod<T>::value || std::is_integral<T>::value, "Contained types must be POD or integral");

    // ------------------------------------------------------------------------------------  MEMBER
    /*! \internal */
    // ------------------------------------------------------------------------------------  MEMBER
    T DequeueInternal();
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \internal */
    // ------------------------------------------------------------------------------------  MEMBER
    void EnqueueInternal(const T &item);
    // ------------------------------------------------------------------------------------  MEMBER
    /// next ready instance to be consumed (head)
    // ------------------------------------------------------------------------------------  MEMBER
    T * mDQ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// put the next enqueued item here (tail)
    // ------------------------------------------------------------------------------------  MEMBER
    T *  mNQ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Wrap the queue here.
    // ------------------------------------------------------------------------------------  MEMBER
    T *  mWrap;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Wrap the queue here.
    // ------------------------------------------------------------------------------------  MEMBER
    const size_t kQueueSize;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Array of entries
    // ------------------------------------------------------------------------------------  MEMBER
    T *mContents;
};

// --------------------------------------------------------------------------------------  FUNCTION
/// This function assumes the caller has locked the ready queue.
/// It returns a T to be run (and assumes the caller is going
/// to execute it)
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
T BlockingQueue<T>::DequeueInternal()
{
    // Get the return value
    T * returnValue = mDQ;

    // Inc / rap
    T* nextDQ = (mDQ + 1);
    mDQ = ( nextDQ >= mWrap) ? mContents : nextDQ;

    return *returnValue;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
void BlockingQueue<T>::EnqueueInternal(const T &item)
{
    // Fill in the value
    *mNQ = item;

    // Inc / rap
    T* nextNQ = (mNQ+1);
    mNQ = ( nextNQ >= mWrap) ?  mContents : nextNQ;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
T BlockingQueue<T>::Dequeue()
{
    T returnValue;
    RemovalLock();

    returnValue = DequeueInternal();

    RemovalUnLock(1);

    return returnValue;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
void BlockingQueue<T>::Dequeue(XR_OUT_COUNT(count) T * itemList, size_t count)
{
    T * itemCurrent = itemList;
    while( count > 0)
    {
        size_t avialableCount = RemovalLock();

        // Loop count is the lesser of the available and the requested counts.
        const size_t loopCount = avialableCount < count ? avialableCount : count;

        for(size_t i = 0; i < loopCount; i++)
        {
            *itemCurrent++ = DequeueInternal( );
        }

        RemovalUnLock(loopCount);

        // Dec the insert count.
        count -= loopCount;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
void BlockingQueue<T>::Enqueue(const T &item)
{
    InsertionLock();
    EnqueueInternal(item);
    InsertionUnLock(1);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
void BlockingQueue<T>::Enqueue(XR_IN_COUNT(count) const T * itemList, size_t count)
{
    const T * itemCurrent = itemList;
    while( count > 0)
    {
        size_t avialableCount = InsertionLock();

        // Loop count is the lesser of the available and the requested counts.
        const size_t loopCount = avialableCount < count ? avialableCount : count;

        for(size_t i = 0; i < loopCount; i++)
        {
            EnqueueInternal( *itemCurrent++ );
        }

        InsertionUnLock(loopCount);

        // Dec the insert count.
        count -= loopCount;
    }
}
}} // namespace
#endif //#ifndef XR_CORE_CONTAINERS_BLOCKING_QUEUE_H
