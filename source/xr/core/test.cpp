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
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_THREADING_THREAD_H
#include "xr/core/threading/thread.h"
#endif
#ifndef XR_CORE_CONSOLE_H
#include "xr/core/console.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_THREADING_ATOMIC_H
#include "xr/core/threading/atomic.h"
#endif
#ifndef XR_CORE_DATE_H
#include "xr/core/date.h"
#endif
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif

#include <stdio.h> // fflush
#include <stdlib.h> // exit();

#if defined(XR_COMPILER_MICROSOFT)
#include "xr/platform/windows_lite.h"
#endif


// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE

xr::Core::Test::TestGroup XR_UNITTEST_GROUP_NAME(Global)(nullptr,"Global");
// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

#if !defined(XR_COMPILER_MICROSOFT)
jmp_buf Test::sExceptionReturnPoint;
#endif
static xr::Core::Thread::ThreadID sTestThreadID = 0;
static bool sTestFailedButCountinued = false;

// ***************************************************************************************** - TYPE
// Internal type for counting allocation and ensuring tests do not leak
// memory
// ***************************************************************************************** - TYPE
class AllocationCounter : public xr::Core::IUnknownHelper1<AllocationCounter, Core::IAllocator, Core::NotRefCounted>
{
public:
    XR_COM_CLASS_ID(0x149e2e5);

    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    AllocationCounter() :
        xr::Core::IUnknownHelper1<AllocationCounter, Core::IAllocator, Core::NotRefCounted>(),
        mParent(nullptr),
        mNumAllocs(0),
        mNumFrees(0)
        {}
    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    ~AllocationCounter() {}
    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    XR_RETURN_BYTES(size)
    XR_RESTRICT_RETURN
    void * Alloc(
        size_t size,
        size_t alignment,
        MemFlags eFlags,
        const char * description,
        const char * filename,
        int32_t line_number) XR_OVERRIDE;
    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    void   Free(
        XR_OUT_INVALIDATE void * ptr,
        size_t size,
        const char * filename,
        int32_t line_number) XR_OVERRIDE;
    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    XR_NO_SIDE_EFFECTS_PREFIX
    const char * GetName() const XR_OVERRIDE XR_NO_SIDE_EFFECTS_POSTFIX;

    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    void         SetAllocator(IAllocator & a)
    {
        mParent = &a;
    }

    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    void         Reset();
public:
    IAllocator * mParent;
    volatile size_t mNumAllocs;
    volatile size_t mNumFrees;
};

/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
XR_RETURN_BYTES(size)
XR_RESTRICT_RETURN
void * AllocationCounter::Alloc(
    size_t size,
    size_t alignment,
    MemFlags eFlags,
    const char * description,
    const char * filename,
    int32_t line_number)
{
    xr::Core::AtomicIncrement(&mNumAllocs);
    return mParent->Alloc(size, alignment, eFlags, description, filename, line_number);
}
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
void   AllocationCounter::Free(XR_OUT_INVALIDATE void * ptr, size_t size, const char * filename, int32_t line_number)
{
    if(ptr != nullptr)
    {
        mParent->Free(ptr, size, filename, line_number);
        xr::Core::AtomicIncrement(&mNumFrees);
    }
}
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
XR_NO_SIDE_EFFECTS_PREFIX const char * AllocationCounter::GetName() const
{
    return "AllocationCounter";
}
void         AllocationCounter::Reset()
{
    mNumAllocs = (0);
    mNumFrees = (0);
}

static AllocationCounter sAllocationCounter;

// ***************************************************************************************** - TYPE
// Internal type used as assert handler during tests
// Note that some tests place their own assert handler (notably the
// assert system test suite. In this case it is their own responsibility to
// explicitly call FailTest if something fails.
// ***************************************************************************************** - TYPE
class TestAssertHandler: public xr::Core::IUnknownHelper1<TestAssertHandler, ::xr::Core::IAssertFailedHandler, Core::NotRefCounted>
{
public:
    XR_COM_CLASS_ID(0x358a08a);
    /*-------------------------------------------------------------------*/
    /*!  */
    /*-------------------------------------------------------------------*/
    ::xr::Core::AssertFailAction AssertFailed(
                                            ::xr::Core::AssertCallType type,
                                            const char * file,
                                            const int32_t line,
                                            const char * message) XR_OVERRIDE;
    /*-------------------------------------------------------------------*/
    /*!  */
    /*-------------------------------------------------------------------*/
    ::xr::Core::AssertFailAction AssertFailedFormatted(
                                            ::xr::Core::AssertCallType type,
                                            const char * file,
                                            const int32_t line,
#if defined(XR_COMPILER_MICROSOFT)
                                            _Printf_format_string_
#endif
                                            const char * format,
                                            ...) XR_OVERRIDE;

private:
};


