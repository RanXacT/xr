// ######################################################################################### - FILE
/*! \file
 \brief Provides an integrated Unit Testing framework. See Sample usage in
 detailed description.

 \page coretest Testing System Overview

 \section Introduction
 This is a simple unit testing framework. It is built to be easy to use and
 extensible. It does not have any unit test specific assert like logic. It
 leverages the system assert framework. Note that both "Assert" and "Expect"
 defines will cause the running test to fail.

 \section Alternatives
 There are many unit test frameworks for C++. Many are not cross platform. Others
 are GPL or LGPL. The best candidate was google test, which is cross platform
 and new BSD licensed. but that was not chosen for the following reasons:

 \li It is not very mature on windows as far as threading is concerned.
 \li It uses it's own custom test defines (where xr uses the assert logic).
     This would require modification.
 \li It uses exceptions exclusively, where I prefer to compile sans exceptions.

\par Sample Usage

Creating Tests:

\code
XR_UNITTEST_GROUP_BEGIN( GroupName )

XR_UNITTEST_TEST_FUNC( TestName )
{
    int32_t MyVariable;

    XR_ASSERT_TEST_EQ(MyVariable, 0);
}

XR_UNITTEST_GROUP_END()
\endcode

Running tests:
\code
if(xr::Core::Test::RunAll() != xr::Core::kSuccess)
{
    fprintf(stderr, "Test Failure" XR_EOL);
    retVal = -1;
}
\endcode


\section implementation notes
There is a distinct line in the architecture. The groups / functions hierarchy is
self sufficient (and must be so to maintain the static relationships). The logic
for running them is in the manager which can be subclassed to modify how things
are run, but again the existing group hierarchy cannot be overridden.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_TEST_H
#define XR_CORE_TEST_H

#if defined( _MSC_VER )
#pragma once
#endif

#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

// For va_list
#include <stdarg.h>

#if !XR_COMPILER_MICROSOFT
#include <setjmp.h>  // Don't impose exception handling on calling code. Use setjmp instead.
#endif

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/// \internal
#define XR_UNITTEST_GROUP_REFERENCE(name)                                     \
    {                                                                         \
        extern xr::Core::Test::TestGroup XR_UNITTEST_GROUP_NAME(name);              \
        const void * volatile unusedvar__ = XR_UNITTEST_GROUP_NAME(name).GetName(); \
        XR_UNUSED(unusedvar__);                                               \
    }

#define XR_UNITTEST_GROUP_NAME(name)     XR_PREPROCESSOR_JOIN(UnitTestGroup_,name)
#define XR_UNITTEST_TOKEN_OBJ(name)   XR_PREPROCESSOR_JOIN(UnitTestTokenType_,name)
#define XR_UNITTEST_TOKEN_NAME(name)  XR_PREPROCESSOR_JOIN(UnitTestTokenInstance_,name)

// -----------------------------------------------------------------------------------------  MACRO
// It is necessary to disable this warning in tests as they commonly
// have expressions that are constant.
//  4127: expressions that are constant
// -----------------------------------------------------------------------------------------  MACRO
#if defined(XR_COMPILER_MICROSOFT)
#define XR_TEST_GROUP_PREFIX  __pragma(warning(push)) __pragma(warning(disable : 4127))
#define XR_TEST_GROUP_POSTFIX __pragma(warning(pop))
#else
#define XR_TEST_GROUP_PREFIX
#define XR_TEST_GROUP_POSTFIX
#endif
//@}

// -----------------------------------------------------------------------------------------  MACRO
/*! \brief This begins a unit test group
 A unit test group is segregated from other code by an anonymous namespace
 (to prevent name collisions from other files) and a named namespace (to
 prevent name collisions from the same file).
 \param name This is the name of the group to be displayed if the a test fails.
 */
