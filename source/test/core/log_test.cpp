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
#ifndef XR_CORE_LOG_H
#include "xr/core/log.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif
#ifndef XR_CORE_MEM_UTILS_H
#include "xr/core/mem_utils.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif


// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

static xr::Core::LogHandle * sStartingLogHandle = nullptr;

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Log )

    // --------------------------------------------------------------------------------------  FUNCTION
    // This verifies that all of the macros at least filter through to the proper
    // consumers.
    // --------------------------------------------------------------------------------------  FUNCTION
    XR_UNITTEST_TEST_FUNC( Startup )
    {
        using namespace xr::Core;

        sStartingLogHandle = LogHandle::GetFirstHandle();
    }


    class LogCounter: public ::xr::Core::ILogConsumer {
    public:

        LogCounter()
        {
            Reset();
        }

        void Log(::xr::Core::LogLevel level, const char * , int32_t , const char    * message) XR_OVERRIDE
        {
            mTotal++;
            mNarrow++;

#if defined(XR_COMPILER_MICROSOFT)
			// Supress bogus overflow.
#pragma warning(disable: 6385)
#endif
            XR_ASSERT_ALWAYS_TRUE(level >= ::xr::Core::kLogLevelMin);
            XR_ASSERT_ALWAYS_TRUE(level <= ::xr::Core::kLogLevelMax);
            mPerLevel[level]++;

            mLastMessageLengthIncludingNull = xr::Core::StringLengthWithNull(message);
        }
        void Log(::xr::Core::LogLevel  level, const char * , int32_t , const wchar_t * message) XR_OVERRIDE
        {
            mTotal++;
            mWide++;

            XR_ASSERT_ALWAYS_TRUE(level >= ::xr::Core::kLogLevelMin);
            XR_ASSERT_ALWAYS_TRUE(level <= ::xr::Core::kLogLevelMax);

            mPerLevel[level]++;

            mLastMessageLengthIncludingNull = xr::Core::StringLengthWithNull(message);
        }

        void Reset()
        {
            mTotal = 0;
            mWide = 0;
            mNarrow = 0;
            for(int i = 0; i < ::xr::Core::kLogLevelNumLevels; i++)
            {
                mPerLevel[i] = 0;
            }
        }

        int32_t mTotal;
        int32_t mWide;
        int32_t mNarrow;
        int32_t mPerLevel[::xr::Core::kLogLevelNumLevels];
        size_t  mLastMessageLengthIncludingNull;
    };
