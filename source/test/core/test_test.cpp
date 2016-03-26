// ######################################################################################### - FILE
/*!

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

#if defined(XR_CODE_COVERAGE_BUILD)
// coverage build disables asserts, but test system tests needs them.
// This define lets the header know that this is the assert test.
#define XR_ASSERT_INTERNAL_TESTS 1
#endif

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
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_CONSOLE_H
#include "xr/core/console.h"
#endif
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif

// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Test )

// --------------------------------------------------------------------------------------  FUNCTION
//
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Simple )
{
    // This should pass!
}
// --------------------------------------------------------------------------------------  FUNCTION
//
// --------------------------------------------------------------------------------------  FUNCTION
static bool sRanFirst = false;
XR_UNITTEST_TEST_FUNC( Order1 )
{
    sRanFirst = true;
}
// --------------------------------------------------------------------------------------  FUNCTION
//
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Order2 )
{
    XR_ASSERT_ALWAYS_TRUE_M(sRanFirst, "ERROR: Test runner is Running tests out of order! This may cause test failures.");
}
// --------------------------------------------------------------------------------------  FUNCTION
//
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( FailTestPath )
{
    // This should fail!
    if(!::xr::Core::IsDebuggerConnected())
    {
        // Compiler will warn of unreachable code, but the whole point
        // of this test is to make sure the function does not return.
        // This will run the code, but create a situation where if it some
        // how did in fact return, it would halt.
        volatile bool warningFixer = true;
        if(warningFixer)
        {
            ::xr::Core::Test::ExpectCurrentTestFail(this);
            ::xr::Core::Test::FailCurrentTest(XR_FILE_LINE, "Should Fail");
        }
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            "ERROR: Test suite is not failing when a test fails. Results are suspect!");
        XR_HALT();
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
//
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( FailTestPathFormatted )
{
    // This should fail!
    if(!::xr::Core::IsDebuggerConnected())
    {
        volatile bool warningFixer = true;
        if(warningFixer)
        {
            ::xr::Core::Test::ExpectCurrentTestFail(this);
            ::xr::Core::Test::FailCurrentTestFormatted(XR_FILE_LINE, "Should Fail %d", 1);
        }
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            "ERROR: Test suite is not failing when a test fails. Results are suspect!");
        XR_HALT();
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// Make sure that asserts fail a test during a test.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( AssertTestFail )
{
    // This will stop the debugger, so skip it if you're running from debugger.
    if(!::xr::Core::IsDebuggerConnected())
    {
        // This should fail!
       ::xr::Core::Test::ExpectCurrentTestFail(this);
        XR_ASSERT_ALWAYS_EQ(1,2); // SHOULD FAIL RIGHT HERE!
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            "ERROR: Assert does not work! Other test results may not be trustable." XR_EOL);
        XR_HALT();
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// Make sure that asserts fail a test during a test.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( AssertFormattedTestFail )
{
    // This will stop the debugger, so skip it if you're running from debugger.
    if(!::xr::Core::IsDebuggerConnected())
    {
        // This should fail!
       ::xr::Core::Test::ExpectCurrentTestFail(this);
        XR_ASSERT_ALWAYS_EQ_FM(1,2, "Test %d", 2); // SHOULD FAIL RIGHT HERE!
        xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
            "ERROR: Assert does not work! Other test results may not be trustable." XR_EOL);
        XR_HALT();
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
//
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( CoverageCompletion )
{
    // This should fail!
    ::xr::Core::Test::TestGroup * root = ::xr::Core::Test::GetRootTestGroup();

    if(::xr::Core::StringCompare(root->GetName(), "Global") != 0)
    {
        // Note that this makes some assumptions about internal details.
        // It may need to be updated if the name changes.
        ::xr::Core::Test::FailCurrentTest(XR_FILE_LINE, "root test node was not what we expected.");
    }

    XR_ASSERT_ALWAYS_EQ( ::xr::Core::Test::IsTestRunning(), true);

}
// --------------------------------------------------------------------------------------  FUNCTION
// uncomment this to test tests REALLY failing
// --------------------------------------------------------------------------------------  FUNCTION
//XR_UNITTEST_TEST_FUNC( DebugTestFailPath )
//{
//    ::xr::Core::Test::FailCurrentTest(XR_FILE_LINE, "Test Fail Path (for testing purposes)");
//}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