/*-------------------------------------------------------------------*/
/*!  */
/*-------------------------------------------------------------------*/
::xr::Core::AssertFailAction TestAssertHandler::AssertFailed(
                                        ::xr::Core::AssertCallType type,
                                        const char * file,
                                        const int32_t line,
                                        const char * message)
{
    XR_UNUSED(type);

    // In the case a debugger is attached, return (so that
    // the user is presented with the line that failed, not way up a
    // call stack.
    if(::xr::Core::IsDebuggerConnected())
    {
        if(message == nullptr)
        {
            message = "[NO_MESSAGE]";
        }
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            XR_EOL "%s(%" XR_INT32_PRINT "): %s" XR_EOL, file, line, message);
        sTestFailedButCountinued = true;
        return ::xr::Core::kActionBreak;
    }

    // this function does not actually return, it either throws an exception or longjmp's.
    ::xr::Core::Test::FailCurrentTest(file, line, message);

    //FailCurrentTest does not return.
    //return ::xr::Core::kActionBreak;
}
/*-------------------------------------------------------------------*/
/*!  */
/*-------------------------------------------------------------------*/
::xr::Core::AssertFailAction TestAssertHandler::AssertFailedFormatted(
                                        ::xr::Core::AssertCallType type,
                                        const char * file,
                                        const int32_t line,
#if defined(XR_COMPILER_MICROSOFT)
                                        _Printf_format_string_
#endif
                                        const char * format,
                                        ...)
{
    // In the case a debugger is attached, return (so that
    // the user is presented with the line that failed, not way up a
    // call stack.
    if(::xr::Core::IsDebuggerConnected())
    {
        va_list args;
        va_start(args,format);
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            XR_EOL "%s(%" XR_INT32_PRINT "): %s - " , file, line, type == ::xr::Core::kCallAssert ? "Assertion failed" : "Expectation failed");
        xr::Core::vConsolePrintf(xr::Core::kConsoleStdErr, format, args);
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr, XR_EOL);
        va_end(args);

        sTestFailedButCountinued = true;
        return ::xr::Core::kActionBreak;
    }


    XR_UNUSED(type);
    va_list vl;
    va_start(vl,format);

    // this function does not actually return, it either throws an exception or longjmp's.
    ::xr::Core::Test::FailCurrentTestVFormatted(file, line, format, vl);

    //FailCurrentTest does not return.
    //va_end(vl);
    //return ::xr::Core::kActionBreak;
}

