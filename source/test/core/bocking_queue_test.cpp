// ######################################################################################### - FILE
/*!

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_CONTAINERS_BLOCKING_QUEUE_H
#include "xr/core/threading/blocking_queue.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_THREADING_THREAD_H
#include "xr/core/threading/thread.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif

#include "xr/core/console.h"
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Queue )

void SimpleTest(size_t kQueueSize)
{
    xr::Core::BlockingQueue<size_t> test(kQueueSize);

    size_t *values = XR_NEW("Test") size_t[kQueueSize];
    for(size_t i = 0; i < kQueueSize; i++)
    {
        values[i] = i;
    }

    // Add one at a time.
    for(size_t i = 0; i < kQueueSize; i++)
    {
        test.Enqueue(values[i]);
    }

    for(size_t i = 0; i < kQueueSize; i++)
    {
        size_t d = test.Dequeue();
        XR_ASSERT_ALWAYS_EQ( d, i);
    }

    // Add all at once.
    test.Enqueue(values, kQueueSize);

    for(size_t i = 0; i < kQueueSize; i++)
    {
        size_t d = test.Dequeue();
        XR_ASSERT_ALWAYS_EQ( d, i);
    }

    XR_DELETE_ARRAY(values);
}
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( basic )
{
    xr::Core::BlockingQueue<int32_t> test(16);

    int32_t test1 = 1;
    int32_t test2 = 2;

    test.Enqueue(test1);
    size_t d = test.Dequeue();
    XR_ASSERT_ALWAYS_EQ(  d, 1);

    test.Enqueue(test1);
    test.Enqueue(test2);
    d = test.Dequeue();
    XR_ASSERT_ALWAYS_EQ( d, 1);
    d = test.Dequeue();
    XR_ASSERT_ALWAYS_EQ( d, 2);

    SimpleTest(1);
    SimpleTest(2);
    SimpleTest(3);
    SimpleTest(4);
}

template<typename T, size_t kCount>
class ThreadDequeue : public xr::Core::Thread{
public:
    ThreadDequeue(xr::Core::BlockingQueue<T> *queue): xr::Core::Thread("decker")
    {
        mCompleted = false;
        mQueue = queue;
    }

    uintptr_t Run()
    {
        for(size_t i = 0; i < kCount; i++)
        {
            mItems[i] = mQueue->Dequeue();
        }

        mCompleted = true;
        return 0;
    }

    volatile bool   mCompleted;
    T      mItems[kCount];

    xr::Core::BlockingQueue<T> *mQueue;
};

template<typename T, size_t kCount>
class ThreadEnqueue : public xr::Core::Thread{
public:
    ThreadEnqueue(T * Items, xr::Core::BlockingQueue<T> *queue): xr::Core::Thread("encker")
    {
        mCompleted = false;
        for(size_t i = 0; i < kCount; i++)
        {
            mItems[i] = Items[i];
        }
        mQueue = queue;
    }

    uintptr_t Run()
    {
        for(size_t i = 0; i < kCount; i++)
        {
            mQueue->Enqueue(mItems[i]);
        }

        mCompleted = true;
        return 0;
    }

    volatile bool   mCompleted;
    T      mItems[kCount];

    xr::Core::BlockingQueue<T> *mQueue;
};


template<typename T, size_t kCount, size_t kBatchCount>
class ThreadEnqueueBatch : public xr::Core::Thread{
public:
    ThreadEnqueueBatch(T * Items, xr::Core::BlockingQueue<T> *queue): xr::Core::Thread("enckerBatch")
    {
        mCompleted = false;
        for(size_t i = 0; i < kCount; i++)
        {
            mItems[i] = Items[i];
        }
        mQueue = queue;
    }

    uintptr_t Run()
    {
        size_t numItemsLeft = kCount;
        T * itemCurrent = mItems;
        while( numItemsLeft > 0)
        {
            // Loop count is the lesser of the available and the requested counts.
            const size_t loopCount = kBatchCount < numItemsLeft ? kBatchCount : numItemsLeft;

            mQueue->Enqueue(itemCurrent, loopCount);
            itemCurrent += loopCount;

            // Dec the insert count.
            numItemsLeft -= loopCount;
        }


        mCompleted = true;
        return 0;
    }

    volatile bool   mCompleted;
    T      mItems[kCount];

    xr::Core::BlockingQueue<T> *mQueue;
};

template<typename T, size_t kCount, size_t kBatchCount>
class ThreadDequeueBatch : public xr::Core::Thread{
public:
    ThreadDequeueBatch(xr::Core::BlockingQueue<T> *queue): xr::Core::Thread("denckrBatch")
    {
        mCompleted = false;
        mQueue = queue;
    }

    uintptr_t Run()
    {
        size_t numItemsLeft = kCount;
        T * itemCurrent = mItems;
        while( numItemsLeft > 0)
        {
            // Loop count is the lesser of the available and the requested counts.
            const size_t loopCount = kBatchCount < numItemsLeft ? kBatchCount : numItemsLeft;

            mQueue->Dequeue(itemCurrent, loopCount);
            itemCurrent += loopCount;

            // Dec the insert count.
            numItemsLeft -= loopCount;
        }

        mCompleted = true;
        return 0;
    }

    volatile bool   mCompleted;
    T      mItems[kCount];

    xr::Core::BlockingQueue<T> *mQueue;
};

template <size_t kNumThreads, size_t kQueueSize, size_t kLoadCount>
void ThreadTest()
{
    xr::Core::BlockingQueue<size_t> test(kQueueSize);

    size_t values[kLoadCount];
    for(size_t i = 0; i < kLoadCount; i++)
    {
        values[i] = i;
    }

    ThreadDequeue<size_t, kLoadCount> * dequeues[kNumThreads];
    ThreadEnqueue<size_t, kLoadCount> * enqueues[kNumThreads];

    for(size_t i = 0; i < kNumThreads; i++)
    {
        dequeues[i] = XR_NEW( "dequeueThread" ) ThreadDequeue<size_t, kLoadCount>(&test);
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        enqueues[i] = XR_NEW( "enqueueThread" ) ThreadEnqueue<size_t, kLoadCount>(values, &test);
    }


    for(size_t i = 0; i < kNumThreads; i++)
    {
        dequeues[i]->Start();
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        enqueues[i]->Start();
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        enqueues[i]->Join();
        XR_ASSERT_ALWAYS_EQ(enqueues[i]->mCompleted, true);
        XR_DELETE(enqueues[i]);
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        dequeues[i]->Join();
        XR_ASSERT_ALWAYS_EQ(dequeues[i]->mCompleted, true);
        XR_DELETE(dequeues[i]);
    }


    // If any items were lost or doubled, then the counts will be off
    // (or we would have soft locked above)
    XR_ASSERT_ALWAYS_EQ(test.UnsafeGetAvailableCount(), 0);
    XR_ASSERT_ALWAYS_EQ(test.UnsafeGetFreeCount(), kQueueSize);
}

template <size_t kNumThreads, size_t kQueueSize, size_t kLoadCount, size_t kBatch1, size_t kBatch2>
void ThreadTestBatch()
{
    xr::Core::BlockingQueue<size_t> test(kQueueSize);

    size_t values[kLoadCount];
    for(size_t i = 0; i < kQueueSize; i++)
    {
        values[i] = i;
    }

    ThreadDequeueBatch<size_t, kLoadCount, kBatch1> * dequeues[kNumThreads];
    ThreadEnqueueBatch<size_t, kLoadCount, kBatch2> * enqueues[kNumThreads];

    for(size_t i = 0; i < kNumThreads; i++)
    {
        dequeues[i] = XR_NEW( "dequeueThread" ) ThreadDequeueBatch<size_t, kLoadCount, kBatch1>(&test);
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        enqueues[i] = XR_NEW( "dequeueThread" ) ThreadEnqueueBatch<size_t, kLoadCount, kBatch2>(values, &test);
    }


    for(size_t i = 0; i < kNumThreads; i++)
    {
        dequeues[i]->Start();
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        enqueues[i]->Start();
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        dequeues[i]->Join();
        XR_ASSERT_ALWAYS_EQ(dequeues[i]->mCompleted, true);
        XR_DELETE(dequeues[i]);
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        enqueues[i]->Join();
        XR_ASSERT_ALWAYS_EQ(enqueues[i]->mCompleted, true);
        XR_DELETE(enqueues[i]);
    }

    // If any items were lost or doubled, then the counts will be off
    // (or we would have soft locked above)
    XR_ASSERT_ALWAYS_EQ(test.UnsafeGetAvailableCount(), 0);
    XR_ASSERT_ALWAYS_EQ(test.UnsafeGetFreeCount() , kQueueSize);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( basicThreadedTests )
{
    // This doesn't actually verify results, it just looks for asserts.
    ThreadTest<1,2,8>();

    ThreadTest<16,1, 100>();
    ThreadTest<16,2, 100>();
    ThreadTest<16,100, 100>();

    // Having a queue size of 1 forces a lot of full / empty conditions.
    ThreadTest<50,1, 500>();
    ThreadTest<50,2, 500>();
    ThreadTest<50,137, 1049>();

    // Now test with batch Adds
    ThreadTestBatch<1,2,8, 3, 4>();

    ThreadTestBatch<16,1, 100, 25, 33>();
    ThreadTestBatch<16,2, 101, 13, 29>();
    ThreadTestBatch<16,100, 100, 100, 100>();

    // Having a queue size of 2 forces a lot of full / empty conditions.
    ThreadTestBatch<50,1, 500, 17, 21>();
    ThreadTestBatch<50,2, 500, 100, 109>();
    ThreadTestBatch<50,137, 1049, 13, 17>();
}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