// --------------------------------------------------------------------------------------  FUNCTION
// This verifies that all of the macros at least filter through to the proper
// consumers.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( BasicTest )
{
    // Basic test.
    LogCounter lc;

    xr::Core::LogHandle h("LogTest.Basic");

    h.AddLogConsumer(&lc, ::xr::Core::kLogLevelTrace);

    XR_LOG_TRACE_MESSAGE(&h,      "TRACE Message Test");
    XR_LOG_TRACE_FORMATTED(&h,    "TRACE Message %x", 0xff);
    XR_LOG_DEBUG_MESSAGE(&h,      "DEBUG Message Test");
    XR_LOG_DEBUG_FORMATTED(&h,    "DEBUG Message %x", 0xff);
    XR_LOG_INFO_MESSAGE(&h,      "INFO Message Test");
    XR_LOG_INFO_FORMATTED(&h,    "INFO Message %x", 0xff);
    XR_LOG_WARNING_MESSAGE(&h,   "WARNING Message Test");
    XR_LOG_WARNING_FORMATTED(&h, "WARNING Message %x", 0xff);
    XR_LOG_ERROR_MESSAGE(&h,     "ERROR Message Test");
    XR_LOG_ERROR_FORMATTED(&h,   "ERROR Message %x", 0xff);
    XR_LOG_FATAL_MESSAGE(&h,     "FATAL Message Test");
    XR_LOG_FATAL_FORMATTED(&h,   "FATAL Message %x", 0xff);

    // If the number of levels changes (unlikely) this test should be updated to
    // include the new levels.
    XR_ASSERT_ALWAYS_EQ(lc.mTotal, 12);

    XR_ASSERT_ALWAYS_EQ(lc.mTotal, ::xr::Core::kLogLevelNumLevels * 2);
    XR_ASSERT_ALWAYS_EQ(lc.mNarrow, ::xr::Core::kLogLevelNumLevels * 2);
    XR_ASSERT_ALWAYS_EQ(lc.mWide,     0);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelTrace], 2);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelDebug], 2);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelInfo]     , 2);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelWarning], 2);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelError], 2);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelFatal], 2);

    // Wide char versions.
    XR_LOG_TRACE_MESSAGE(&h,     XR_STR_WCHAR_T("TRACE Message Test"));
    XR_LOG_TRACE_FORMATTED(&h,   XR_STR_WCHAR_T("TRACE Message %x"), 0xff);
    XR_LOG_DEBUG_MESSAGE(&h,     XR_STR_WCHAR_T("DEBUG Message Test"));
    XR_LOG_DEBUG_FORMATTED(&h,   XR_STR_WCHAR_T("DEBUG Message %x"), 0xff);
    XR_LOG_INFO_MESSAGE(&h,      XR_STR_WCHAR_T("INFO Message Test"));
    XR_LOG_INFO_FORMATTED(&h,    XR_STR_WCHAR_T("INFO Message %x"), 0xff);
    XR_LOG_WARNING_MESSAGE(&h,   XR_STR_WCHAR_T("WARNING Message Test"));
    XR_LOG_WARNING_FORMATTED(&h, XR_STR_WCHAR_T("WARNING Message %x"), 0xff);
    XR_LOG_ERROR_MESSAGE(&h,     XR_STR_WCHAR_T("ERROR Message Test"));
    XR_LOG_ERROR_FORMATTED(&h,   XR_STR_WCHAR_T("ERROR Message %x"), 0xff);
    XR_LOG_FATAL_MESSAGE(&h,     XR_STR_WCHAR_T("FATAL Message Test"));
    XR_LOG_FATAL_FORMATTED(&h,   XR_STR_WCHAR_T("FATAL Message %x"), 0xff);

    XR_ASSERT_ALWAYS_EQ(lc.mTotal,   (::xr::Core::kLogLevelNumLevels * 2) * 2 );
    XR_ASSERT_ALWAYS_EQ(lc.mNarrow,   ::xr::Core::kLogLevelNumLevels * 2);
    XR_ASSERT_ALWAYS_EQ(lc.mWide,     ::xr::Core::kLogLevelNumLevels * 2);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelTrace], 4);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelDebug], 4);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelInfo]     , 4);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelWarning], 4);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelError], 4);
    XR_ASSERT_ALWAYS_EQ(lc.mPerLevel[::xr::Core::kLogLevelFatal], 4);

    h.RemoveLogConsumer(&lc);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( EmptyTest )
{
    {
        // Basic test.
        LogCounter lc;
        xr::Core::LogHandle h1("");
        xr::Core::LogHandle h2("1");

        h1.AddLogConsumer(&lc, ::xr::Core::kLogLevelTrace);

        XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
        XR_LOG_TRACE_FORMATTED(&h2,    "TRACE Message %x", 0xff);

        // If the number of levels changes (unlikely) this test should be updated to
        // include the new levels.
        XR_ASSERT_ALWAYS_EQ(lc.mTotal, 2);
    }
    {
        // Basic test.
        LogCounter lc;
        xr::Core::LogHandle h1("1");
        xr::Core::LogHandle h2("");

        h1.AddLogConsumer(&lc, ::xr::Core::kLogLevelTrace);

        XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
        XR_LOG_TRACE_FORMATTED(&h2,    "TRACE Message %x", 0xff);

        // If the number of levels changes (unlikely) this test should be updated to
        // include the new levels.
        XR_ASSERT_ALWAYS_EQ(lc.mTotal, 1);
    }
    {
        // Basic test.
        LogCounter lc;
        xr::Core::LogHandle h1("1");
        xr::Core::LogHandle h2("");

        h2.AddLogConsumer(&lc, ::xr::Core::kLogLevelTrace);

        XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
        XR_LOG_TRACE_FORMATTED(&h2,    "TRACE Message %x", 0xff);

        // If the number of levels changes (unlikely) this test should be updated to
        // include the new levels.
        XR_ASSERT_ALWAYS_EQ(lc.mTotal, 2);
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// This just bounces between 2 unrelated log handles and makes sure they don't
// cause changes in the other.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( PingPongTest )
{
    // Basic test.
    LogCounter lc1;
    LogCounter lc2;

    xr::Core::LogHandle h1("LogTest.Ping");
    xr::Core::LogHandle h2("LogTest.Pong");

    h1.AddLogConsumer(&lc1, ::xr::Core::kLogLevelTrace);
    h2.AddLogConsumer(&lc2, ::xr::Core::kLogLevelTrace);

    XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h1,      XR_STR_WCHAR_T("TRACE Message Test"));

    XR_ASSERT_ALWAYS_EQ(lc1.mTotal, 2);
    XR_ASSERT_ALWAYS_EQ(lc2.mTotal, 0);

    XR_LOG_TRACE_MESSAGE(&h2,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h2,     XR_STR_WCHAR_T("TRACE Message Test"));

    XR_ASSERT_ALWAYS_EQ(lc1.mTotal, 2);
    XR_ASSERT_ALWAYS_EQ(lc2.mTotal, 2);


    h1.RemoveLogConsumer(&lc1);
    h2.RemoveLogConsumer(&lc2);

}
// --------------------------------------------------------------------------------------  FUNCTION
// Test that each level responds to basic usage.
// --------------------------------------------------------------------------------------  FUNCTION
void LogAllLevelsTest(const xr::Core::LogHandle *h, const LogCounter *lc, int32_t trace, int32_t debug, int32_t info, int32_t warning, int32_t error, int32_t fatal)
{
    XR_LOG_TRACE_MESSAGE(h,      "TRACE Message Test");
    XR_LOG_TRACE_FORMATTED(h,    "TRACE Message %x", 0xff);
    XR_LOG_DEBUG_MESSAGE(h,      "DEBUG Message Test");
    XR_LOG_DEBUG_FORMATTED(h,    "DEBUG Message %x", 0xff);
    XR_LOG_INFO_MESSAGE(h,      "INFO Message Test");
    XR_LOG_INFO_FORMATTED(h,    "INFO Message %x", 0xff);
    XR_LOG_WARNING_MESSAGE(h,   "WARNING Message Test");
    XR_LOG_WARNING_FORMATTED(h, "WARNING Message %x", 0xff);
    XR_LOG_ERROR_MESSAGE(h,     "ERROR Message Test");
    XR_LOG_ERROR_FORMATTED(h,   "ERROR Message %x", 0xff);
    XR_LOG_FATAL_MESSAGE(h,     "FATAL Message Test");
    XR_LOG_FATAL_FORMATTED(h,   "FATAL Message %x", 0xff);

    // The * 2 in the count is because we actually write to each one twice
    XR_ASSERT_ALWAYS_EQ(lc->mPerLevel[xr::Core::kLogLevelTrace], trace* 2);
    XR_ASSERT_ALWAYS_EQ(lc->mPerLevel[xr::Core::kLogLevelDebug], debug* 2);
    XR_ASSERT_ALWAYS_EQ(lc->mPerLevel[xr::Core::kLogLevelInfo], info* 2);
    XR_ASSERT_ALWAYS_EQ(lc->mPerLevel[xr::Core::kLogLevelWarning], warning* 2);
    XR_ASSERT_ALWAYS_EQ(lc->mPerLevel[xr::Core::kLogLevelError], error* 2);
    XR_ASSERT_ALWAYS_EQ(lc->mPerLevel[xr::Core::kLogLevelFatal], fatal* 2);

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void PropTestInternalParentChild(const char * parent, const char * child)
{
    // Basic test.
    LogCounter lc1;
    LogCounter lc2;

    xr::Core::LogHandle h1(parent);
    xr::Core::LogHandle h2(child);

    h1.AddLogConsumer(&lc1, ::xr::Core::kLogLevelTrace);

    // All messages should go to "LogTest"
    XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h1,      XR_STR_WCHAR_T("TRACE Message Test"));
    XR_LOG_TRACE_MESSAGE(&h2,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h2,      XR_STR_WCHAR_T("TRACE Message Test"));

    XR_ASSERT_ALWAYS_EQ(lc1.mTotal, 4);
    XR_ASSERT_ALWAYS_EQ(lc2.mTotal, 0);

    lc1.Reset();
    // h1 messages should go to both "LogTest", but "LogTest.Pong"
    // h2 messages should only go to the child.
    h2.AddLogConsumer(&lc2, ::xr::Core::kLogLevelTrace);

    XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h1,      XR_STR_WCHAR_T("TRACE Message Test"));
    XR_LOG_TRACE_MESSAGE(&h2,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h2,     XR_STR_WCHAR_T("TRACE Message Test"));

    XR_ASSERT_ALWAYS_EQ(lc1.mTotal, 4);
    XR_ASSERT_ALWAYS_EQ(lc2.mTotal, 2);

    lc1.Reset();
    lc2.Reset();
    xr::Core::LogHandle h11(parent);
    xr::Core::LogHandle h22(child);

    XR_LOG_TRACE_MESSAGE(&h11,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h11,      XR_STR_WCHAR_T("TRACE Message Test"));
    XR_LOG_TRACE_MESSAGE(&h22,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h22,     XR_STR_WCHAR_T("TRACE Message Test"));

    XR_ASSERT_ALWAYS_EQ(lc1.mTotal, 4);
    XR_ASSERT_ALWAYS_EQ(lc2.mTotal, 2);

    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h1, &lc1, 1, 1, 1, 1, 1, 1);
    // Now make sure that changes to h11 change h1
    h11.SetLogConsumerLevel(&lc1, ::xr::Core::kLogLevelDebug);

    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h1, &lc1, 0, 1, 1, 1, 1, 1);

    LogAllLevelsTest(&h2, &lc2, 1, 1, 1, 1, 1, 1);
    // Now make sure that changes to h22 change h2
    h22.SetLogConsumerLevel(&lc2, ::xr::Core::kLogLevelDebug);

    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h2, &lc2, 0, 1, 1, 1, 1, 1);

    // Now make sure that changes to h1 change h11
    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h11, &lc1, 0, 1, 1, 1, 1, 1);

    h1.SetLogConsumerLevel(&lc1, ::xr::Core::kLogLevelInfo);

    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h11, &lc1, 0, 0, 1, 1, 1, 1);

    // Now make sure that changes to h2 change h22
    LogAllLevelsTest(&h22, &lc2, 0, 1, 1, 1, 1, 1);

    h2.SetLogConsumerLevel(&lc2, ::xr::Core::kLogLevelInfo);

    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h22, &lc2, 0, 0, 1, 1, 1, 1);


    h1.RemoveLogConsumer(&lc1);
    h2.RemoveLogConsumer(&lc2);

}