static TestAssertHandler sTestAssertHandler;
// ######################################################################################### - FILE
// Test members follow:
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
volatile bool   Test::sTestsAreRunning = false;
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Test::TestToken *     Test::sExpectTestFail = nullptr;
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t                Test::sExpectTestLeaks = 0;
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult  Test::RunAll()
{
    StandardResult result = RunGroup("Global", nullptr);
    return result;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool            Test::IsTestRunning()
{
    return sTestsAreRunning;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult  Test::RunGroup(const char * groupName, const char * testNameOrNull)
{
    TestGroup * item = &XR_UNITTEST_GROUP_NAME(Global);
    int32_t failureCount = 0;
    while(item != nullptr)
    {
        if(xr::Core::StringCompare(groupName,item->GetName()) == 0 )
        {
            if(RunInternal(item, testNameOrNull) != xr::Core::kSuccess)
            {
                xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
                    "Test Group %s Failed!" XR_EOL, item->GetName());
                failureCount++;
            }
        }
        item = item->GetNextGroup();
    }
    return failureCount==0 ? xr::Core::kSuccess : xr::Core::kFailure;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Test::FailCurrentTest( const char * filename, int32_t lineNumber, const char * description )
{
    fflush(stdout);
    fflush(stderr);
    if(sTestsAreRunning && sExpectTestFail == nullptr)
    {
        // Log the failure info while we have file / line
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            XR_EOL "Test Failed: %s(%" XR_INT32_PRINT "): %s" XR_EOL, filename, lineNumber, description);

        if(::xr::Core::IsDebuggerConnected())
        {
            XR_HALT();
        }
    }

    if(sTestThreadID == xr::Core::Thread::GetCurrentThreadID())
    {
#if !defined(XR_COMPILER_MICROSOFT)
        longjmp(sExceptionReturnPoint, 1);
#else
        RaiseException(0,0,0,0 );
#endif
    }
    else
    {
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            XR_EOL "Failure occurred from non owning thread. Tests cannot safely continue." XR_EOL);
        exit(-1);
    }

    for(;;)
        ;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Test::FailCurrentTestFormatted( const char * filename, int32_t lineNumber, const char * format, ... )
{
    // Log the failure while we have file / line
    va_list args;
    va_start(args,format);
    FailCurrentTestVFormatted(filename, lineNumber, format, args);
    //Function does not return.
    //va_end(args);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Test::FailCurrentTestVFormatted( const char * filename, int32_t lineNumber, const char * format, va_list args )
{
    if(sTestsAreRunning && sExpectTestFail == nullptr)
    {
        // Log the failure while we have file / line
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            XR_EOL "Test Failed: %s(%" XR_INT32_PRINT "): ", filename, lineNumber);
        xr::Core::vConsolePrintf(xr::Core::kConsoleStdErr, format, args);
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr, XR_EOL);

        if(::xr::Core::IsDebuggerConnected())
        {
            XR_HALT();
        }
    }

#if !defined(XR_COMPILER_MICROSOFT)
    longjmp(sExceptionReturnPoint, 1);
#else
    RaiseException(0,0,0,0 );
#endif
    for(;;)
        ;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Test::ExpectCurrentTestFail( ::xr::Core::Test::TestFunction * testToFail )
{
    sExpectTestFail = testToFail;
    return;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Test::ExpectCurrentTestLeak(size_t numAllocations)
{
    sExpectTestLeaks = numAllocations;
    return;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Test::TestGroup * Test::GetRootTestGroup()
{
    return & XR_UNITTEST_GROUP_NAME(Global);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void  Test::TestGroup::RegisterGroup(TestGroup * parentGroup, TestGroup * childGroup)
{
    if(parentGroup == nullptr)
    {
        XR_ASSERT_DEBUG_EQ(childGroup, &XR_UNITTEST_GROUP_NAME(Global));
        return;
    }

    if(parentGroup->mFirstChild == nullptr)
    {
        // Start the Chain
        parentGroup->mFirstChild = parentGroup->mLastChild = childGroup;
    }
    else
    {
        // Add a link to the end
        parentGroup->mLastChild->SetNextGroup(childGroup);
        parentGroup->mLastChild = childGroup;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Test::TestGroup::RegisterToken(TestGroup * testGroup, TestToken * testItem)
{
    if(testGroup->mFirstToken == nullptr)
    {
        // Start the Chain
        testGroup->mFirstToken = testGroup->mLastToken = testItem;
    }
    else
    {
        // Add a link to the end
        testGroup->mLastToken->SetNextToken(testItem);
        testGroup->mLastToken = testItem;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
StandardResult Test::RunInternal(TestGroup * pGroup, const char * testNameOrNull)
{
    TestToken * item = pGroup->GetTokenChainHead();
    int32_t failureCount = 0;

    ::xr::Core::IAssertFailedHandler & originalHandler = ::xr::Core::Assert::GetFailHandler();

    IAllocator & originalAllocator = GetGeneralAllocator();
    sAllocationCounter.SetAllocator(originalAllocator);
    /*`````````````````````````````````````````````````````````````````````*/
    // Look for matching tests. (linear search)
    /*`````````````````````````````````````````````````````````````````````*/
    while(item != nullptr)
    {
        if(testNameOrNull == nullptr || (xr::Core::StringCompare(testNameOrNull,item->GetTokenName()) == 0) )
        {
            if(item->GetTokenType() != kTokenFunction)
            {
                // Can only run test functions. Other types are unsupported at this time.
                return xr::Core::kFailure;
            }

            /*```````````````````````````````````````````````````````````*/
            // Found a test function to call. Setup the ability to exit the
            // test, either by exceptions or by longjmp.
            /*```````````````````````````````````````````````````````````*/
#if !defined(XR_COMPILER_MICROSOFT)
            if(setjmp(sExceptionReturnPoint))
            {
                // goto is bad, but prevents copy / pasted code.
                // besides, it is not much worse than using setjmp, longjump...
                // It's use is purely to enable users who do not want to use
                // exceptions
                goto TestFailed;
            }
            else
#else
            __try
#endif
            {
                char dateStr[16];
                xr::Core::ToString(kDateLocal, dateStr, 16, "%X", xr::Core::Now());
                /*```````````````````````````````````````````````````````*/
                // Run the Test
                /*```````````````````````````````````````````````````````*/
                xr::Core::ConsolePrintf(
                    xr::Core::kConsoleStdOut,
                    "%s Test %s::%s ...", dateStr, pGroup->GetName(), item->GetTokenName());

                sExpectTestFail = nullptr;
                sExpectTestLeaks = 0;
                sTestsAreRunning = true;

                sTestThreadID = xr::Core::Thread::GetCurrentThreadID();

                // Some tests replace the assert handler for test purposes. Be sure to restore the proper handler
                ::xr::Core::Assert::SetFailHandler(sTestAssertHandler);

                TestFunction * tf = static_cast<TestFunction*>(item);

                sAllocationCounter.Reset();
                SetGeneralAllocator(sAllocationCounter);
                tf->Run();

                sTestThreadID = 0;
                sTestsAreRunning = false;

                ::xr::Core::Assert::SetFailHandler(originalHandler);

                /*```````````````````````````````````````````````````````*/
                // Check for Leaks first
                /*```````````````````````````````````````````````````````*/
                if( (sAllocationCounter.mNumAllocs - sAllocationCounter.mNumFrees) != sExpectTestLeaks)
                {
                    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr, XR_EOL);
                    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr, "Test %s::%s Leaked %" XR_SIZE_PRINT " Allocs. (Expected %" XR_SIZE_PRINT " Leaks)" XR_EOL,
                        pGroup->GetName(),
                        item->GetTokenName(),
                        sAllocationCounter.mNumAllocs - sAllocationCounter.mNumFrees,
                        sExpectTestLeaks
                        );

                    if(::xr::Core::IsDebuggerConnected())
                    {
                        // Trigger an exception, feel free to walk debugger back up to "Run" call above.
                        XR_HALT();
                    }
                }
                /*```````````````````````````````````````````````````````*/
                // If the test stated it intended to fail, we shouldn't be here.
                /*```````````````````````````````````````````````````````*/
                else if(item == sExpectTestFail)
                {
                    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr, XR_EOL);
                    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
                        "Test %s::%s Was Expected to Fail, but passed!!!" XR_EOL , pGroup->GetName(), item->GetTokenName());
                    failureCount++;

                    if(::xr::Core::IsDebuggerConnected())
                    {
                        // Trigger an exception, feel free to walk debugger back up to "Run" call above.
                        XR_HALT();
                    }
                }
                else if(sTestFailedButCountinued == false)
                {
                    // Note that if you are debugging and hit "F5" or continue passed a failed test,
                    // this "passed" will still print (after the failed prints). Since this only occurs
                    // when debugging, it seems unlikely to cause confusion. If this is deemed
                    // confusing at a later date, it can be fixed.
                    xr::Core::ConsolePrintf(
                        xr::Core::kConsoleStdOut,
                        "Passed" XR_EOL);
                }
                else
                {
                    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
                        "Test %s::%s Failed!" XR_EOL, pGroup->GetName(), item->GetTokenName());
                    failureCount++;
                    sTestFailedButCountinued = false;
                }

                item = item->GetNextToken();
                continue;
            }
            /*```````````````````````````````````````````````````````````*/
            // Test failed logic.
            /*```````````````````````````````````````````````````````````*/
#if defined(XR_COMPILER_MICROSOFT)
            // If a debugger is present, let the exception / failure go to the debugger. That's
            // why you are debugging, right?
            __except(xr::Core::IsDebuggerConnected() ? EXCEPTION_CONTINUE_SEARCH : EXCEPTION_EXECUTE_HANDLER)

#else
            // Goto target for setjump/longjmp implementation.
            TestFailed:
#endif
            {
                if(item != sExpectTestFail)
                {

                    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
                        "Test %s::%s Failed!" XR_EOL, pGroup->GetName(), item->GetTokenName());
                    failureCount++;
                }
                else
                {
                    // Test failed, but was supposed to fail (probably an assert or test related test.
                    xr::Core::ConsolePrintf(xr::Core::kConsoleStdOut,
                        "Passed" XR_EOL);
                }
            }

        }
        item = item->GetNextToken();
    }

    /*```````````````````````````````````````````````````````````*/
    // This group's tests are done, run all of our children's tests
    /*```````````````````````````````````````````````````````````*/
    TestGroup * pChild = pGroup->GetChildGroupChainHead();
    while(pChild != nullptr)
    {
        if(RunInternal(pChild, testNameOrNull) != xr::Core::kSuccess)
        {
            failureCount++;
        }
        pChild = pChild->GetNextGroup();
    }

    /*```````````````````````````````````````````````````````````*/
    // Restore any modified variables before returning.
    /*```````````````````````````````````````````````````````````*/
    sTestThreadID = 0;
    sTestsAreRunning = false;
    ::xr::Core::Assert::SetFailHandler(originalHandler);

    SetGeneralAllocator(originalAllocator);
    return failureCount==0 ? xr::Core::kSuccess : xr::Core::kFailure;
}
}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
