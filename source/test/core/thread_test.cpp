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
#ifndef XR_CORE_THREADING_THREAD_H
#include "xr/core/threading/thread.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_THREADING_MUTEX_H
#include "xr/core/threading/mutex.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_CONSOLE_H
#include "xr/core/console.h"
#endif
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif

#include <stdlib.h>
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Thread )

    class ThreadTestSimple: public xr::Core::Thread
    {
    public:
        ThreadTestSimple() : ::xr::Core::Thread("testThread")
        {

        }

        class AssertFiredTempReplacement: public xr::Core::IUnknownHelper1<AssertFiredTempReplacement, ::xr::Core::IAssertFailedHandler, xr::Core::NotRefCounted>
        {
        public:
            XR_COM_CLASS_ID(0x317e450);

            AssertFiredTempReplacement(): mCount(0){};
            xr::Core::AssertFailAction AssertFailed(xr::Core::AssertCallType , const char * filename, const int32_t lineNumber, const char * message)
            {
                xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
                    XR_EOL "%s(%" XR_INT32_PRINT "): %s" XR_EOL, filename, lineNumber, message);
                mCount++;
                return ::xr::Core::IsDebuggerConnected() ? xr::Core::kActionBreak : xr::Core::kActionContinue;
            }

            xr::Core::AssertFailAction AssertFailedFormatted(xr::Core::AssertCallType , const char * filename, const int32_t lineNumber,
#if defined(XR_COMPILER_MICROSOFT)
                _Printf_format_string_
#endif
                const char * format, ... )
            {
                va_list args;
                va_start (args, format);

                xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
                    XR_EOL "%s(%" XR_INT32_PRINT "): ", filename, lineNumber);
                xr::Core::vConsolePrintf(xr::Core::kConsoleStdErr,
                    format, args);
                xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr, XR_EOL);


                va_end (args);
                mCount++;
                return ::xr::Core::IsDebuggerConnected() ? xr::Core::kActionBreak : xr::Core::kActionContinue;
            }

            inline int32_t GetCount() { return mCount;}
            inline void ResetCount() { mCount = 0;}
        private:
            int32_t					 mCount;
        };

        //---------------------------------------------------------------------
        // WARNING!!!! Asserts that fail in this function will crash.
        // Need to supplant the assert handler temporarily in order to
        // catch any failures.
        // The test system does not allow failures from another thread (or
        // more specifically it handles them, but exit's the process in doing so
        // for lack of any better course of action. This allows for more
        // graceful output in that case.
        //---------------------------------------------------------------------
        uintptr_t Run()
        {
            AssertFiredTempReplacement newHandler;
            xr::Core::IAssertFailedHandler & oldHandler = xr::Core::Assert::SetFailHandler(newHandler);

            xr::Core::Thread::ThreadID id = GetID();
            XR_ASSERT_ALWAYS_NE(id, 0);
            GetPriority();
            bool started = HasStarted();
            XR_ASSERT_ALWAYS_TRUE(started);
            const char * name = GetName();
            XR_ASSERT_ALWAYS_EQ(xr::Core::StringCompare(name, "testThread"), 0);
            bool quitRequested = IsQuitRequested();
            XR_ASSERT_ALWAYS_FALSE(quitRequested);
            bool completed = HasCompleted();
            XR_ASSERT_ALWAYS_FALSE(completed);
            RequestQuit();
            quitRequested = IsQuitRequested();
            XR_ASSERT_ALWAYS_TRUE(quitRequested);

            Thread & t = xr::Core::Thread::GetCurrentThread();

            XR_ASSERT_ALWAYS_EQ(&t, this);

            xr::Core::Thread::ThreadID id2 = xr::Core::Thread::GetCurrentThreadID();

            XR_ASSERT_ALWAYS_EQ(id, id2);

            const char * name2 = xr::Core::Thread::GetCurrentThreadName();
            XR_ASSERT_ALWAYS_EQ(xr::Core::StringCompare(name2, name), 0);

            xr::Core::Assert::SetFailHandler(oldHandler);
            return newHandler.GetCount() == 0 ? 42 : -1;
        }
    };

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Simple )
{
    ThreadTestSimple tester;

    XR_ASSERT_ALWAYS_FALSE(tester.HasStarted());
    XR_ASSERT_ALWAYS_FALSE(tester.HasCompleted());
    tester.Start();
    tester.Join();
    XR_ASSERT_ALWAYS_TRUE(tester.HasStarted());
    XR_ASSERT_ALWAYS_TRUE(tester.HasCompleted());
    uintptr_t retValue = tester.GetReturnCode();
    XR_ASSERT_ALWAYS_EQ(retValue, 42);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Coverage )
{
    // Sadly the code to create a Thread object for system threads
    // leaks an allocation. It's more of a waste than a leak, but
    // still will be reported as a leak to the testing framework.
    static bool sRanOnce = false;
    ::xr::Core::Test::ExpectCurrentTestLeak(sRanOnce ? 0 : 1);
    sRanOnce = true;

    xr::Core::Thread &t = xr::Core::Thread::GetCurrentThread();
    bool completed  = t.HasCompleted();
    XR_ASSERT_ALWAYS_FALSE(completed);
    bool started = t.HasStarted();
    XR_ASSERT_ALWAYS_TRUE(started);

    xr::Core::Thread::YieldCurrentThread();
    xr::Core::Thread::YieldCurrentThread(10);


}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