// --------------------------------------------------------------------------------------  FUNCTION
// Tests propagation of settings by handle.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( PropTest )
{
    PropTestInternalParentChild("logTest", "logTest.Pong");
    PropTestInternalParentChild("logTest", "logTest.a");
    PropTestInternalParentChild("logTest", "logTest.");
    PropTestInternalParentChild("logTest", "logTest.a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p");
    PropTestInternalParentChild("", ".");
    PropTestInternalParentChild("", ".test");
    PropTestInternalParentChild("", ".test.");
    PropTestInternalParentChild(".", "..");
    PropTestInternalParentChild(".", "..test");
    PropTestInternalParentChild(".", "..test.");
}
// --------------------------------------------------------------------------------------  FUNCTION
// Man in middle creation / deletion. Make sure that deleting duplicate
// handles does not allow usage of stale memory. essentially try to be
// creative with the order in which handles are created and deleted in
// the list to ensure the list remains valid.
// --------------------------------------------------------------------------------------  FUNCTION
template <typename T>
static void DeleteHelper_Temp(T * p)
{
    p->~T();
}
void internal_test()
{
#define TEST_CREATE_HANDLE( index, log_name ) \
    test[index] = new (test_buff + ((sizeof(xr::Core::LogHandle)/sizeof(uint64_t)+1) * index) ) xr::Core::LogHandle(log_name);

#define TEST_DELETE_HANDLE( index) \
    DeleteHelper_Temp<xr::Core::LogHandle>(test[index]); \
    ::xr::Core::MemFill8(test[index], 0xFF, sizeof(xr::Core::LogHandle)); \
    test[index] = nullptr


    xr::Core::LogHandle * test[3] = {nullptr,nullptr,nullptr};

    uint64_t test_buff[(sizeof(xr::Core::LogHandle)/sizeof(uint64_t)+1) * 3];

    for(uint32_t i = 0; i < 3; i++)
    {
        for(uint32_t j = 0; j < 3; j++)
        {
            if(j == i)
            {
                continue;
            }
            for(uint32_t k = 0; k < 3; k++)
            {
                if(k == i || k == j)
                {
                    continue;
                }

                TEST_CREATE_HANDLE( 0, "test");
                TEST_CREATE_HANDLE( 1, "test.1");
                TEST_CREATE_HANDLE( 2, "test.1.2");

                XR_ASSERT_ALWAYS_TRUE(xr::Core::LogSystemHasHandleForName("test"));
                XR_ASSERT_ALWAYS_TRUE(xr::Core::LogSystemHasHandleForName("test.1"));
                XR_ASSERT_ALWAYS_TRUE(xr::Core::LogSystemHasHandleForName("test.1.2"));

                XR_LOG_TRACE_MESSAGE(test[i],      "TRACE Message Test");

                TEST_DELETE_HANDLE( i);

                XR_LOG_TRACE_MESSAGE(test[j],      "TRACE Message Test");

                TEST_DELETE_HANDLE( j );

                XR_LOG_TRACE_MESSAGE(test[k],      "TRACE Message Test");

                TEST_DELETE_HANDLE( k );

            }
        }
    }
#undef TEST_CREATE_HANDLE
#undef TEST_DELETE_HANDLE
}

