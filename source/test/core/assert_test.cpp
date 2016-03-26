// ######################################################################################### - FILE
/*!

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE

#if defined(XR_CODE_COVERAGE_BUILD)
// coverage build disables asserts, but assert tests needs them.
// This define lets the header know that this is the assert test.
#define XR_ASSERT_INTERNAL_TESTS 1
#endif

#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif

#include <stdio.h>
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Assert )

class AssertFiredCounter: public xr::Core::IUnknownHelper1<AssertFiredCounter, ::xr::Core::IAssertFailedHandler, xr::Core::NotRefCounted>
{
public:
    XR_COM_CLASS_ID(0x038b6d4);

    AssertFiredCounter(): mResponse(xr::Core::kActionBreak), mCount(0){};

    xr::Core::AssertFailAction AssertFailed(xr::Core::AssertCallType , const char * , const int32_t , const char * ) XR_OVERRIDE
    {
        mCount++;
        return mResponse;
    }

    xr::Core::AssertFailAction AssertFailedFormatted(
        xr::Core::AssertCallType ,
        const char * ,
        const int32_t ,
#if defined(XR_COMPILER_MICROSOFT)
        _Printf_format_string_
#endif
        const char *, ... ) XR_OVERRIDE
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

class AssertHandlerRestorer
{
public:
    AssertHandlerRestorer(xr::Core::IAssertFailedHandler & newHandler) : mOriginalHandler(xr::Core::Assert::SetFailHandler(newHandler))
    {
    }
    ~AssertHandlerRestorer()
    {
        // reset to default state
        xr::Core::Assert::SetFailHandler(mOriginalHandler);
    }
private:
    xr::Core::IAssertFailedHandler & mOriginalHandler;

    AssertHandlerRestorer & operator = (AssertHandlerRestorer);
};

#define XR_TEMP_SHOULD_EQ(x, y) if(x != y){ XR_HALT(); ::xr::Core::Test::FailCurrentTest(XR_FILE_LINE, #x " SHould Equal " #y); }
// --------------------------------------------------------------------------------------  FUNCTION
// Really basic test, ensures that the assert system is minimally functional
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( BasicAssertTest )
{
    AssertFiredCounter filter;
    AssertHandlerRestorer testHelper(filter);
    filter.SetResponse(xr::Core::kActionContinue);

    XR_TEMP_SHOULD_EQ(filter.GetCount(), 0);
    XR_ASSERT_ALWAYS_EQ(1,1);
    XR_TEMP_SHOULD_EQ(filter.GetCount(), 0);
    XR_ASSERT_ALWAYS_EQ(1,2);
    XR_TEMP_SHOULD_EQ(filter.GetCount(), 1);
    XR_ASSERT_ALWAYS_EQ(1,2);
    XR_TEMP_SHOULD_EQ(filter.GetCount(), 2);

    int blah = 0;
    XR_ASSERT_ALWAYS_EQ_FM(1,2, "Testing %d", blah++ );
    XR_TEMP_SHOULD_EQ(blah, 1);

}


//---------------------------------------------------------------------------------------------
// This is an ugly macro mess, but it's purpose is to test every permutation of every assert
// macro to ensure it properly reports errors.
//---------------------------------------------------------------------------------------------
// Just in case. This will prevent odd errors in the macro pasting
#undef ASSERT_
#undef EXPECT_
#undef ALWAYS_

#define SHOULD_PASS(expr) \
{ \
    test.ResetCount(); \
    expr; \
    if(test.GetCount() != 0) \
    { \
        xr::Core::Test::FailCurrentTest( XR_FILE_LINE, #expr );  \
    } \
}

#define SHOULD_FAIL(expr) \
{ \
    test.ResetCount(); \
    expr; \
    if(test.GetCount() != 1) \
    { \
        xr::Core::Test::FailCurrentTest( XR_FILE_LINE, #expr ); \
    } \
}

#define TYPE_TEST( WHEN, AORE, MINVAL, MAXVAL)\
{ \
SHOULD_PASS( XR_##AORE##WHEN##FALSE( false         ) );\
SHOULD_FAIL( XR_##AORE##WHEN##FALSE(       true          ) );\
SHOULD_PASS( XR_##AORE##WHEN##TRUE(        true          ) );\
SHOULD_FAIL( XR_##AORE##WHEN##TRUE(        false         ) );\
SHOULD_PASS( XR_##AORE##WHEN##EQ(          MINVAL,MINVAL ) );\
SHOULD_FAIL( XR_##AORE##WHEN##EQ(          MINVAL,MAXVAL ) );\
SHOULD_PASS( XR_##AORE##WHEN##NE(          MINVAL,MAXVAL ) );\
SHOULD_FAIL( XR_##AORE##WHEN##NE(          MINVAL,MINVAL ) );\
SHOULD_PASS( XR_##AORE##WHEN##LT(          MINVAL,MAXVAL ) );\
SHOULD_FAIL( XR_##AORE##WHEN##LT(          MINVAL,MINVAL ) );\
SHOULD_FAIL( XR_##AORE##WHEN##LT(          MAXVAL,MINVAL ) );\
SHOULD_PASS( XR_##AORE##WHEN##LE(          MINVAL,MAXVAL ) );\
SHOULD_PASS( XR_##AORE##WHEN##LE(          MINVAL,MINVAL ) );\
SHOULD_FAIL( XR_##AORE##WHEN##LE(          MAXVAL,MINVAL) );\
SHOULD_PASS( XR_##AORE##WHEN##GT(          MAXVAL,MINVAL ) );\
SHOULD_FAIL( XR_##AORE##WHEN##GT(          MINVAL,MINVAL ) );\
SHOULD_FAIL( XR_##AORE##WHEN##GT(          MINVAL,MAXVAL ) );\
SHOULD_PASS( XR_##AORE##WHEN##GE(          MAXVAL,MINVAL ) );\
SHOULD_PASS( XR_##AORE##WHEN##GE(          MINVAL,MINVAL ) );\
SHOULD_FAIL( XR_##AORE##WHEN##GE(          MINVAL,MAXVAL ) );\
SHOULD_FAIL( XR_##WHEN##UNEXPECTED     ()    );\
SHOULD_FAIL( XR_##WHEN##FAIL           ()    );\
SHOULD_PASS( XR_##AORE##WHEN##FALSE_M( false         , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##FALSE_M(       true          , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##TRUE_M(        true          , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##TRUE_M(        false         , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##EQ_M(          MINVAL,MINVAL , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##EQ_M(          MINVAL,MAXVAL , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##NE_M(          MINVAL,MAXVAL , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##NE_M(          MINVAL,MINVAL , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##LT_M(          MINVAL,MAXVAL , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##LT_M(          MINVAL,MINVAL , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##LT_M(          MAXVAL,MINVAL , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##LE_M(          MINVAL,MAXVAL , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##LE_M(          MINVAL,MINVAL , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##LE_M(          MAXVAL,MINVAL , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##GT_M(          MAXVAL,MINVAL , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##GT_M(          MINVAL,MINVAL , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##GT_M(          MINVAL,MAXVAL , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##GE_M(          MAXVAL,MINVAL , "test") );\
SHOULD_PASS( XR_##AORE##WHEN##GE_M(          MINVAL,MINVAL , "test") );\
SHOULD_FAIL( XR_##AORE##WHEN##GE_M(          MINVAL,MAXVAL , "test") );\
SHOULD_FAIL( XR_##WHEN##UNEXPECTED_M     ( "test")    );\
SHOULD_FAIL( XR_##WHEN##FAIL_M           ( "test")    );\
SHOULD_PASS( XR_##AORE##WHEN##FALSE_FM( false         , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##FALSE_FM(       true          , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##TRUE_FM(        true          , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##TRUE_FM(        false         , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##EQ_FM(          MINVAL,MINVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##EQ_FM(          MINVAL,MAXVAL , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##NE_FM(          MINVAL,MAXVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##NE_FM(          MINVAL,MINVAL , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##LT_FM(          MINVAL,MAXVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##LT_FM(          MINVAL,MINVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##LT_FM(          MAXVAL,MINVAL , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##LE_FM(          MINVAL,MAXVAL , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##LE_FM(          MINVAL,MINVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##LE_FM(          MAXVAL,MINVAL , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##GT_FM(          MAXVAL,MINVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##GT_FM(          MINVAL,MINVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##GT_FM(          MINVAL,MAXVAL , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##GE_FM(          MAXVAL,MINVAL , "test %d", 0) );\
SHOULD_PASS( XR_##AORE##WHEN##GE_FM(          MINVAL,MINVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##AORE##WHEN##GE_FM(          MINVAL,MAXVAL , "test %d", 0) );\
SHOULD_FAIL( XR_##WHEN##UNEXPECTED_FM     ( "test %d", 0)    );\
SHOULD_FAIL( XR_##WHEN##FAIL_FM           ( "test %d", 0)    );\
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template <typename T> void defineTester(AssertFiredCounter &test, T v0, T v1)
{
    TYPE_TEST(TEST_,ASSERT_,v0, v1);
    TYPE_TEST(TEST_,EXPECT_,v0, v1);
    TYPE_TEST(DEBUG_,ASSERT_,v0, v1);
    TYPE_TEST(DEBUG_,EXPECT_,v0, v1);
    TYPE_TEST(ALWAYS_,ASSERT_,v0, v1);
    TYPE_TEST(ALWAYS_,EXPECT_,v0, v1);
}

// --------------------------------------------------------------------------------------  FUNCTION
// This tests every permutation of assert macro for every supported data type
// to ensure that it succeeds when the argument is valid and fails when it is not.
// Specifically this tests if the expressions are correctly evaluated for all
// permutations
//
// This is split into multiple tests to keep any single test from being huge
// and killing the compiler.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( ValidateDefines1 )
{
    AssertFiredCounter test;
    AssertHandlerRestorer testHelper(test);
    test.SetResponse(xr::Core::kActionContinue);

    defineTester<uint8_t>(test, 0, 1);
    defineTester<uint16_t>(test, 0, 1);
    defineTester<uint32_t>(test, 0, 1);
    defineTester<uint64_t>(test, 0, 1);
    defineTester<uintptr_t>(test, 0, 1);
    defineTester< intptr_t>(test, 0, 1);
    defineTester<size_t>(test, 0, 1);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( ValidateDefines2 )
{
    AssertFiredCounter test;
    AssertHandlerRestorer testHelper(test);
    test.SetResponse(xr::Core::kActionContinue);

    defineTester<int8_t>(test, 0, 1);
    defineTester<int16_t>(test, 0, 1);
    defineTester<int32_t>(test, 0, 1);
    defineTester<int64_t>(test, 0, 1);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( ValidateDefines3 )
{
    AssertFiredCounter test;
    AssertHandlerRestorer testHelper(test);
    test.SetResponse(xr::Core::kActionContinue);

    defineTester<int8_t>(test,  XR_INT8_MIN, XR_INT8_MAX    );
    defineTester<int16_t>(test, XR_INT16_MIN, XR_INT16_MAX  );
    defineTester<int32_t>(test, XR_INT32_MIN, XR_INT32_MAX  );
    defineTester<int64_t>(test, XR_INT64_MIN, XR_INT64_MAX  );

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( ValidateDefines4 )
{
    AssertFiredCounter test;
    AssertHandlerRestorer testHelper(test);
    test.SetResponse(xr::Core::kActionContinue);


    defineTester<uint8_t>(test,  XR_UINT8_MIN, XR_UINT8_MAX   );
    defineTester<uint16_t>(test, XR_UINT16_MIN, XR_UINT16_MAX );
    defineTester<uint32_t>(test, XR_UINT32_MIN, XR_UINT32_MAX );
    defineTester<uint64_t>(test, XR_UINT64_MIN, XR_UINT64_MAX );
    defineTester<uintptr_t>(test, XR_UINTPTR_MIN, XR_UINTPTR_MAX);
    defineTester< intptr_t>(test, XR_INTPTR_MIN, XR_INTPTR_MAX);
    defineTester<size_t>(test, XR_SIZE_MIN, XR_SIZE_MAX);

}
// --------------------------------------------------------------------------------------  FUNCTION
// This counts references (calls to the "get" function) it's used
// ensure that expressions passed to assert macros are only evaluated once.
// --------------------------------------------------------------------------------------  FUNCTION
template <typename T> class ValueWrapper
{
public:
    ValueWrapper(T val): internal_value(val), internal_count(0) {}
    T       get(){ internal_count++; return internal_value; }
    size_t  GetSetCount() { return internal_count; }
    void    reset() { internal_count = 0; }
private:
    T internal_value;
    size_t internal_count;
};
// --------------------------------------------------------------------------------------  FUNCTION
// The goal of this test is to ensure that all expressions passed are only
// evaluated once, so calling functions is OK. Although in general practice
// you probably shouldn't call anything with side effects inside an assert
// statement...
// This does not test that expressions are properly evaluated, only that the
// expressions passed are not evaluated multiple times.
// It does however test each macro twice (once as a pass and once as a fail)
// this ensures no issues crop up in only one or the other condition.
//
// This is split into multiple tests to keep any single test from being huge
// and killing the compiler.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC(ExpressionReferenceCount1)
{
    AssertFiredCounter temp;
    AssertHandlerRestorer testHelper(temp);
    temp.SetResponse(xr::Core::kActionContinue);

    const char * test =   "xyz";
    const char * format = "%d%x";

    ValueWrapper<bool> b0(true);
    ValueWrapper<bool> b1(false);


#define TPRE()  b0.reset(); b1.reset();
#define TPOST()  if(b0.GetSetCount() != 1) {::xr::Core::Test::FailCurrentTest( XR_FILE_LINE, "b0 count is not correct" );} if(b1.GetSetCount() != 1) {::xr::Core::Test::FailCurrentTest( XR_FILE_LINE, "b1 count is not correct" );}
    TPRE();XR_ASSERT_DEBUG_FALSE(b0.get());XR_ASSERT_TEST_FALSE(b1.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_TRUE(b0.get());XR_ASSERT_TEST_TRUE(b1.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_FALSE_M(b0.get(),test);XR_ASSERT_TEST_FALSE_M(b1.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_TRUE_M(b0.get(),test);XR_ASSERT_TEST_TRUE_M(b1.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_FALSE_FM(b0.get(),format,1,1);XR_ASSERT_TEST_FALSE_FM(b1.get(),format,1,1);TPOST();
    TPRE();XR_ASSERT_DEBUG_TRUE_FM(b0.get(),format,1,1);XR_ASSERT_TEST_TRUE_FM(b1.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_FALSE(b0.get());XR_EXPECT_TEST_FALSE(b1.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_TRUE(b0.get());XR_EXPECT_TEST_TRUE(b1.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_FALSE_M(b0.get(),test);XR_EXPECT_TEST_FALSE_M(b1.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_TRUE_M(b0.get(),test);XR_EXPECT_TEST_TRUE_M(b1.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_FALSE_FM(b0.get(),format,1,1);XR_EXPECT_TEST_FALSE_FM(b1.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_TRUE_FM(b0.get(),format,1,1);XR_EXPECT_TEST_TRUE_FM(b1.get(),format,1,1);TPOST();

#undef TPRE
#undef TPOST

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC(ExpressionReferenceCount2)
{
    AssertFiredCounter temp;
    AssertHandlerRestorer testHelper(temp);
    temp.SetResponse(xr::Core::kActionContinue);

    const char * test =   "xyz";
    const char * format = "%d%x";

    ValueWrapper<uint32_t> v0(0);
    ValueWrapper<uint32_t> v1(1);
    ValueWrapper<uint32_t> v00(0); // duplicate value with separate count

#define TPRE()  { v0.reset(); v00.reset();
#define TPOST()  if(v0.GetSetCount() != 1) {::xr::Core::Test::FailCurrentTest( XR_FILE_LINE, "v0 count is not correct" ); }  if(v00.GetSetCount() != 1) {::xr::Core::Test::FailCurrentTest( XR_FILE_LINE, "v00 count is not correct" );} }
    // EQ / NE, test pass and fail conditions.
    TPRE();XR_ASSERT_DEBUG_EQ(v0.get(),v00.get());TPOST();TPRE();XR_ASSERT_TEST_EQ(v0.get(),v00.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_NE(v0.get(),v00.get());TPOST();TPRE();XR_ASSERT_TEST_NE(v0.get(),v00.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_EQ_M(v0.get(),v00.get(),test);TPOST();TPRE();XR_ASSERT_TEST_EQ_M(v0.get(),v00.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_NE_M(v0.get(),v00.get(),test);TPOST();TPRE();XR_ASSERT_TEST_NE_M(v0.get(),v00.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_EQ_FM(v0.get(),v00.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_EQ_FM(v0.get(),v00.get(),format,1,1);TPOST();
    TPRE();XR_ASSERT_DEBUG_NE_FM(v0.get(),v00.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_NE_FM(v0.get(),v00.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_EQ(v0.get(),v00.get());TPOST();TPRE();XR_EXPECT_TEST_EQ(v0.get(),v00.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_NE(v0.get(),v00.get());TPOST();TPRE();XR_EXPECT_TEST_NE(v0.get(),v00.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_EQ_M(v0.get(),v00.get(),test);TPOST();TPRE();XR_EXPECT_TEST_EQ_M(v0.get(),v00.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_NE_M(v0.get(),v00.get(),test);TPOST();TPRE();XR_EXPECT_TEST_NE_M(v0.get(),v00.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_EQ_FM(v0.get(),v00.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_EQ_FM(v0.get(),v00.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_NE_FM(v0.get(),v00.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_NE_FM(v0.get(),v00.get(),format,1,1);TPOST();

#undef TPRE
#undef TPOST

}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC(ExpressionReferenceCount3)
{
    AssertFiredCounter temp;
    AssertHandlerRestorer testHelper(temp);
    temp.SetResponse(xr::Core::kActionContinue);

    const char * test =   "xyz";
    const char * format = "%d%x";

    ValueWrapper<uint32_t> v0(0);
    ValueWrapper<uint32_t> v1(1);
    ValueWrapper<uint32_t> v00(0); // duplicate value with separate count

#define TPRE()  { v0.reset(); v1.reset();
#define TPOST()  if(v0.GetSetCount() != 1) {::xr::Core::Test::FailCurrentTest( XR_FILE_LINE, "v0 count is not correct" ); }  if(v1.GetSetCount() != 1) {::xr::Core::Test::FailCurrentTest( XR_FILE_LINE, "v00 count is not correct"); } }

    TPRE();XR_ASSERT_DEBUG_EQ(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_TEST_EQ(v0.get(),v1.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_NE(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_TEST_NE(v0.get(),v1.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_EQ_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_TEST_EQ_M(v0.get(),v1.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_NE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_TEST_NE_M(v0.get(),v1.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_EQ_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_EQ_FM(v0.get(),v1.get(),format,1,1);TPOST();
    TPRE();XR_ASSERT_DEBUG_NE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_NE_FM(v0.get(),v1.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_EQ(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_TEST_EQ(v0.get(),v1.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_NE(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_TEST_NE(v0.get(),v1.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_EQ_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_TEST_EQ_M(v0.get(),v1.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_NE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_TEST_NE_M(v0.get(),v1.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_EQ_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_EQ_FM(v0.get(),v1.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_NE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_NE_FM(v0.get(),v1.get(),format,1,1);TPOST();

    // Nothing to test on these, they don't take an expression
    //XR_DEBUG_UNEXPECTED();XR_TEST_UNEXPECTED();
    //XR_DEBUG_UNEXPECTED_M(test);XR_TEST_UNEXPECTED_M(test);
    //XR_DEBUG_UNEXPECTED_FM(format,1,1);XR_TEST_UNEXPECTED_FM(format,1,1);
    //XR_DEBUG_FAIL();XR_TEST_FAIL();
    //XR_DEBUG_FAIL_M(test);XR_TEST_FAIL_M(test);
    //XR_DEBUG_FAIL_FM(format,1,1);XR_TEST_FAIL_FM(format,1,1);

#undef TPRE
#undef TPOST

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC(ExpressionReferenceCount4)
{
    AssertFiredCounter temp;
    AssertHandlerRestorer testHelper(temp);
    temp.SetResponse(xr::Core::kActionContinue);

    const char * test =   "xyz";
    const char * format = "%d%x";

    ValueWrapper<uint32_t> v0(0);
    ValueWrapper<uint32_t> v1(1);
    ValueWrapper<uint32_t> v00(0); // duplicate value with separate count

#define TPRE()  { v0.reset(); v1.reset();
#define TPOST()  if(v0.GetSetCount() != 1) {::xr::Core::Test::FailCurrentTest(XR_FILE_LINE, "v0 count is not correct" ); }  if(v1.GetSetCount() != 1) {::xr::Core::Test::FailCurrentTest( XR_FILE_LINE, "v00 count is not correct"); } }

    // relational defines
    TPRE();XR_ASSERT_DEBUG_LT(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_TEST_LT(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_DEBUG_LT(v1.get(),v0.get());TPOST();TPRE();XR_ASSERT_TEST_LT(v1.get(),v0.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_LE(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_TEST_LE(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_DEBUG_LE(v1.get(),v0.get());TPOST();TPRE();XR_ASSERT_TEST_LE(v1.get(),v0.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_GT(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_TEST_GT(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_DEBUG_GT(v1.get(),v0.get());TPOST();TPRE();XR_ASSERT_TEST_GT(v1.get(),v0.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_GE(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_TEST_GE(v0.get(),v1.get());TPOST();TPRE();XR_ASSERT_DEBUG_GE(v1.get(),v0.get());TPOST();TPRE();XR_ASSERT_TEST_GE(v1.get(),v0.get());TPOST();
    TPRE();XR_ASSERT_DEBUG_LT_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_TEST_LT_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_DEBUG_LT_M(v1.get(),v0.get(),test);TPOST();TPRE();XR_ASSERT_TEST_LT_M(v1.get(),v0.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_LE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_TEST_LE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_DEBUG_LE_M(v1.get(),v0.get(),test);TPOST();TPRE();XR_ASSERT_TEST_LE_M(v1.get(),v0.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_GT_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_TEST_GT_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_DEBUG_GT_M(v1.get(),v0.get(),test);TPOST();TPRE();XR_ASSERT_TEST_GT_M(v1.get(),v0.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_GE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_TEST_GE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_ASSERT_DEBUG_GE_M(v1.get(),v0.get(),test);TPOST();TPRE();XR_ASSERT_TEST_GE_M(v1.get(),v0.get(),test);TPOST();
    TPRE();XR_ASSERT_DEBUG_LT_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_LT_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_DEBUG_LT_FM(v1.get(),v0.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_LT_FM(v1.get(),v0.get(),format,1,1);TPOST();
    TPRE();XR_ASSERT_DEBUG_LE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_LE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_DEBUG_LE_FM(v1.get(),v0.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_LE_FM(v1.get(),v0.get(),format,1,1);TPOST();
    TPRE();XR_ASSERT_DEBUG_GT_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_GT_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_DEBUG_GT_FM(v1.get(),v0.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_GT_FM(v1.get(),v0.get(),format,1,1);TPOST();
    TPRE();XR_ASSERT_DEBUG_GE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_GE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_ASSERT_DEBUG_GE_FM(v1.get(),v0.get(),format,1,1);TPOST();TPRE();XR_ASSERT_TEST_GE_FM(v1.get(),v0.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_LT(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_TEST_LT(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_DEBUG_LT(v1.get(),v0.get());TPOST();TPRE();XR_EXPECT_TEST_LT(v1.get(),v0.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_LE(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_TEST_LE(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_DEBUG_LE(v1.get(),v0.get());TPOST();TPRE();XR_EXPECT_TEST_LE(v1.get(),v0.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_GT(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_TEST_GT(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_DEBUG_GT(v1.get(),v0.get());TPOST();TPRE();XR_EXPECT_TEST_GT(v1.get(),v0.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_GE(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_TEST_GE(v0.get(),v1.get());TPOST();TPRE();XR_EXPECT_DEBUG_GE(v1.get(),v0.get());TPOST();TPRE();XR_EXPECT_TEST_GE(v1.get(),v0.get());TPOST();
    TPRE();XR_EXPECT_DEBUG_LT_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_TEST_LT_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_DEBUG_LT_M(v1.get(),v0.get(),test);TPOST();TPRE();XR_EXPECT_TEST_LT_M(v1.get(),v0.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_LE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_TEST_LE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_DEBUG_LE_M(v1.get(),v0.get(),test);TPOST();TPRE();XR_EXPECT_TEST_LE_M(v1.get(),v0.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_GT_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_TEST_GT_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_DEBUG_GT_M(v1.get(),v0.get(),test);TPOST();TPRE();XR_EXPECT_TEST_GT_M(v1.get(),v0.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_GE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_TEST_GE_M(v0.get(),v1.get(),test);TPOST();TPRE();XR_EXPECT_DEBUG_GE_M(v1.get(),v0.get(),test);TPOST();TPRE();XR_EXPECT_TEST_GE_M(v1.get(),v0.get(),test);TPOST();
    TPRE();XR_EXPECT_DEBUG_LT_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_LT_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_DEBUG_LT_FM(v1.get(),v0.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_LT_FM(v1.get(),v0.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_LE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_LE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_DEBUG_LE_FM(v1.get(),v0.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_LE_FM(v1.get(),v0.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_GT_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_GT_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_DEBUG_GT_FM(v1.get(),v0.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_GT_FM(v1.get(),v0.get(),format,1,1);TPOST();
    TPRE();XR_EXPECT_DEBUG_GE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_GE_FM(v0.get(),v1.get(),format,1,1);TPOST();TPRE();XR_EXPECT_DEBUG_GE_FM(v1.get(),v0.get(),format,1,1);TPOST();TPRE();XR_EXPECT_TEST_GE_FM(v1.get(),v0.get(),format,1,1);TPOST();

    // Nothing to test on these, they don't take an expression
    //XR_DEBUG_UNEXPECTED();XR_TEST_UNEXPECTED();
    //XR_DEBUG_UNEXPECTED_M(test);XR_TEST_UNEXPECTED_M(test);
    //XR_DEBUG_UNEXPECTED_FM(format,1,1);XR_TEST_UNEXPECTED_FM(format,1,1);
    //XR_DEBUG_FAIL();XR_TEST_FAIL();
    //XR_DEBUG_FAIL_M(test);XR_TEST_FAIL_M(test);
    //XR_DEBUG_FAIL_FM(format,1,1);XR_TEST_FAIL_FM(format,1,1);

#undef TPRE
#undef TPOST

}

#if defined(XR_COMPILER_MICROSOFT)
// --------------------------------------------------------------------------------------  FUNCTION
// Make sure that, given no other instruction the system will break.
// This uses ACTION BREAK, which will only cause a failure on msvc, which uses
// exception handling. Other systems do not and explicitly call longjmp.
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( AssertActionResponseTestDefaultBreak )
{
    AssertFiredCounter test;
    AssertHandlerRestorer testHelper(test);
    test.SetResponse(xr::Core::kActionBreak);

    // This will stop the debugger, so skip it if you're running from debugger.
    if(!::xr::Core::IsDebuggerConnected())
    {
        // This should fail!
       ::xr::Core::Test::ExpectCurrentTestFail(this);
        XR_ASSERT_ALWAYS_EQ(1,2); // SHOULD FAIL RIGHT HERE!
        fprintf(stderr, "ERROR: Assert does not work! Other test results may not be trustable." XR_EOL);
        fflush(stderr);
        XR_HALT();
    }
}
#endif


XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
