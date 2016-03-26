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
#ifndef XR_CORE_CONTAINERS_BLOCKING_STACK_H
#include "xr/core/threading/blocking_stack.h"
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
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Stack )

void SimpleTest(size_t kStackSize)
{
    xr::Core::BlockingStack<size_t> test(kStackSize);

    size_t *values = XR_NEW("Test") size_t[kStackSize];
    for(size_t i = 0; i < kStackSize; i++)
    {
        values[i] = i;
    }

    // Add one at a time.
    for(size_t i = 0; i < kStackSize; i++)
    {
        test.Push(values[i]);
    }

    for(size_t i = 0; i < kStackSize; i++)
    {
        size_t p = test.Pop();
        XR_ASSERT_ALWAYS_EQ( p, kStackSize - i - 1);
    }

    // Add all at once.
    test.Push(values, kStackSize);

    for(size_t i = 0; i < kStackSize; i++)
    {
        size_t p = test.Pop();
        XR_ASSERT_ALWAYS_EQ( p, kStackSize - i - 1);
    }

    XR_DELETE_ARRAY(values);
}
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( basic )
{
    xr::Core::BlockingStack<int32_t> test(16);

    int32_t test1 = 1;
    int32_t test2 = 2;

    test.Push(test1);
    size_t p = test.Pop();
    XR_ASSERT_ALWAYS_EQ(  p, 1);

    test.Push(test1);
    test.Push(test2);
    p = test.Pop();
    XR_ASSERT_ALWAYS_EQ( p, 2);
    p = test.Pop();
    XR_ASSERT_ALWAYS_EQ( p, 1);

    SimpleTest(1);
    SimpleTest(2);
    SimpleTest(3);
    SimpleTest(4);
}

template<typename T, size_t kCount>
class ThreadPop : public xr::Core::Thread{
public:
    ThreadPop(xr::Core::BlockingStack<T> *stack): xr::Core::Thread("popper")
    {
        mCompleted = false;
        mStack = stack;
    }

    uintptr_t Run()
    {
        for(size_t i = 0; i < kCount; i++)
        {
            mItems[i] = mStack->Pop();
        }

        mCompleted = true;
        return 0;
    }

    bool   mCompleted;
    T      mItems[kCount];

    xr::Core::BlockingStack<T> *mStack;
};

template<typename T, size_t kCount>
class ThreadPush : public xr::Core::Thread{
public:
    ThreadPush(T * Items, xr::Core::BlockingStack<T> *stack): xr::Core::Thread("Pusher")
    {
        mCompleted = false;
        for(size_t i = 0; i < kCount; i++)
        {
            mItems[i] = Items[i];
        }
        mStack = stack;
    }

    uintptr_t Run()
    {
        for(size_t i = 0; i < kCount; i++)
        {
            mStack->Push(mItems[i]);
        }

        mCompleted = true;
        return 0;
    }

    bool   mCompleted;
    T      mItems[kCount];

    xr::Core::BlockingStack<T> *mStack;
};


template<typename T, size_t kCount, size_t kBatchCount>
class ThreadPushBatch : public xr::Core::Thread{
public:
    ThreadPushBatch(T * Items, xr::Core::BlockingStack<T> *Stack): xr::Core::Thread("pusher")
    {
        mCompleted = false;
        for(size_t i = 0; i < kCount; i++)
        {
            mItems[i] = Items[i];
        }
        mStack = Stack;
    }

    uintptr_t Run()
    {
        size_t numItemsLeft = kCount;
        T * itemCurrent = mItems;
        while( numItemsLeft > 0)
        {
            // Loop count is the lesser of the available and the requested counts.
            const size_t loopCount = kBatchCount < numItemsLeft ? kBatchCount : numItemsLeft;

            mStack->Push(itemCurrent, loopCount);
            itemCurrent += loopCount;

            // Dec the insert count.
            numItemsLeft -= loopCount;
        }


        mCompleted = true;
        return 0;
    }

    bool   mCompleted;
    T      mItems[kCount];

    xr::Core::BlockingStack<T> *mStack;
};