XR_UNITTEST_TEST_FUNC( middle )
{

    {
        internal_test();
    }
    {
        xr::Core::LogHandle h("unrelated");
        internal_test();
        XR_LOG_TRACE_MESSAGE(&h,      "TRACE Message Test");
    }
    {
        xr::Core::LogHandle h("test");
        internal_test();
        XR_LOG_TRACE_MESSAGE(&h,      "TRACE Message Test");
    }
    {
        xr::Core::LogHandle h("test.1");
        internal_test();
        XR_LOG_TRACE_MESSAGE(&h,      "TRACE Message Test");
    }
    {
        xr::Core::LogHandle h("test.1.2");
        internal_test();
        XR_LOG_TRACE_MESSAGE(&h,      "TRACE Message Test");
    }
    {
        xr::Core::LogHandle h("test.1.2.3");
        internal_test();
        XR_LOG_TRACE_MESSAGE(&h,      "TRACE Message Test");
    }
    {
        xr::Core::LogHandle h0("test");
        xr::Core::LogHandle h1("test.1");
        xr::Core::LogHandle h2("test.1.2");
        xr::Core::LogHandle h3("test.1.2.3");
        xr::Core::LogHandle h4("test.1.2.3.4");
        internal_test();
        XR_LOG_TRACE_MESSAGE(&h0,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h2,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h3,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h4,      "TRACE Message Test");
    }
    {
        xr::Core::LogHandle h0("test");
        xr::Core::LogHandle h1("test.1");
        xr::Core::LogHandle h2("test.1.2");
        xr::Core::LogHandle h3("test.1.2.3");
        xr::Core::LogHandle h4("test.1.2.3.4");
        xr::Core::LogHandle h5("test");
        xr::Core::LogHandle h6("test.1");
        xr::Core::LogHandle h7("test.1.2");
        xr::Core::LogHandle h8("test.1.2.3");
        xr::Core::LogHandle h9("test.1.2.3.4");
        internal_test();
        XR_LOG_TRACE_MESSAGE(&h0,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h2,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h3,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h4,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h5,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h6,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h7,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h8,      "TRACE Message Test");
        XR_LOG_TRACE_MESSAGE(&h9,      "TRACE Message Test");
    }

}
// --------------------------------------------------------------------------------------  FUNCTION
// Tests propagation of settings by name.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( ByName )
{
    {

    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest"));
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest.Pong"));

    // Basic test.
    LogCounter lc1;
    LogCounter lc2;

    xr::Core::LogHandle h1("LogTest");
    xr::Core::LogHandle h2("LogTest.Pong");

    h1.AddLogConsumer(&lc1, ::xr::Core::kLogLevelTrace);

    // All messages should go to "LogTest"
    XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h1,      XR_STR_WCHAR_T("TRACE Message Test"));
    XR_LOG_TRACE_MESSAGE(&h2,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h2,      XR_STR_WCHAR_T("TRACE Message Test"));

    XR_ASSERT_ALWAYS_EQ(lc1.mTotal, 4);

    lc1.Reset();
    // h2 messages should no longer go to "LogTest", but "LogTest.Pong"
    xr::Core::AddLogConsumer("LogTest", &lc2, ::xr::Core::kLogLevelTrace);

    XR_LOG_TRACE_MESSAGE(&h1,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h1,      XR_STR_WCHAR_T("TRACE Message Test"));
    XR_LOG_TRACE_MESSAGE(&h2,      "TRACE Message Test");
    XR_LOG_TRACE_MESSAGE(&h2,     XR_STR_WCHAR_T("TRACE Message Test"));

    XR_ASSERT_ALWAYS_EQ(lc1.mTotal, 4);
    XR_ASSERT_ALWAYS_EQ(lc2.mTotal, 4);

    lc1.Reset();
    lc2.Reset();
    xr::Core::LogHandle h3("LogTest.Pong");

    xr::Core::SetLogConsumerLevel("LogTest", &lc2, ::xr::Core::kLogLevelDebug);

    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h1, &lc1, 1, 1, 1, 1, 1, 1);
    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h2, &lc1, 1, 1, 1, 1, 1, 1);
    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h3, &lc1, 1, 1, 1, 1, 1, 1);
    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h1, &lc2, 0, 1, 1, 1, 1, 1);
    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h2, &lc2, 0, 1, 1, 1, 1, 1);
    lc1.Reset();
    lc2.Reset();
    LogAllLevelsTest(&h3, &lc2, 0, 1, 1, 1, 1, 1);

    // h2 messages should no longer go to "LogTest", but "LogTest.Pong"
    xr::Core::RemoveLogConsumer("LogTest", &lc2);


    }

    xr::Core::LogSystemShutdown();

    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest"));
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest.Pong"));
}

