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
#ifndef XR_SERVICES_SCHEDULING_H
#include "xr/services/scheduling.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_THREADING_ATOMIC_H
#include "xr/core/threading/atomic.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_THREADING_THREAD_H
#include "xr/core/threading/thread.h"
#endif
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Scheduling )

void TestRunnable(const xr::Core::Arguments * a)
{
    bool * b = (bool*)(a->a0);
    *b = true;
}

class TestRunnableFunctor{
public:
    TestRunnableFunctor(): didRun(nullptr) {}

    void operator() () const
    {
        *didRun = true;
    }

    bool *didRun;
};

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Basic )
{
    xr::Scheduling::IManager::InitializeOptions options;
    options.mNumThreads = 1;
    options.mReadyListSize = 1;
    options.mFreeListSize = 1;

    xr::Scheduling::IManager * p = xr::Scheduling::IManager::Initialize(&options);
    xr::Scheduling::JobHandle h;

    // Try with static function.
    {
        bool didRun = false;
        xr::Core::Arguments a;
        a.a0 = (uintptr_t)&didRun;

        XR_ASSERT_ALWAYS_EQ(didRun, false);
        h = p->InsertReady(&TestRunnable, &a);
        h.WaitOn();
        XR_ASSERT_ALWAYS_EQ(didRun, true);
    }

    // Try with lambda
    {
        bool didRun = false;

        XR_ASSERT_ALWAYS_EQ(didRun, false);
        h = p->InsertReady([&didRun] () { 
            didRun = true; 
        });
        h.WaitOn();
        XR_ASSERT_ALWAYS_EQ(didRun, true);
    }

    // Try with functor
    {
        TestRunnableFunctor trf;
        bool didRun = false;
        trf.didRun = &didRun;

        XR_ASSERT_ALWAYS_EQ(didRun, false);
        h = p->InsertReady(trf);
        h.WaitOn();
        XR_ASSERT_ALWAYS_EQ(didRun, true);
    }

    xr::Scheduling::IManager::Shutdown(p);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Blocked )
{
    xr::Scheduling::IManager::InitializeOptions options;
    options.mNumThreads = 1;
    options.mReadyListSize = 2;
    options.mFreeListSize = 2;

    xr::Scheduling::IManager * p = xr::Scheduling::IManager::Initialize(&options);


    // Try with lambda
    bool didBlockedRun = false;

    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);
    xr::Scheduling::JobHandleBlocked h = p->InsertBlocked([&didBlockedRun] () { didBlockedRun = true; });
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);

    // Pump through other jobs.
    for(size_t i = 0; i < 10; ++i)
    {
        bool didOtherRun = false;
        p->InsertReady([&didOtherRun] () { didOtherRun = true; }).WaitOn();
        XR_ASSERT_ALWAYS_EQ(didOtherRun, true);
    }

    // Now make sure the blocked job has not run.
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);
    XR_ASSERT_ALWAYS_EQ(h.IsDone(), false);

    h.ReleaseBarrier();
    h.WaitOn();
    XR_ASSERT_ALWAYS_EQ(h.IsDone(), true);
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, true);

    xr::Scheduling::IManager::Shutdown(p);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( BlockedBarrier )
{
    xr::Scheduling::IManager::InitializeOptions options;
    options.mNumThreads = 1;
    options.mReadyListSize = 2;
    options.mFreeListSize = 2;

    xr::Scheduling::IManager * p = xr::Scheduling::IManager::Initialize(&options);


    // Try with lambda
    bool didBlockedRun = false;

    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);
    xr::Scheduling::JobHandleBlocked h = p->InsertBlocked([&didBlockedRun] () { didBlockedRun = true; });
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);

    // Pump through other jobs.
    for(size_t i = 0; i < 10; ++i)
    {
        bool didOtherRun = false;
        p->InsertReady([&didOtherRun] () { didOtherRun = true; }).WaitOn();
        XR_ASSERT_ALWAYS_EQ(didOtherRun, true);
    }

    // Now make sure the blocked job has not run.
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);
    XR_ASSERT_ALWAYS_EQ(h.IsDone(), false);

    ::xr::Core::Arguments barrierArgs;
    ::xr::Core::Runnable barrier = h.AddBarrier(barrierArgs, 5);
    h.ReleaseBarrier();

    // Now make sure the blocked job has not run.
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);

    for(size_t i = 0; i < 4; ++i)
    {
        barrier(&barrierArgs);
        XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);

    }

    barrier(&barrierArgs);
    h.WaitOn();
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, true);


    xr::Scheduling::IManager::Shutdown(p);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( BlockedBarrierJobs )
{
    xr::Scheduling::IManager::InitializeOptions options;
    options.mNumThreads = 1;
    options.mReadyListSize = 2;
    options.mFreeListSize = 2;

    xr::Scheduling::IManager * p = xr::Scheduling::IManager::Initialize(&options);


    // Try with lambda
    bool didBlockedRun = false;

    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);
    xr::Scheduling::JobHandleBlocked h = p->InsertBlocked([&didBlockedRun] () { didBlockedRun = true; });
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);

    // Pump through other jobs.
    for(size_t i = 0; i < 10; ++i)
    {
        bool didOtherRun = false;
        p->InsertReady([&didOtherRun] () { didOtherRun = true; }).WaitOn();
        XR_ASSERT_ALWAYS_EQ(didOtherRun, true);
    }

    // Now make sure the blocked job has not run.
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);
    XR_ASSERT_ALWAYS_EQ(h.IsDone(), false);

    ::xr::Core::Arguments barrierArgs;
    ::xr::Core::Runnable barrier = h.AddBarrier(barrierArgs, 5);
    h.ReleaseBarrier();

    // Now make sure the blocked job has not run.
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);

    for(size_t i = 0; i < 4; ++i)
    {
        p->InsertReady(barrier, &barrierArgs);
        XR_ASSERT_ALWAYS_EQ(didBlockedRun, false);
    }

    p->InsertReady(barrier, &barrierArgs);
    h.WaitOn();
    XR_ASSERT_ALWAYS_EQ(didBlockedRun, true);


    xr::Scheduling::IManager::Shutdown(p);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