// -----------------------------------------------------------------------------------------  MACRO
#define XR_UNITTEST_GROUP_BEGIN_EX(parent, name)                               \
  XR_TEST_GROUP_PREFIX                                                         \
  /* Create an instance of TestGroup with this name (in global scope) */       \
  ::xr::Core::Test::TestGroup XR_UNITTEST_GROUP_NAME(name) (&XR_UNITTEST_GROUP_NAME(parent), #name);   \
  /* Anonymous namespace prevents name collisions in tests */                  \
  namespace                                                                    \
  {                                                                            \
    /* Create a local helper function within this namespace. */                \
    /* Without the anonymous namespace, this would be a duplicated symbol.*/   \
    ::xr::Core::Test::TestGroup * _CurrentTestGroup()                          \
    {                                                                          \
        return &XR_UNITTEST_GROUP_NAME(name);                                  \
    }

// -----------------------------------------------------------------------------------------  MACRO
/// \brief This is the normal means of defining tests.
// -----------------------------------------------------------------------------------------  MACRO
#define XR_UNITTEST_GROUP_BEGIN(name) XR_UNITTEST_GROUP_BEGIN_EX(Global,name)
// -----------------------------------------------------------------------------------------  MACRO
/*! \brief This Ends a unit test group
 Essentially it closes off the anonymous namespace, and pops any warning
 changes.
 */
// -----------------------------------------------------------------------------------------  MACRO
#define XR_UNITTEST_GROUP_END() } XR_TEST_GROUP_POSTFIX

// -----------------------------------------------------------------------------------------  MACRO
/*! \brief This defines a unit test function
 Unit test functions are run linearly as defined within a group, but no set
 order is guaranteed between groups.
*/
// -----------------------------------------------------------------------------------------  MACRO
#define XR_UNITTEST_TEST_FUNC(name)                                           \
  class XR_UNITTEST_TOKEN_OBJ(name): public ::xr::Core::Test::TestFunction    \
  {                                                                           \
  public:                                                                     \
    XR_UNITTEST_TOKEN_OBJ(name)(): ::xr::Core::Test::TestFunction(#name)      \
    {                                                                         \
      ::xr::Core::Test::TestGroup::RegisterToken(_CurrentTestGroup(), this);  \
    }                                                                         \
    void Run( );                                                              \
  };                                                                          \
  XR_UNITTEST_TOKEN_OBJ(name) XR_UNITTEST_TOKEN_NAME(name);                   \
  void XR_UNITTEST_TOKEN_OBJ(name)::Run()


// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/*! Class exists solely to group the functions together. All functions are
    static. Logic for which tests to run etc, is left to calling code.
*/
// ***************************************************************************************** - TYPE
class Test{
public:

    /************************************************************************************-SUBTYPE*/
    /*! Type of token in the test list.
        Only kTokenFunction is currently implemented. Others are
        planned, but not needed yet.
        */
    /************************************************************************************-SUBTYPE*/
    enum TokenType{
        kTokenFunction,
        kTokenSetup,
        kTokenTeardown
    };
    /************************************************************************************-SUBTYPE*/
    /*! \brief Token in the test list, must get type and cast to use tokens.
        */
    /************************************************************************************-SUBTYPE*/
    class XR_PURE_INTERFACE TestToken
    {
    public:
        /*! Standard constructor                                             */
        TestToken(const char * name, TokenType eType): mName(name), mNext(nullptr), mType(eType) {}
        virtual ~TestToken() {}

        //@{
        /// \internal
        XR_INLINE const char *       GetTokenName()           { return mName; }
        XR_INLINE TokenType          GetTokenType()           { return mType; }
        XR_INLINE TestToken *        GetNextToken()           { return mNext; }
        XR_INLINE void               SetNextToken(TestToken * next)  { mNext = next; }
        //@}
    private:
        //@{
        /// \internal
        const char *   mName;
        TestToken  *   mNext;
        TokenType      mType;
        //@}
    };

    /************************************************************************************-SUBTYPE*/
    /*! \brief class to wrap a TestFunction entry in a test groups token list.
    */
    /************************************************************************************-SUBTYPE*/
    class XR_PURE_INTERFACE TestFunction : public TestToken
    {
    public:
        /*! Sole User implemented function                                   */
        virtual void Run() = 0;
        /*! Standard constructor                                             */
        TestFunction(const char * name): TestToken(name, kTokenFunction) {}
        virtual ~TestFunction() {}
    private:
    };
    /************************************************************************************-SUBTYPE*/
    /*! \brief Class to wrap the concept of a test group
        internally maintains a head pointer to a list of items to execute
        \internal
        */
    /************************************************************************************-SUBTYPE*/
    class TestGroup
    {
    public:
        //@{
        /*! \internal */
        XR_INLINE TestGroup(TestGroup * parentGroup, const char * name):
            mName(name)
/*          mFirstToken(nullptr),
            mLastToken(nullptr),
            mFirstChild(nullptr),
            mLastChild(nullptr),
            mNext(nullptr),
*/
        {
            RegisterGroup(parentGroup, this);
        }

        ~TestGroup() {}

        static void RegisterToken(TestGroup * testGroup, TestToken * testItem);
        static void RegisterGroup(TestGroup * parentGroup, TestGroup * childGroup);
        XR_INLINE const char   * GetName()                 { return mName; }
        XR_INLINE TestToken    * GetTokenChainHead()     { return mFirstToken; }
        XR_INLINE TestGroup    * GetChildGroupChainHead()   { return mFirstChild; }
        XR_INLINE TestGroup    * GetNextGroup()             { return mNext; }
        XR_INLINE void           SetNextGroup(TestGroup * p){ mNext = p; }
    private:
        const char * mName;        ///< Group Name
        TestToken * mFirstToken;   ///< These are the tokens (mainly functions)
        TestToken * mLastToken;    ///< End of List
        TestGroup * mFirstChild;   ///< These are child groups (each can have their own tokens)
        TestGroup * mLastChild;    ///< End of List
        TestGroup  * mNext;        ///< This is the next Sibling in the group list
    };

    // ------------------------------------------------------------------------------------  MEMBER
    /*! This function will run all registered test                       */
    // ------------------------------------------------------------------------------------  MEMBER
    static StandardResult  RunAll();
    // ------------------------------------------------------------------------------------  MEMBER
    /*! This will return true if we are currently running tests          */
    // ------------------------------------------------------------------------------------  MEMBER
    static bool            IsTestRunning() ;
    // ------------------------------------------------------------------------------------  MEMBER
    /*! This function will run all tests marked with the passed group name.
        group names are set by calling XR_UNITTEST_GROUP_BEGIN_EX instead of
        XR_UNITTEST_GROUP_BEGIN. Tests defined with XR_UNITTEST_GROUP_BEGIN
        are in the "XR_UNITTEST_GROUP_NAME(Global)" group.
        \see XR_UNITTEST_GROUP_BEGIN_EX
    */
    // ------------------------------------------------------------------------------------  MEMBER
    static StandardResult  RunGroup(const char * groupName, const char * testNameOrNull);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Causes the current test to fail. This should not typically be called directly
        it also only exists (obviously) in test enabled builds. Tests should in general
        use the system assert logic to trigger test failures.
        */
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_RETURN static void            FailCurrentTest(const char * filename, int32_t lineNumber, const char * description);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Causes the current test to fail. This should not typically be called directly
        it also only exists (obviously) in test enabled builds. Tests should in general
        use the system assert logic to trigger test failures.
        */
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_RETURN static void            FailCurrentTestFormatted(const char * filename, int32_t lineNumber, const char * description, ...);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Causes the current test to fail. This should not typically be called directly
        it also only exists (obviously) in test enabled builds. Tests should in general
        use the system assert logic to trigger test failures.
        */
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_RETURN static void            FailCurrentTestVFormatted(const char * filename, int32_t lineNumber, const char * description, va_list args );
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    /*! For testing purposes, it's necessary to have a way to force a failure.
        If the test that causes this passes, then that is bad.
        \internal
        */
    // ------------------------------------------------------------------------------------  MEMBER
    static void            ExpectCurrentTestFail(TestFunction * testToFail);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Some static allocations are lazy and cannot occur up front
        these are intentionally rare, but can be difficult to resolve
        cleanly. In these cases, you can set an expectation that a given
        test will leak a certain number of allocations. This should only
        be used with specific knowledge of what these allocations are!!!
        */
    // ------------------------------------------------------------------------------------  MEMBER
    static void            ExpectCurrentTestLeak(size_t numAllocations);
    // ------------------------------------------------------------------------------------  MEMBER
    /*! returns the root test node. Used by higher level code for complex
        test usage
        */
    // ------------------------------------------------------------------------------------  MEMBER
    static TestGroup * GetRootTestGroup();
private:
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    static StandardResult RunInternal(TestGroup * pGroup, const char * testNameOrNull);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    static          TestToken *sExpectTestFail;   ///< Test function expected to break (or nullptr)
    static          size_t     sExpectTestLeaks;  ///< Test function expected to leak this number of allocations
    static volatile bool       sTestsAreRunning;  ///< True if a test is currently running

#if !defined(XR_COMPILER_MICROSOFT)
    static jmp_buf             sExceptionReturnPoint;
#endif
}; // End systemTest
}} // namespace

// ######################################################################################### - FILE
/*! Root Test group (intentionally in global scope). Use "GetRootNode"
 \internal.
*/
// ######################################################################################### - FILE
extern ::xr::Core::Test::TestGroup XR_UNITTEST_GROUP_NAME(Global);

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
#endif //#ifndef XR_CORE_TEST_H