template<typename T, size_t kCount, size_t kBatchCount>
class ThreadPopBatch : public xr::Core::Thread{
public:
    ThreadPopBatch(xr::Core::BlockingStack<T> *Stack): xr::Core::Thread("popper")
    {
        mCompleted = false;
        mStack = Stack;
    }

    uintptr_t Run()
    {
        size_t numItemsLeft = kCount;
        T * itemCurrent = mItems;
        while( numItemsLeft > 0)
        {
            // Loop count is the lesser of the available and the requested counts.
            const size_t loopCount = kBatchCount < numItemsLeft ? kBatchCount : numItemsLeft;

            mStack->Pop(itemCurrent, loopCount);
            itemCurrent += loopCount;

            // Dec the insert count.
            numItemsLeft -= loopCount;
        }

        mCompleted = true;
        return 0;
    }

    bool   mCompleted;
    T      mItems[kCount];

    xr::Core::BlockingStack<T> *mStack;
};

template <size_t kNumThreads, size_t kStackSize, size_t kLoadCount>
void ThreadTest()
{
    xr::Core::BlockingStack<size_t> test(kStackSize);

    size_t values[kLoadCount];
    for(size_t i = 0; i < kStackSize; i++)
    {
        values[i] = i;
    }

    ThreadPop<size_t, kLoadCount> * pops[kNumThreads];
    ThreadPush<size_t, kLoadCount> * pushes[kNumThreads];

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pops[i] = XR_NEW( "popperThread" ) ThreadPop<size_t, kLoadCount>(&test);
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pushes[i] = XR_NEW( "popperThread" ) ThreadPush<size_t, kLoadCount>(values, &test);
    }


    for(size_t i = 0; i < kNumThreads; i++)
    {
        pops[i]->Start();
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pushes[i]->Start();
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pops[i]->Join();
        XR_ASSERT_ALWAYS_EQ(pops[i]->mCompleted, true);
        XR_DELETE(pops[i]);
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pushes[i]->Join();
        XR_ASSERT_ALWAYS_EQ(pushes[i]->mCompleted, true);
        XR_DELETE(pushes[i]);
    }

    // If any items were lost or doubled, then the counts will be off
    // (or we would have soft locked above)
    XR_ASSERT_ALWAYS_EQ(test.UnsafeGetAvailableCount(), 0);
    XR_ASSERT_ALWAYS_EQ(test.UnsafeGetFreeCount(), kStackSize);
}

template <size_t kNumThreads, size_t kStackSize, size_t kLoadCount, size_t kBatch1, size_t kBatch2>
void ThreadTestBatch()
{
    xr::Core::BlockingStack<size_t> test(kStackSize);

    size_t values[kLoadCount];
    for(size_t i = 0; i < kStackSize; i++)
    {
        values[i] = i;
    }

    ThreadPopBatch<size_t, kLoadCount, kBatch1> * pops[kNumThreads];
    ThreadPushBatch<size_t, kLoadCount, kBatch2> * pushes[kNumThreads];

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pops[i] = XR_NEW( "popThread" ) ThreadPopBatch<size_t, kLoadCount, kBatch1>(&test);
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pushes[i] = XR_NEW( "batchPush" ) ThreadPushBatch<size_t, kLoadCount, kBatch2>(values, &test);
    }


    for(size_t i = 0; i < kNumThreads; i++)
    {
        pops[i]->Start();
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pushes[i]->Start();
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pops[i]->Join();
        XR_ASSERT_ALWAYS_EQ(pops[i]->mCompleted, true);
        XR_DELETE(pops[i]);
    }

    for(size_t i = 0; i < kNumThreads; i++)
    {
        pushes[i]->Join();
        XR_ASSERT_ALWAYS_EQ(pushes[i]->mCompleted, true);
        XR_DELETE(pushes[i]);
    }

    // If any items were lost or doubled, then the counts will be off
    // (or we would have soft locked above)
    XR_ASSERT_ALWAYS_EQ(test.UnsafeGetAvailableCount(), 0);
    XR_ASSERT_ALWAYS_EQ(test.UnsafeGetFreeCount() , kStackSize);
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