void LinkingTestInternal()
{
    xr::Core::LogHandle h1("LogTest");
    xr::Core::LogHandle h2("LogTest");
    xr::Core::LogHandle h3("LogTest");

    xr::Core::LogHandle h4("LogTest.Pong");
    xr::Core::LogHandle h5("LogTest.Pong");
    xr::Core::LogHandle h6("LogTest.Pong");

    xr::Core::LogHandle h7("LogTest.Ponga");
    xr::Core::LogHandle h8("LogTest.Ponga");
    xr::Core::LogHandle h9("LogTest.Ponga");

    LogCounter lc1;

    xr::Core::AddLogConsumer("LogTest", &lc1, ::xr::Core::kLogLevelTrace);

    lc1.Reset();
    LogAllLevelsTest(&h1, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h2, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h3, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h4, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h5, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h6, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h7, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h8, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h9, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();

    h1.SetLogConsumerLevel(&lc1,::xr::Core::kLogLevelDebug);
    LogAllLevelsTest(&h1, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h2, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h3, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h4, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h5, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h6, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h7, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h8, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h9, &lc1, 0, 1, 1, 1, 1, 1); lc1.Reset();

    h2.SetLogConsumerLevel(&lc1,::xr::Core::kLogLevelInfo);
    LogAllLevelsTest(&h1, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h2, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h3, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h4, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h5, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h6, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h7, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h8, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h9, &lc1, 0, 0, 1, 1, 1, 1); lc1.Reset();

    h3.SetLogConsumerLevel(&lc1,::xr::Core::kLogLevelTrace);
    LogAllLevelsTest(&h1, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h2, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h3, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h4, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h5, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h6, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h7, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h8, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h9, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();


    // Test by setting a child to have an additional. and see what happens
    LogCounter lc2;
    xr::Core::AddLogConsumer("LogTest.Pong", &lc2, ::xr::Core::kLogLevelTrace);


    h4.SetLogConsumerLevel(&lc2, ::xr::Core::kLogLevelTrace);
    LogAllLevelsTest(&h1, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h2, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h3, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h4, &lc2, 1, 1, 1, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h5, &lc2, 1, 1, 1, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h6, &lc2, 1, 1, 1, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h7, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h8, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h9, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    lc1.Reset();
    LogAllLevelsTest(&h1, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h2, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h3, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h4, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h5, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h6, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h7, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h8, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h9, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();

    h5.SetLogConsumerLevel(&lc2, ::xr::Core::kLogLevelDebug);
    lc2.Reset();
    LogAllLevelsTest(&h1, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h2, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h3, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h4, &lc2, 0, 1, 1, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h5, &lc2, 0, 1, 1, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h6, &lc2, 0, 1, 1, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h7, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h8, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h9, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    lc1.Reset();
    LogAllLevelsTest(&h1, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h2, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h3, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h4, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h5, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h6, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h7, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h8, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h9, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();

    h6.SetLogConsumerLevel(&lc2, ::xr::Core::kLogLevelWarning);
    lc2.Reset();
    LogAllLevelsTest(&h1, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h2, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h3, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h4, &lc2, 0, 0, 0, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h5, &lc2, 0, 0, 0, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h6, &lc2, 0, 0, 0, 1, 1, 1); lc2.Reset();
    LogAllLevelsTest(&h7, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h8, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    LogAllLevelsTest(&h9, &lc2, 0, 0, 0, 0, 0, 0); lc2.Reset();
    lc1.Reset();
    LogAllLevelsTest(&h1, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h2, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h3, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h4, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h5, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h6, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h7, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h8, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();
    LogAllLevelsTest(&h9, &lc1, 1, 1, 1, 1, 1, 1); lc1.Reset();

    xr::Core::RemoveLogConsumer("LogTest", &lc1);
    xr::Core::RemoveLogConsumer("LogTest.Pong", &lc2);

    xr::Core::LogSystemShutdown();
}

// --------------------------------------------------------------------------------------  FUNCTION
// Test that values are propagated forward and backward in the list. Make sure
// there are no edge conditions of propagation to similar named items.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Linking )
{
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest"));
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest.Pong"));
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest.Ponga"));

    LinkingTestInternal();

    xr::Core::LogHandle h1("LogTes");
    xr::Core::LogHandle h2("LogTesta");
    xr::Core::LogHandle h3("LogTest.");

    LinkingTestInternal();

    xr::Core::LogHandle h4("LogTest.Pon");
    xr::Core::LogHandle h5("LogTest.Ponh");
    xr::Core::LogHandle h6("LogTest.Pong.test.test.test.test");

    LinkingTestInternal();

    xr::Core::LogHandle h7("");
    xr::Core::LogHandle h8("a");
    xr::Core::LogHandle h9(".");
    xr::Core::LogHandle h10("z");

    LinkingTestInternal();

    xr::Core::LogSystemShutdown();

    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest"));
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest.Pong"));
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest.Ponga"));
}

void LinkingTest2Internal(const char * a, const char * b, const char * c)
{
    xr::Core::LogHandle h1(a);
    {
        xr::Core::LogHandle h2(b);
        {
            xr::Core::LogHandle h3(c);
        }
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// Test that values are propagated forward and backward in the list. Make sure
// there are no edge conditions of propagation to similar named items.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Linking2 )
{
    const char * chars[] = { "a", "b", "c" };

    for(size_t x = 0; x < 3; x++)
    {
        for(size_t y = 0; y < 3; y++)
        {
            for(size_t z = 0; z < 3; z++)
            {
                LinkingTest2Internal(chars[x], chars[y], chars[z]);
            }
        }

    }


}

// --------------------------------------------------------------------------------------  FUNCTION
// This allows us to test defensive code (error paths)
// --------------------------------------------------------------------------------------  FUNCTION
class AssertFiredCounterLogger: public xr::Core::IAssertFailedHandler
{
public:
    AssertFiredCounterLogger(): xr::Core::IAssertFailedHandler(),mResponse(xr::Core::kActionContinue), mCount(0){};
    xr::Core::AssertFailAction AssertFailed(xr::Core::AssertCallType , const char * , const int32_t , const char * )
    {
        mCount++;
        return mResponse;
    }

    xr::Core::AssertFailAction AssertFailedFormatted(xr::Core::AssertCallType , const char * , const int32_t ,
#if defined(XR_COMPILER_MICROSOFT)
        _Printf_format_string_
#endif
        const char *, ... )
    {
        mCount++;
        return mResponse;
    }

    inline void   SetResponse(xr::Core::AssertFailAction reponse) { mResponse = reponse;}

    inline int32_t GetCount() { return mCount;}
    inline void ResetCount() { mCount = 0;}
private:
    xr::Core::AssertFailAction mResponse;
    int32_t					 mCount;
};


class TempAllocator: public xr::Core::IUnknownHelper1<TempAllocator, xr::Core::IAllocator, xr::Core::NotRefCounted>{
public:
    XR_COM_CLASS_ID(0x2a2511f);

    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    inline TempAllocator(){isAllocated = false;}
    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    inline ~TempAllocator() {}
    /*---------------------------------------------------------------*/
    /// Alloc Function
    /*---------------------------------------------------------------*/
    XR_RETURN_BYTES(size)
    XR_RESTRICT_RETURN void * Alloc(
        size_t size,
        size_t alignment,
        xr::Core::MemFlags ,
        const char * ,
        const char * ,
        int32_t )
    {
        // This just tells us if the alignment of this test object needs to be updated.
        // this will not actually be an error in the library itself.
        XR_ASSERT_ALWAYS_EQ( ((uintptr_t)space) & ( alignment - 1), 0 );
        XR_ASSERT_ALWAYS_EQ( size, sizeof(space) );
        isAllocated = true;
        return space;
    }
    /*---------------------------------------------------------------*/
    /// Free function
    /*---------------------------------------------------------------*/
    void   Free(XR_OUT_INVALIDATE void * ptr, size_t , const char * , int32_t )
    {
        XR_ASSERT_ALWAYS_EQ(ptr, space);
        isAllocated = false;
    }
    /*---------------------------------------------------------------*/
    /// Function that returns true if the ptr was allocated from this allocator.
    /*---------------------------------------------------------------*/
    bool   DoesContainPointer(void * ptr)
    {
        return (ptr == space);
    }
    /*---------------------------------------------------------------*/
    /// For debugging purposes.
    /*---------------------------------------------------------------*/
    const char * GetName() const
    {
        return "Logging Temp";
    }

    xr::Core::LogHandle * GetLogHandle()
    {
        union Temp
        {
            uintptr_t * a;
            xr::Core::LogHandle * b;
        };
        Temp t;
        t.a = space;
        return t.b;
    }

    bool isAllocated;
private:
    uintptr_t space[ sizeof(xr::Core::LogHandle) / sizeof(uintptr_t) ];

};
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( OnTheFly )
{
    TempAllocator ta;
    xr::Core::IAllocator &original = xr::Core::SetGeneralAllocator(ta);

    xr::Core::LogHandle h1("LogTest");

    // This tests functionality that is not fully functional yet.
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest.CreatedOnTheFly"));
    xr::Core::AddLogConsumer("LogTest.CreatedOnTheFly", nullptr, ::xr::Core::kLogLevelMax);

    XR_ASSERT_ALWAYS_EQ(ta.isAllocated, true);

    xr::Core::LogHandle * pHandle = ta.GetLogHandle();

    XR_DELETE(pHandle);

    xr::Core::SetGeneralAllocator(original);

    xr::Core::LogSystemShutdown();
    XR_ASSERT_ALWAYS_FALSE(xr::Core::LogSystemHasHandleForName("LogTest.CreatedOnTheFly"));

}

// --------------------------------------------------------------------------------------  FUNCTION
// This verifies that setting a level really changes what is logged.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( LevelTest )
{
    // Basic test.
    LogCounter lc;

    xr::Core::LogHandle h("LogTest.Basic");

    h.AddLogConsumer(&lc, ::xr::Core::kLogLevelTrace);

    LogAllLevelsTest(&h, &lc, 1, 1, 1, 1, 1, 1);
    h.SetLogConsumerLevel(&lc,::xr::Core::kLogLevelDebug);
    LogAllLevelsTest(&h, &lc, 1, 2, 2, 2, 2, 2);
    h.SetLogConsumerLevel(&lc,::xr::Core::kLogLevelInfo);
    LogAllLevelsTest(&h, &lc, 1, 2, 3, 3, 3, 3);
    h.SetLogConsumerLevel(&lc,::xr::Core::kLogLevelWarning);
    LogAllLevelsTest(&h, &lc, 1, 2, 3, 4, 4, 4);
    h.SetLogConsumerLevel(&lc,::xr::Core::kLogLevelError);
    LogAllLevelsTest(&h, &lc, 1, 2, 3, 4, 5, 5);
    h.SetLogConsumerLevel(&lc,::xr::Core::kLogLevelFatal);
    LogAllLevelsTest(&h, &lc, 1, 2, 3, 4, 5, 6);

}



// --------------------------------------------------------------------------------------  FUNCTION
// This verifies format string overflow issues in formatted logs.
// This is not meant to be a test on vsnprintf itself, just that we are using
// it properly. an exhaustive test on vsprintf is well beyond the scope of this
// library
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( BoundsTest )
{
    // Basic test.
    LogCounter lc;

    xr::Core::LogHandle h("LogTest.Basic");

    h.AddLogConsumer(&lc, ::xr::Core::kLogLevelTrace);

    const size_t testLength = ::xr::Core::LogHandle::kLogHandleMaxFormattedStringSize + 100;
    char     tester[testLength];
    wchar_t wtester[testLength];

    ::xr::Core::MemFill8( tester, 'x', testLength* sizeof(tester[0]));
    tester[testLength-1] = '\0';

    for(size_t i = 0; i < testLength; i++)
    {
        wtester[i] = XR_STR_WCHAR_T('y');
    }
    wtester[testLength-1] = XR_STR_WCHAR_T( '\0' );

    XR_ASSERT_ALWAYS_EQ(xr::Core::StringLengthWithNull(tester), testLength);
    XR_ASSERT_ALWAYS_EQ(xr::Core::StringLengthWithNull(wtester), testLength);

    // Test the format string itself being too long.
    XR_LOG_TRACE_FORMATTED(&h,    tester);

    XR_ASSERT_ALWAYS_EQ(lc.mLastMessageLengthIncludingNull, ::xr::Core::LogHandle::kLogHandleMaxFormattedStringSize);

    XR_LOG_TRACE_FORMATTED(&h,    wtester);

    XR_ASSERT_ALWAYS_EQ(lc.mLastMessageLengthIncludingNull, ::xr::Core::LogHandle::kLogHandleMaxFormattedStringSize);

    char nf[] = "%s";
    wchar_t wf[] = XR_STR_WCHAR_T("%ls");

    // Test the result being too long.
    XR_LOG_TRACE_FORMATTED(&h,    nf, tester);
    XR_ASSERT_ALWAYS_EQ(lc.mLastMessageLengthIncludingNull, ::xr::Core::LogHandle::kLogHandleMaxFormattedStringSize);

    XR_LOG_TRACE_FORMATTED(&h,    wf, wtester);
    XR_ASSERT_ALWAYS_EQ(lc.mLastMessageLengthIncludingNull, ::xr::Core::LogHandle::kLogHandleMaxFormattedStringSize);


}


// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( CoverageComplete )
{

}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( Cleanup )
{
    using namespace xr::Core;

    LogHandle * h = LogHandle::GetFirstHandle();
    // Check that the tests have not leaked any log handles.
    // Note that this check may need to be more intelligent if any other handles
    // are setup.
    XR_ASSERT_ALWAYS_EQ(h, sStartingLogHandle);
}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