void RunTestWithParams(size_t numThreads, size_t numReady, size_t numFree, size_t numJobs)
{
    xr::Scheduling::IManager::InitializeOptions options;
    options.mNumThreads = numThreads;
    options.mReadyListSize = numReady;
    options.mFreeListSize = numFree;

    xr::Scheduling::IManager * p = xr::Scheduling::IManager::Initialize(&options);

    xr::Scheduling::JobHandle * h = XR_NEW( "TestJobs") xr::Scheduling::JobHandle[numJobs];

    volatile size_t counter = 0;
    for(size_t i = 0; i < numJobs; i++)
    {
        h[i] = p->InsertReady([&counter] () { xr::Core::AtomicIncrement(&counter); });
    }

    for(size_t i = 0; i < numJobs; i++)
    {
        h[i].WaitOn();
        XR_ASSERT_ALWAYS_GE(counter, i);
    }

    XR_ASSERT_ALWAYS_EQ(counter, numJobs);

    XR_DELETE_ARRAY(h);
    xr::Scheduling::IManager::Shutdown(p);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Counter )
{
	//               numThreads, numReady, numFree, numJobs
    RunTestWithParams( 1, 1, 1, 100);
    RunTestWithParams(10, 1, 1, 100);
    RunTestWithParams(30, 1, 1, 100);
    RunTestWithParams(10, 10, 10, 1000);
    RunTestWithParams(10, 100, 100, 5000);
    RunTestWithParams(30, 1000, 1000, 10000);
}


void RunOneToManyTestWithParams(size_t numThreads, size_t numReady, size_t numFree, size_t numJobs)
{
    xr::Scheduling::IManager::InitializeOptions options;
    options.mNumThreads = numThreads;
    options.mReadyListSize = numReady;
    options.mFreeListSize = numFree;

    xr::Scheduling::IManager * p = xr::Scheduling::IManager::Initialize(&options);

    volatile bool   waitFor    = false;
    volatile bool   waiterDone = false;
    volatile size_t counter    = 0;

    xr::Scheduling::JobHandle * h = XR_NEW( "TestJobs") xr::Scheduling::JobHandle[ numJobs ];

    for(int xxx = 0; xxx < 10; xxx++)
    {
        waitFor = false;
        waiterDone = false;
        counter = 0;

        xr::Scheduling::JobHandle coreHandle = p->InsertReady([&waitFor, &waiterDone] () {
            while(!waitFor)
                xr::Core::Thread::YieldCurrentThread();
            waiterDone = true;
        });

        for(size_t i = 0; i < numJobs; i++)
        {
            h[i] = p->InsertAfter([&counter] () {
                xr::Core::AtomicIncrement(&counter);
            }, &coreHandle, 1);
        }

        for(size_t i = 0; i < numJobs; i++)
        {
            XR_ASSERT_ALWAYS_EQ(h[i].IsDone(), false);
            XR_ASSERT_ALWAYS_EQ(counter, 0);
        }

        XR_ASSERT_ALWAYS_EQ(waiterDone, false);


        // Unlock the Thread.
        waitFor = true;
        // Wait for it.
        coreHandle.WaitOn();
        XR_ASSERT_ALWAYS_EQ(waiterDone, true);

        for(size_t i = 0; i < numJobs; i++)
        {
            h[i].WaitOn();
            XR_ASSERT_ALWAYS_GE(counter, i);
        }

        XR_ASSERT_ALWAYS_EQ(counter, numJobs);
    }

    XR_DELETE_ARRAY(h);
    xr::Scheduling::IManager::Shutdown(p);
}


// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( DependencyOneToMany )
{
	//               numThreads, numReady, numFree, numJobs
    RunOneToManyTestWithParams(1, 1, 2, 1);
    RunOneToManyTestWithParams(1, 2, 3, 2);
    // Note that some free entries will be used as dependency objects.
    RunOneToManyTestWithParams( 1,100, 100, 80);
    RunOneToManyTestWithParams( 8, 99, 100, 80);
    RunOneToManyTestWithParams(10, 99, 100, 80);
}


void RunManyToOneTestWithParams(size_t numThreads, size_t numReady, size_t numFree, size_t numJobs)
{
    xr::Scheduling::IManager::InitializeOptions options;
    options.mNumThreads = numThreads;
    options.mReadyListSize = numReady;
    options.mFreeListSize = numFree;

    xr::Scheduling::IManager * p = xr::Scheduling::IManager::Initialize(&options);

    volatile bool   waitFor    = false;
    volatile bool   waiterDone = false;

    volatile size_t result     = 0;
    volatile size_t counter    = 0;

    xr::Scheduling::JobHandle * h = XR_NEW( "TestJobs") xr::Scheduling::JobHandle[numJobs];

    for(int xxx = 0; xxx < 10; xxx++)
    {
        waitFor    = false;
        waiterDone = false;
        counter = 0;
        result = 0;

        for(size_t i = 0; i < numJobs-1; i++)
        {
            h[i] = p->InsertReady([&counter] () {
                xr::Core::AtomicIncrement(&counter);
            });
        }
        h[numJobs-1] = p->InsertReady([&waitFor, &waiterDone] () {
            while(!waitFor)
                xr::Core::Thread::YieldCurrentThread();
            waiterDone = true;
        });

        xr::Scheduling::JobHandle resultHandle = p->InsertAfter( [&result] () {
            xr::Core::AtomicIncrement(&result);
            }, h, numJobs);


        XR_ASSERT_ALWAYS_EQ(waiterDone, false);
        XR_ASSERT_ALWAYS_EQ(result, 0);

        // Wait for the non-gated jobs.
        for(size_t i = 0; i < numJobs-1; i++)
        {
            h[i].WaitOn();
            XR_ASSERT_ALWAYS_GE(counter, i);
        }

        XR_ASSERT_ALWAYS_EQ(counter, numJobs-1);
        XR_ASSERT_ALWAYS_EQ(result, 0);

        // Unlock the last Job.
        waitFor = true;

        h[numJobs-1].WaitOn();

        XR_ASSERT_ALWAYS_EQ(waiterDone, true);

        // Now the result should be running.
        resultHandle.WaitOn();

        XR_ASSERT_ALWAYS_EQ(result, 1);
    }

    XR_DELETE_ARRAY(h);
    xr::Scheduling::IManager::Shutdown(p);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( DependencyManyToOne )
{
	//               numThreads, numReady, numFree, numJobs
    RunManyToOneTestWithParams(1, 1, 2, 1);
    RunManyToOneTestWithParams(1, 2, 3, 2);
    // Note that some free entries will be used as dependency objects.
    RunManyToOneTestWithParams( 1,100, 100, 80);
    RunManyToOneTestWithParams( 8, 99, 100, 80);
    RunManyToOneTestWithParams(10, 99, 100, 80);
}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
