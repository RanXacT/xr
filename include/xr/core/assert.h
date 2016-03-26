// ######################################################################################### - FILE
/*! \file
    Provides a simple assert system. Assert Failed callback can be overridden
    at any time to provide more detailed functionality. Default functionality
    is to print to stderr and then exit the process on any assert.

    \par
    These functions have no state and are always available. Although an assert
    fired prior to having a higher level filtration system being setup will
    not be skip-able.

    \par
    Architecturally, this purely provides macros for assertion and a callback
    for "Assertion Failed" events. The "services" library provides a set of
    callbacks that implement assert skip / advanced handling mechanisms,
    although even that must be extended to provide a graphical assert system.

 \page coreassert Assert System Overview


 \section Intro Introduction
    The Assert system design is quite traditional from a client code
    perspective. Use the provided macros, that is all. As for setup /
    configuration, the default behavior is to print info and break at
    all asserts.

    \code
    Exrmples:
    XR_ASSERT_ALWAYS_EQ( shouldBeZero, 0);
    XR_ASSERT_ALWAYS_LT_M( sholdBeLessThan, 4, "Hey, there are too many entries!");
    XR_ASSERT_ALWAYS_TRUE_FM( h.Succeeded() , "Oh, no the call failed %"XR_INTPTR_PRINTX"!", h.GetErrorCode());
    \endcode


  \section Macros Macros and description
  Assertions / Expectations in all builds, these can be useful for logging /
  reporting field issues. Used to validate assumptions within code. Failures
  are passed to registered IAssertHandler objects

  \li Assert: Implies that the code will not function properly if the condition
          is not met. These are typically fatal (although an option may be
          provided to skip them).
  \li Expect: Imply the code was intended for other purposes, but that it
          should in theory continue to continue to operate. Such operation may
          involve minor glitches or performance problems but should not be
          expected to crash / corrupt memory.

  \par Build inclusion Suffixes:
   \li TEST:   Only included if XR_TEST_FEATURES_ENABLED
   \li DEBUG:  Only included if XR_DEBUG_FEATURES_ENABLED
   \li ALWAYS: Included in all builds

  \par Operation Suffixes:
   \li *_TRUE:  Assumes single expression should be true.
   \li *_FALSE: Assumes single expression should be false.
   \li *_EQ:    Assumes passed expressions should be equal.
   \li *_NE:    Assumes passed expressions should not be equal.
   \li *_LT:    Assumes the first expression is less than the second.
   \li *_LE:    Assumes the first expression is less than or equal to the second.
   \li *_GT:    Assumes the first expression is greater than the second.
   \li *_GE:    Assumes the first expression is greater than or equal to the second.

  \par Message Suffixes:
    \li _M:  Const char * message passed to be logged with the error.
    \li _FM: printf style formatted message to be assembled and logged with the error.

  \par putting it all together
    XR_[ASSERT|EXPECT]_[BUILD]_[CONDITION](_[MESSAGE]) Exrmples:
    \li XR_ASSERT_ALWAYS_FALSE: with "ASSERT" "ALWAYS" (in all builds) that expression should be FALSE

 \par Full assertion Macro list
<table>
<tr> <td>FALSE</td><td>#XR_ASSERT_ALWAYS_FALSE        </td><td>#XR_ASSERT_DEBUG_FALSE     </td><td>#XR_ASSERT_TEST_FALSE    </td></tr>
<tr> <td>TRUE </td><td>#XR_ASSERT_ALWAYS_TRUE         </td><td>#XR_ASSERT_DEBUG_TRUE      </td><td>#XR_ASSERT_TEST_TRUE</td></tr>
<tr> <td>EQ   </td><td>#XR_ASSERT_ALWAYS_EQ           </td><td>#XR_ASSERT_DEBUG_EQ        </td><td>#XR_ASSERT_TEST_EQ</td></tr>
<tr> <td>NE   </td><td>#XR_ASSERT_ALWAYS_NE           </td><td>#XR_ASSERT_DEBUG_NE        </td><td>#XR_ASSERT_TEST_NE</td></tr>
<tr> <td>LT   </td><td>#XR_ASSERT_ALWAYS_LT           </td><td>#XR_ASSERT_DEBUG_LT        </td><td>#XR_ASSERT_TEST_LT</td></tr>
<tr> <td>LE   </td><td>#XR_ASSERT_ALWAYS_LE           </td><td>#XR_ASSERT_DEBUG_LE        </td><td>#XR_ASSERT_TEST_LE</td></tr>
<tr> <td>GT   </td><td>#XR_ASSERT_ALWAYS_GT           </td><td>#XR_ASSERT_DEBUG_GT        </td><td>#XR_ASSERT_TEST_GT</td></tr>
<tr> <td>GE   </td><td>#XR_ASSERT_ALWAYS_GE           </td><td>#XR_ASSERT_DEBUG_GE        </td><td>#XR_ASSERT_TEST_GE</td></tr>
<tr> <td>FALSE</td><td>#XR_ASSERT_ALWAYS_FALSE_M      </td><td>#XR_ASSERT_DEBUG_FALSE_M   </td><td>#XR_ASSERT_TEST_FALSE_M</td></tr>
<tr> <td>TRUE </td><td>#XR_ASSERT_ALWAYS_TRUE_M       </td><td>#XR_ASSERT_DEBUG_TRUE_M    </td><td>#XR_ASSERT_TEST_TRUE_M</td></tr>
<tr> <td>EQ   </td><td>#XR_ASSERT_ALWAYS_EQ_M         </td><td>#XR_ASSERT_DEBUG_EQ_M      </td><td>#XR_ASSERT_TEST_EQ_M</td></tr>
<tr> <td>NE   </td><td>#XR_ASSERT_ALWAYS_NE_M         </td><td>#XR_ASSERT_DEBUG_NE_M      </td><td>#XR_ASSERT_TEST_NE_M</td></tr>
<tr> <td>LT   </td><td>#XR_ASSERT_ALWAYS_LT_M         </td><td>#XR_ASSERT_DEBUG_LT_M      </td><td>#XR_ASSERT_TEST_LT_M</td></tr>
<tr> <td>LE   </td><td>#XR_ASSERT_ALWAYS_LE_M         </td><td>#XR_ASSERT_DEBUG_LE_M      </td><td>#XR_ASSERT_TEST_LE_M</td></tr>
<tr> <td>GT   </td><td>#XR_ASSERT_ALWAYS_GT_M         </td><td>#XR_ASSERT_DEBUG_GT_M      </td><td>#XR_ASSERT_TEST_GT_M</td></tr>
<tr> <td>GE   </td><td>#XR_ASSERT_ALWAYS_GE_M         </td><td>#XR_ASSERT_DEBUG_GE_M      </td><td>#XR_ASSERT_TEST_GE_M</td></tr>
<tr> <td>FALSE</td><td>#XR_ASSERT_ALWAYS_FALSE_FM     </td><td>#XR_ASSERT_DEBUG_FALSE_FM  </td><td>#XR_ASSERT_TEST_FALSE_FM</td></tr>
<tr> <td>TRUE </td><td>#XR_ASSERT_ALWAYS_TRUE_FM      </td><td>#XR_ASSERT_DEBUG_TRUE_FM   </td><td>#XR_ASSERT_TEST_TRUE_FM</td></tr>
<tr> <td>EQ   </td><td>#XR_ASSERT_ALWAYS_EQ_FM        </td><td>#XR_ASSERT_DEBUG_EQ_FM     </td><td>#XR_ASSERT_TEST_EQ_FM</td></tr>
<tr> <td>NE   </td><td>#XR_ASSERT_ALWAYS_NE_FM        </td><td>#XR_ASSERT_DEBUG_NE_FM     </td><td>#XR_ASSERT_TEST_NE_FM</td></tr>
<tr> <td>LT   </td><td>#XR_ASSERT_ALWAYS_LT_FM        </td><td>#XR_ASSERT_DEBUG_LT_FM     </td><td>#XR_ASSERT_TEST_LT_FM</td></tr>
<tr> <td>LE   </td><td>#XR_ASSERT_ALWAYS_LE_FM        </td><td>#XR_ASSERT_DEBUG_LE_FM     </td><td>#XR_ASSERT_TEST_LE_FM</td></tr>
<tr> <td>GT   </td><td>#XR_ASSERT_ALWAYS_GT_FM        </td><td>#XR_ASSERT_DEBUG_GT_FM     </td><td>#XR_ASSERT_TEST_GT_FM</td></tr>
<tr> <td>GE   </td><td>#XR_ASSERT_ALWAYS_GE_FM        </td><td>#XR_ASSERT_DEBUG_GE_FM     </td><td>#XR_ASSERT_TEST_GE_FM</td></tr>
<tr> <td>FALSE</td><td>#XR_EXPECT_ALWAYS_FALSE        </td><td>#XR_EXPECT_DEBUG_FALSE     </td><td>#XR_EXPECT_TEST_FALSE</td></tr>
<tr> <td>TRUE </td><td>#XR_EXPECT_ALWAYS_TRUE         </td><td>#XR_EXPECT_DEBUG_TRUE      </td><td>#XR_EXPECT_TEST_TRUE</td></tr>
<tr> <td>EQ   </td><td>#XR_EXPECT_ALWAYS_EQ           </td><td>#XR_EXPECT_DEBUG_EQ        </td><td>#XR_EXPECT_TEST_EQ</td></tr>
<tr> <td>NE   </td><td>#XR_EXPECT_ALWAYS_NE           </td><td>#XR_EXPECT_DEBUG_NE        </td><td>#XR_EXPECT_TEST_NE</td></tr>
<tr> <td>LT   </td><td>#XR_EXPECT_ALWAYS_LT           </td><td>#XR_EXPECT_DEBUG_LT        </td><td>#XR_EXPECT_TEST_LT</td></tr>
<tr> <td>LE   </td><td>#XR_EXPECT_ALWAYS_LE           </td><td>#XR_EXPECT_DEBUG_LE        </td><td>#XR_EXPECT_TEST_LE</td></tr>
<tr> <td>GT   </td><td>#XR_EXPECT_ALWAYS_GT           </td><td>#XR_EXPECT_DEBUG_GT        </td><td>#XR_EXPECT_TEST_GT</td></tr>
<tr> <td>GE   </td><td>#XR_EXPECT_ALWAYS_GE           </td><td>#XR_EXPECT_DEBUG_GE        </td><td>#XR_EXPECT_TEST_GE</td></tr>
<tr> <td>FALSE</td><td>#XR_EXPECT_ALWAYS_FALSE_M      </td><td>#XR_EXPECT_DEBUG_FALSE_M   </td><td>#XR_EXPECT_TEST_FALSE_M</td></tr>
<tr> <td>TRUE </td><td>#XR_EXPECT_ALWAYS_TRUE_M       </td><td>#XR_EXPECT_DEBUG_TRUE_M    </td><td>#XR_EXPECT_TEST_TRUE_M</td></tr>
<tr> <td>EQ   </td><td>#XR_EXPECT_ALWAYS_EQ_M         </td><td>#XR_EXPECT_DEBUG_EQ_M      </td><td>#XR_EXPECT_TEST_EQ_M</td></tr>
<tr> <td>NE   </td><td>#XR_EXPECT_ALWAYS_NE_M         </td><td>#XR_EXPECT_DEBUG_NE_M      </td><td>#XR_EXPECT_TEST_NE_M</td></tr>
<tr> <td>LT   </td><td>#XR_EXPECT_ALWAYS_LT_M         </td><td>#XR_EXPECT_DEBUG_LT_M      </td><td>#XR_EXPECT_TEST_LT_M</td></tr>
<tr> <td>LE   </td><td>#XR_EXPECT_ALWAYS_LE_M         </td><td>#XR_EXPECT_DEBUG_LE_M      </td><td>#XR_EXPECT_TEST_LE_M</td></tr>
<tr> <td>GT   </td><td>#XR_EXPECT_ALWAYS_GT_M         </td><td>#XR_EXPECT_DEBUG_GT_M      </td><td>#XR_EXPECT_TEST_GT_M</td></tr>
<tr> <td>GE   </td><td>#XR_EXPECT_ALWAYS_GE_M         </td><td>#XR_EXPECT_DEBUG_GE_M      </td><td>#XR_EXPECT_TEST_GE_M</td></tr>
<tr> <td>FALSE</td><td>#XR_EXPECT_ALWAYS_FALSE_FM     </td><td>#XR_EXPECT_DEBUG_FALSE_FM  </td><td>#XR_EXPECT_TEST_FALSE_FM</td></tr>
<tr> <td>TRUE </td><td>#XR_EXPECT_ALWAYS_TRUE_FM      </td><td>#XR_EXPECT_DEBUG_TRUE_FM   </td><td>#XR_EXPECT_TEST_TRUE_FM</td></tr>
<tr> <td>EQ   </td><td>#XR_EXPECT_ALWAYS_EQ_FM        </td><td>#XR_EXPECT_DEBUG_EQ_FM     </td><td>#XR_EXPECT_TEST_EQ_FM</td></tr>
<tr> <td>NE   </td><td>#XR_EXPECT_ALWAYS_NE_FM        </td><td>#XR_EXPECT_DEBUG_NE_FM     </td><td>#XR_EXPECT_TEST_NE_FM</td></tr>
<tr> <td>LT   </td><td>#XR_EXPECT_ALWAYS_LT_FM        </td><td>#XR_EXPECT_DEBUG_LT_FM     </td><td>#XR_EXPECT_TEST_LT_FM</td></tr>
<tr> <td>LE   </td><td>#XR_EXPECT_ALWAYS_LE_FM        </td><td>#XR_EXPECT_DEBUG_LE_FM     </td><td>#XR_EXPECT_TEST_LE_FM</td></tr>
<tr> <td>GT   </td><td>#XR_EXPECT_ALWAYS_GT_FM        </td><td>#XR_EXPECT_DEBUG_GT_FM     </td><td>#XR_EXPECT_TEST_GT_FM</td></tr>
<tr> <td>GE   </td><td>#XR_EXPECT_ALWAYS_GE_FM        </td><td>#XR_EXPECT_DEBUG_GE_FM     </td><td>#XR_EXPECT_TEST_GE_FM</td></tr>
<tr> <td></td><td>#XR_ALWAYS_UNEXPECTED          </td><td>#XR_DEBUG_UNEXPECTED       </td><td>#XR_TEST_UNEXPECTED</td></tr>
<tr> <td></td><td>#XR_ALWAYS_UNEXPECTED_M        </td><td>#XR_DEBUG_UNEXPECTED_M     </td><td>#XR_TEST_UNEXPECTED_M</td></tr>
<tr> <td></td><td>#XR_ALWAYS_UNEXPECTED_FM       </td><td>#XR_DEBUG_UNEXPECTED_FM    </td><td>#XR_TEST_UNEXPECTED_FM</td></tr>
<tr> <td></td><td>#XR_ALWAYS_FAIL                </td><td>#XR_DEBUG_FAIL             </td><td>#XR_TEST_FAIL</td></tr>
<tr> <td></td><td>#XR_ALWAYS_FAIL_M              </td><td>#XR_DEBUG_FAIL_M           </td><td>#XR_TEST_FAIL_M</td></tr>
<tr> <td></td><td>#XR_ALWAYS_FAIL_FM             </td><td>#XR_DEBUG_FAIL_FM          </td><td>#XR_TEST_FAIL_FM</td></tr>
</table>


\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_ASSERT_H
#define XR_CORE_ASSERT_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

#ifndef XR_CORE_UNKNOWN_H
#include "xr/core/unknown.h"
#endif

// For va_list
#include <stdarg.h>

#if defined(XR_COMPILER_MICROSOFT)
// For printf annotation
#include <sal.h>
#endif

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// -----------------------------------------------------------------------------------------  MACRO
// Helpers:
// -----------------------------------------------------------------------------------------  MACRO

/// It is necessary to disable this warning in asserts as they commonly
/// have expressions that are constant. (don't want to disable it wholly though)
//@{
#if defined(XR_COMPILER_MICROSOFT)
#define XR_ASSERT_PREFIX  __pragma(warning(push)) __pragma(warning(disable : 4127))
#define XR_ASSERT_POSTFIX __pragma(warning(pop))
#else
#define XR_ASSERT_PREFIX
#define XR_ASSERT_POSTFIX
#endif
//@}

/// Allows easy configuration of whether or not assert expressions are compiled
/// into the built executable. The messages could pose some security risk, but
/// even if not should be disabled to reduce the size of the final executable.
/// \internal
#if defined(XR_ASSERTS_PRINT_EXPRESSION)
#define XR_ASSERT_ARGS_INTERNAL(expr) XR_FILE_LINE, #expr
#else
#define XR_ASSERT_ARGS_INTERNAL(expr) XR_FILE_LINE
#endif

// -----------------------------------------------------------------------------------------  MACRO
// Assert Macro Definitions:
// -----------------------------------------------------------------------------------------  MACRO
#if !defined(XR_CODE_COVERAGE_BUILD) || defined(XR_ASSERT_INTERNAL_TESTS)
/// \copydoc coreassert
//@{
#define XR_ASSERT_ALWAYS_FALSE(expr)                 XR_ASSERT_PREFIX if( (expr)){ if(::xr::Core::Assert::GetFailHandler().AssertFailed( ::xr::Core::kCallAssert, XR_ASSERT_ARGS_INTERNAL(expr) ) == ::xr::Core::kActionBreak) {XR_HALT();}; } XR_ASSERT_POSTFIX
#define XR_ASSERT_ALWAYS_TRUE(expr)                  XR_ASSERT_ALWAYS_FALSE(!(expr))
#define XR_ASSERT_ALWAYS_FALSE_M(expr, message)      XR_ASSERT_PREFIX if( (expr)){  if(::xr::Core::Assert::GetFailHandler().AssertFailed( ::xr::Core::kCallAssert, XR_FILE_LINE, message ) == ::xr::Core::kActionBreak) {XR_HALT();}; } XR_ASSERT_POSTFIX
#define XR_ASSERT_ALWAYS_TRUE_M( expr, message)      XR_ASSERT_ALWAYS_FALSE_M(!(expr), message)
#define XR_ASSERT_ALWAYS_FALSE_FM(expr, ...)         XR_ASSERT_PREFIX if( (expr)){ if(::xr::Core::Assert::GetFailHandler().AssertFailedFormatted( ::xr::Core::kCallAssert, XR_FILE_LINE, __VA_ARGS__ ) == ::xr::Core::kActionBreak) {XR_HALT();}; } XR_ASSERT_POSTFIX
#define XR_ASSERT_ALWAYS_TRUE_FM( expr, ...)         XR_ASSERT_ALWAYS_FALSE_FM(!(expr), __VA_ARGS__)
#define XR_EXPECT_ALWAYS_FALSE(expr)                 XR_ASSERT_PREFIX if((expr)){ if(::xr::Core::Assert::GetFailHandler().AssertFailed( ::xr::Core::kCallExpect, XR_ASSERT_ARGS_INTERNAL(expr) ) == ::xr::Core::kActionBreak) {XR_HALT();}; } XR_ASSERT_POSTFIX
#define XR_EXPECT_ALWAYS_TRUE(expr)                  XR_EXPECT_ALWAYS_FALSE(!(expr))
#define XR_EXPECT_ALWAYS_FALSE_M(expr, message)      XR_ASSERT_PREFIX if( (expr)){ if(::xr::Core::Assert::GetFailHandler().AssertFailed( ::xr::Core::kCallExpect, XR_FILE_LINE, message ) == ::xr::Core::kActionBreak) {XR_HALT();}; } XR_ASSERT_POSTFIX
#define XR_EXPECT_ALWAYS_TRUE_M( expr, message)      XR_EXPECT_ALWAYS_FALSE_M(!(expr), message)
#define XR_EXPECT_ALWAYS_FALSE_FM(expr, ...)         XR_ASSERT_PREFIX if( (expr)){ if(::xr::Core::Assert::GetFailHandler().AssertFailedFormatted( ::xr::Core::kCallExpect, XR_FILE_LINE, __VA_ARGS__ ) == ::xr::Core::kActionBreak) {XR_HALT();}; } XR_ASSERT_POSTFIX
#define XR_EXPECT_ALWAYS_TRUE_FM( expr, ...)         XR_EXPECT_ALWAYS_FALSE_FM(!(expr), __VA_ARGS__)
#define XR_ALWAYS_UNEXPECTED()                       if(::xr::Core::Assert::GetFailHandler().AssertFailed( ::xr::Core::kCallExpect, XR_FILE_LINE ) == ::xr::Core::kActionBreak) {XR_HALT();}
#define XR_ALWAYS_UNEXPECTED_M(message)              if(::xr::Core::Assert::GetFailHandler().AssertFailed( ::xr::Core::kCallExpect, XR_FILE_LINE, message ) == ::xr::Core::kActionBreak) {XR_HALT();}
#define XR_ALWAYS_UNEXPECTED_FM(...)                 if(::xr::Core::Assert::GetFailHandler().AssertFailedFormatted( ::xr::Core::kCallExpect, XR_FILE_LINE, __VA_ARGS__ ) == ::xr::Core::kActionBreak) {XR_HALT();}
#define XR_ALWAYS_FAIL()                             if(::xr::Core::Assert::GetFailHandler().AssertFailed( ::xr::Core::kCallAssert, XR_FILE_LINE ) == ::xr::Core::kActionBreak) {XR_HALT();}
#define XR_ALWAYS_FAIL_M(message)                    if(::xr::Core::Assert::GetFailHandler().AssertFailed( ::xr::Core::kCallAssert, XR_FILE_LINE, message ) == ::xr::Core::kActionBreak) {XR_HALT();}
#define XR_ALWAYS_FAIL_FM(...)                       if(::xr::Core::Assert::GetFailHandler().AssertFailedFormatted( ::xr::Core::kCallAssert, XR_FILE_LINE, __VA_ARGS__ ) == ::xr::Core::kActionBreak) {XR_HALT();}

#define XR_ASSERT_ALWAYS_EQ(expr1, expr2)               do { XR_ASSERT_ALWAYS_TRUE( ((expr1) == (expr2))); XR_ANALYSIS_ASSUME(((expr1) == (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_NE(expr1, expr2)               do { XR_ASSERT_ALWAYS_TRUE( ((expr1) != (expr2))); XR_ANALYSIS_ASSUME(((expr1) != (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_LT(expr1, expr2)               do { XR_ASSERT_ALWAYS_TRUE( ((expr1) <  (expr2))); XR_ANALYSIS_ASSUME(((expr1) <  (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_LE(expr1, expr2)               do { XR_ASSERT_ALWAYS_TRUE( ((expr1) <= (expr2))); XR_ANALYSIS_ASSUME(((expr1) <= (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_GT(expr1, expr2)               do { XR_ASSERT_ALWAYS_TRUE( ((expr1) >  (expr2))); XR_ANALYSIS_ASSUME(((expr1) >  (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_GE(expr1, expr2)               do { XR_ASSERT_ALWAYS_TRUE( ((expr1) >= (expr2))); XR_ANALYSIS_ASSUME(((expr1) >= (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_EQ_M(expr1, expr2, message)    do { XR_ASSERT_ALWAYS_TRUE_M( ((expr1) == (expr2)) , message ); XR_ANALYSIS_ASSUME(((expr1) == (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_NE_M(expr1, expr2, message)    do { XR_ASSERT_ALWAYS_TRUE_M( ((expr1) != (expr2)) , message ); XR_ANALYSIS_ASSUME(((expr1) != (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_LT_M(expr1, expr2, message)    do { XR_ASSERT_ALWAYS_TRUE_M( ((expr1) <  (expr2)) , message ); XR_ANALYSIS_ASSUME(((expr1) <  (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_LE_M(expr1, expr2, message)    do { XR_ASSERT_ALWAYS_TRUE_M( ((expr1) <= (expr2)) , message ); XR_ANALYSIS_ASSUME(((expr1) <= (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_GT_M(expr1, expr2, message)    do { XR_ASSERT_ALWAYS_TRUE_M( ((expr1) >  (expr2)) , message ); XR_ANALYSIS_ASSUME(((expr1) >  (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_GE_M(expr1, expr2, message)    do { XR_ASSERT_ALWAYS_TRUE_M( ((expr1) >= (expr2)) , message ); XR_ANALYSIS_ASSUME(((expr1) >= (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_EQ_FM(expr1, expr2, ...)       do { XR_ASSERT_ALWAYS_TRUE_FM( ((expr1) == (expr2)) , __VA_ARGS__ ); XR_ANALYSIS_ASSUME(((expr1) == (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_NE_FM(expr1, expr2, ...)       do { XR_ASSERT_ALWAYS_TRUE_FM( ((expr1) != (expr2)) , __VA_ARGS__ ); XR_ANALYSIS_ASSUME(((expr1) != (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_LT_FM(expr1, expr2, ...)       do { XR_ASSERT_ALWAYS_TRUE_FM( ((expr1) <  (expr2)) , __VA_ARGS__ ); XR_ANALYSIS_ASSUME(((expr1) <  (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_LE_FM(expr1, expr2, ...)       do { XR_ASSERT_ALWAYS_TRUE_FM( ((expr1) <= (expr2)) , __VA_ARGS__ ); XR_ANALYSIS_ASSUME(((expr1) <= (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_GT_FM(expr1, expr2, ...)       do { XR_ASSERT_ALWAYS_TRUE_FM( ((expr1) >  (expr2)) , __VA_ARGS__ ); XR_ANALYSIS_ASSUME(((expr1) >  (expr2))); } while (false)
#define XR_ASSERT_ALWAYS_GE_FM(expr1, expr2, ...)       do { XR_ASSERT_ALWAYS_TRUE_FM( ((expr1) >= (expr2)) , __VA_ARGS__ ); XR_ANALYSIS_ASSUME(((expr1) >= (expr2))); } while (false)

#define XR_EXPECT_ALWAYS_EQ(expr1, expr2)    XR_EXPECT_ALWAYS_TRUE( ((expr1) == (expr2))  )
#define XR_EXPECT_ALWAYS_NE(expr1, expr2)    XR_EXPECT_ALWAYS_TRUE( ((expr1) != (expr2))  )
#define XR_EXPECT_ALWAYS_LT(expr1, expr2)    XR_EXPECT_ALWAYS_TRUE( ((expr1) <  (expr2))  )
#define XR_EXPECT_ALWAYS_LE(expr1, expr2)    XR_EXPECT_ALWAYS_TRUE( ((expr1) <= (expr2))  )
#define XR_EXPECT_ALWAYS_GT(expr1, expr2)    XR_EXPECT_ALWAYS_TRUE( ((expr1) >  (expr2))  )
#define XR_EXPECT_ALWAYS_GE(expr1, expr2)    XR_EXPECT_ALWAYS_TRUE( ((expr1) >= (expr2))  )
#define XR_EXPECT_ALWAYS_EQ_M(expr1, expr2, message)    XR_EXPECT_ALWAYS_TRUE_M( ((expr1) == (expr2)) , message )
#define XR_EXPECT_ALWAYS_NE_M(expr1, expr2, message)    XR_EXPECT_ALWAYS_TRUE_M( ((expr1) != (expr2)) , message )
#define XR_EXPECT_ALWAYS_LT_M(expr1, expr2, message)    XR_EXPECT_ALWAYS_TRUE_M( ((expr1) <  (expr2)) , message )
#define XR_EXPECT_ALWAYS_LE_M(expr1, expr2, message)    XR_EXPECT_ALWAYS_TRUE_M( ((expr1) <= (expr2)) , message )
#define XR_EXPECT_ALWAYS_GT_M(expr1, expr2, message)    XR_EXPECT_ALWAYS_TRUE_M( ((expr1) >  (expr2)) , message )
#define XR_EXPECT_ALWAYS_GE_M(expr1, expr2, message)    XR_EXPECT_ALWAYS_TRUE_M( ((expr1) >= (expr2)) , message )
#define XR_EXPECT_ALWAYS_EQ_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_TRUE_FM( ((expr1) == (expr2)) , __VA_ARGS__ )
#define XR_EXPECT_ALWAYS_NE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_TRUE_FM( ((expr1) != (expr2)) , __VA_ARGS__ )
#define XR_EXPECT_ALWAYS_LT_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_TRUE_FM( ((expr1) <  (expr2)) , __VA_ARGS__ )
#define XR_EXPECT_ALWAYS_LE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_TRUE_FM( ((expr1) <= (expr2)) , __VA_ARGS__ )
#define XR_EXPECT_ALWAYS_GT_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_TRUE_FM( ((expr1) >  (expr2)) , __VA_ARGS__ )
#define XR_EXPECT_ALWAYS_GE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_TRUE_FM( ((expr1) >= (expr2)) , __VA_ARGS__ )

//@}
#else  // (!XR_CODE_COVERAGE_BUILD) || XR_ASSERT_INTERNAL_TESTS
#define XR_ASSERT_ALWAYS_FALSE(expr)                         (expr)
#define XR_ASSERT_ALWAYS_TRUE(expr)                          (expr)
#define XR_ASSERT_ALWAYS_EQ(expr1, expr2)                    (expr1, expr2)
#define XR_ASSERT_ALWAYS_NE(expr1, expr2)                    (expr1, expr2)
#define XR_ASSERT_ALWAYS_LT(expr1, expr2)                    (expr1, expr2)
#define XR_ASSERT_ALWAYS_LE(expr1, expr2)                    (expr1, expr2)
#define XR_ASSERT_ALWAYS_GT(expr1, expr2)                    (expr1, expr2)
#define XR_ASSERT_ALWAYS_GE(expr1, expr2)                    (expr1, expr2)
#define XR_ASSERT_ALWAYS_FALSE_M(expr, message)              (expr)
#define XR_ASSERT_ALWAYS_TRUE_M( expr, message)              (expr)
#define XR_ASSERT_ALWAYS_EQ_M(expr1, expr2, message)         (expr1, expr2)
#define XR_ASSERT_ALWAYS_NE_M(expr1, expr2, message)         (expr1, expr2)
#define XR_ASSERT_ALWAYS_LT_M(expr1, expr2, message)         (expr1, expr2)
#define XR_ASSERT_ALWAYS_LE_M(expr1, expr2, message)         (expr1, expr2)
#define XR_ASSERT_ALWAYS_GT_M(expr1, expr2, message)         (expr1, expr2)
#define XR_ASSERT_ALWAYS_GE_M(expr1, expr2, message)         (expr1, expr2)
#define XR_ASSERT_ALWAYS_FALSE_FM(expr, ...)                 (expr)
#define XR_ASSERT_ALWAYS_TRUE_FM( expr, ...)                 (expr)
#define XR_ASSERT_ALWAYS_EQ_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_ASSERT_ALWAYS_NE_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_ASSERT_ALWAYS_LT_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_ASSERT_ALWAYS_LE_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_ASSERT_ALWAYS_GT_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_ASSERT_ALWAYS_GE_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_EXPECT_ALWAYS_FALSE(expr)                         (expr)
#define XR_EXPECT_ALWAYS_TRUE(expr)                          (expr)
#define XR_EXPECT_ALWAYS_EQ(expr1, expr2)                    (expr1, expr2)
#define XR_EXPECT_ALWAYS_NE(expr1, expr2)                    (expr1, expr2)
#define XR_EXPECT_ALWAYS_LT(expr1, expr2)                    (expr1, expr2)
#define XR_EXPECT_ALWAYS_LE(expr1, expr2)                    (expr1, expr2)
#define XR_EXPECT_ALWAYS_GT(expr1, expr2)                    (expr1, expr2)
#define XR_EXPECT_ALWAYS_GE(expr1, expr2)                    (expr1, expr2)
#define XR_EXPECT_ALWAYS_FALSE_M(expr, message)              (expr)
#define XR_EXPECT_ALWAYS_TRUE_M( expr, message)              (expr)
#define XR_EXPECT_ALWAYS_EQ_M(expr1, expr2, message)         (expr1, expr2)
#define XR_EXPECT_ALWAYS_NE_M(expr1, expr2, message)         (expr1, expr2)
#define XR_EXPECT_ALWAYS_LT_M(expr1, expr2, message)         (expr1, expr2)
#define XR_EXPECT_ALWAYS_LE_M(expr1, expr2, message)         (expr1, expr2)
#define XR_EXPECT_ALWAYS_GT_M(expr1, expr2, message)         (expr1, expr2)
#define XR_EXPECT_ALWAYS_GE_M(expr1, expr2, message)         (expr1, expr2)
#define XR_EXPECT_ALWAYS_FALSE_FM(expr, ...)                 (expr)
#define XR_EXPECT_ALWAYS_TRUE_FM( expr, ...)                 (expr)
#define XR_EXPECT_ALWAYS_EQ_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_EXPECT_ALWAYS_NE_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_EXPECT_ALWAYS_LT_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_EXPECT_ALWAYS_LE_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_EXPECT_ALWAYS_GT_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_EXPECT_ALWAYS_GE_FM(expr1, expr2, ...)            (expr1, expr2)
#define XR_ALWAYS_UNEXPECTED()                               ((void)0)
#define XR_ALWAYS_UNEXPECTED_M(message)                      ((void)0)
#define XR_ALWAYS_UNEXPECTED_FM(...)                         ((void)0)
#define XR_ALWAYS_FAIL()                                     ((void)0)
#define XR_ALWAYS_FAIL_M(message)                            ((void)0)
#define XR_ALWAYS_FAIL_FM(...)                               ((void)0)
#endif // (!XR_CODE_COVERAGE_BUILD) || XR_ASSERT_INTERNAL_TESTS
/// Assert logic only compiled in test or debug builds respectively.
/// To enable XR_DEBUG_* define XR_DEBUG_FEATURES_ENABLED
/// To enable XR_TEST_* define XR_TEST_FEATURES_ENABLED
/// \see XR_ASSERT_ALWAYS_TRUE
//@{
#if defined(XR_TEST_FEATURES_ENABLED) && (!defined(XR_CODE_COVERAGE_BUILD) || defined(XR_ASSERT_INTERNAL_TESTS))
#define XR_ASSERT_TEST_FALSE(expr)                 XR_ASSERT_ALWAYS_FALSE(expr)
#define XR_ASSERT_TEST_TRUE(expr)                  XR_ASSERT_ALWAYS_TRUE(expr)
#define XR_ASSERT_TEST_EQ(expr1, expr2)            XR_ASSERT_ALWAYS_EQ(expr1, expr2)
#define XR_ASSERT_TEST_NE(expr1, expr2)            XR_ASSERT_ALWAYS_NE(expr1, expr2)
#define XR_ASSERT_TEST_LT(expr1, expr2)            XR_ASSERT_ALWAYS_LT(expr1, expr2)
#define XR_ASSERT_TEST_LE(expr1, expr2)            XR_ASSERT_ALWAYS_LE(expr1, expr2)
#define XR_ASSERT_TEST_GT(expr1, expr2)            XR_ASSERT_ALWAYS_GT(expr1, expr2)
#define XR_ASSERT_TEST_GE(expr1, expr2)            XR_ASSERT_ALWAYS_GE(expr1, expr2)
#define XR_ASSERT_TEST_FALSE_M(expr, message)      XR_ASSERT_ALWAYS_FALSE_M(expr, message)
#define XR_ASSERT_TEST_TRUE_M( expr, message)      XR_ASSERT_ALWAYS_TRUE_M( expr, message)
#define XR_ASSERT_TEST_EQ_M(expr1, expr2, message) XR_ASSERT_ALWAYS_EQ_M(expr1, expr2, message)
#define XR_ASSERT_TEST_NE_M(expr1, expr2, message) XR_ASSERT_ALWAYS_NE_M(expr1, expr2, message)
#define XR_ASSERT_TEST_LT_M(expr1, expr2, message) XR_ASSERT_ALWAYS_LT_M(expr1, expr2, message)
#define XR_ASSERT_TEST_LE_M(expr1, expr2, message) XR_ASSERT_ALWAYS_LE_M(expr1, expr2, message)
#define XR_ASSERT_TEST_GT_M(expr1, expr2, message) XR_ASSERT_ALWAYS_GT_M(expr1, expr2, message)
#define XR_ASSERT_TEST_GE_M(expr1, expr2, message) XR_ASSERT_ALWAYS_GE_M(expr1, expr2, message)
#define XR_ASSERT_TEST_FALSE_FM(expr, ...)         XR_ASSERT_ALWAYS_FALSE_FM(expr, __VA_ARGS__)
#define XR_ASSERT_TEST_TRUE_FM( expr, ...)         XR_ASSERT_ALWAYS_TRUE_FM( expr, __VA_ARGS__)
#define XR_ASSERT_TEST_EQ_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_EQ_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_TEST_NE_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_NE_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_TEST_LT_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_LT_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_TEST_LE_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_LE_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_TEST_GT_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_GT_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_TEST_GE_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_GE_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_TEST_FALSE(expr)                 XR_EXPECT_ALWAYS_FALSE(expr)
#define XR_EXPECT_TEST_TRUE(expr)                  XR_EXPECT_ALWAYS_TRUE(expr)
#define XR_EXPECT_TEST_EQ(expr1, expr2)            XR_EXPECT_ALWAYS_EQ(expr1, expr2)
#define XR_EXPECT_TEST_NE(expr1, expr2)            XR_EXPECT_ALWAYS_NE(expr1, expr2)
#define XR_EXPECT_TEST_LT(expr1, expr2)            XR_EXPECT_ALWAYS_LT(expr1, expr2)
#define XR_EXPECT_TEST_LE(expr1, expr2)            XR_EXPECT_ALWAYS_LE(expr1, expr2)
#define XR_EXPECT_TEST_GT(expr1, expr2)            XR_EXPECT_ALWAYS_GT(expr1, expr2)
#define XR_EXPECT_TEST_GE(expr1, expr2)            XR_EXPECT_ALWAYS_GE(expr1, expr2)
#define XR_EXPECT_TEST_FALSE_M(expr, message)      XR_EXPECT_ALWAYS_FALSE_M(expr, message)
#define XR_EXPECT_TEST_TRUE_M( expr, message)      XR_EXPECT_ALWAYS_TRUE_M( expr, message)
#define XR_EXPECT_TEST_EQ_M(expr1, expr2, message) XR_EXPECT_ALWAYS_EQ_M(expr1, expr2, message)
#define XR_EXPECT_TEST_NE_M(expr1, expr2, message) XR_EXPECT_ALWAYS_NE_M(expr1, expr2, message)
#define XR_EXPECT_TEST_LT_M(expr1, expr2, message) XR_EXPECT_ALWAYS_LT_M(expr1, expr2, message)
#define XR_EXPECT_TEST_LE_M(expr1, expr2, message) XR_EXPECT_ALWAYS_LE_M(expr1, expr2, message)
#define XR_EXPECT_TEST_GT_M(expr1, expr2, message) XR_EXPECT_ALWAYS_GT_M(expr1, expr2, message)
#define XR_EXPECT_TEST_GE_M(expr1, expr2, message) XR_EXPECT_ALWAYS_GE_M(expr1, expr2, message)
#define XR_EXPECT_TEST_FALSE_FM(expr, ...)         XR_EXPECT_ALWAYS_FALSE_FM(expr, __VA_ARGS__)
#define XR_EXPECT_TEST_TRUE_FM( expr, ...)         XR_EXPECT_ALWAYS_TRUE_FM( expr, __VA_ARGS__)
#define XR_EXPECT_TEST_EQ_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_EQ_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_TEST_NE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_NE_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_TEST_LT_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_LT_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_TEST_LE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_LE_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_TEST_GT_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_GT_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_TEST_GE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_GE_FM(expr1, expr2, __VA_ARGS__)
#define XR_TEST_UNEXPECTED()                       XR_ALWAYS_UNEXPECTED()
#define XR_TEST_UNEXPECTED_M(message)              XR_ALWAYS_UNEXPECTED_M(message)
#define XR_TEST_UNEXPECTED_FM(...)                 XR_ALWAYS_UNEXPECTED_FM(__VA_ARGS__)
#define XR_TEST_FAIL()                             XR_ALWAYS_FAIL()
#define XR_TEST_FAIL_M(message)                    XR_ALWAYS_FAIL_M(message)
#define XR_TEST_FAIL_FM(...)                       XR_ALWAYS_FAIL_FM(__VA_ARGS__)
#else

#define XR_ASSERT_TEST_FALSE(expr)                  ((void)0)
#define XR_ASSERT_TEST_TRUE(expr)                   ((void)0)
#define XR_ASSERT_TEST_EQ(expr1, expr2)             ((void)0)
#define XR_ASSERT_TEST_NE(expr1, expr2)             ((void)0)
#define XR_ASSERT_TEST_LT(expr1, expr2)             ((void)0)
#define XR_ASSERT_TEST_LE(expr1, expr2)             ((void)0)
#define XR_ASSERT_TEST_GT(expr1, expr2)             ((void)0)
#define XR_ASSERT_TEST_GE(expr1, expr2)             ((void)0)
#define XR_ASSERT_TEST_FALSE_M(expr, message)       ((void)0)
#define XR_ASSERT_TEST_TRUE_M( expr, message)       ((void)0)
#define XR_ASSERT_TEST_EQ_M(expr1, expr2, message)  ((void)0)
#define XR_ASSERT_TEST_NE_M(expr1, expr2, message)  ((void)0)
#define XR_ASSERT_TEST_LT_M(expr1, expr2, message)  ((void)0)
#define XR_ASSERT_TEST_LE_M(expr1, expr2, message)  ((void)0)
#define XR_ASSERT_TEST_GT_M(expr1, expr2, message)  ((void)0)
#define XR_ASSERT_TEST_GE_M(expr1, expr2, message)  ((void)0)
#define XR_ASSERT_TEST_FALSE_FM(expr, ...)          ((void)0)
#define XR_ASSERT_TEST_TRUE_FM( expr, ...)          ((void)0)
#define XR_ASSERT_TEST_EQ_FM(expr1, expr2, ...)     ((void)0)
#define XR_ASSERT_TEST_NE_FM(expr1, expr2, ...)     ((void)0)
#define XR_ASSERT_TEST_LT_FM(expr1, expr2, ...)     ((void)0)
#define XR_ASSERT_TEST_LE_FM(expr1, expr2, ...)     ((void)0)
#define XR_ASSERT_TEST_GT_FM(expr1, expr2, ...)     ((void)0)
#define XR_ASSERT_TEST_GE_FM(expr1, expr2, ...)     ((void)0)
#define XR_EXPECT_TEST_FALSE(expr)                  ((void)0)
#define XR_EXPECT_TEST_TRUE(expr)                   ((void)0)
#define XR_EXPECT_TEST_EQ(expr1, expr2)             ((void)0)
#define XR_EXPECT_TEST_NE(expr1, expr2)             ((void)0)
#define XR_EXPECT_TEST_LT(expr1, expr2)             ((void)0)
#define XR_EXPECT_TEST_LE(expr1, expr2)             ((void)0)
#define XR_EXPECT_TEST_GT(expr1, expr2)             ((void)0)
#define XR_EXPECT_TEST_GE(expr1, expr2)             ((void)0)
#define XR_EXPECT_TEST_FALSE_M(expr, message)       ((void)0)
#define XR_EXPECT_TEST_TRUE_M( expr, message)       ((void)0)
#define XR_EXPECT_TEST_EQ_M(expr1, expr2, message)  ((void)0)
#define XR_EXPECT_TEST_NE_M(expr1, expr2, message)  ((void)0)
#define XR_EXPECT_TEST_LT_M(expr1, expr2, message)  ((void)0)
#define XR_EXPECT_TEST_LE_M(expr1, expr2, message)  ((void)0)
#define XR_EXPECT_TEST_GT_M(expr1, expr2, message)  ((void)0)
#define XR_EXPECT_TEST_GE_M(expr1, expr2, message)  ((void)0)
#define XR_EXPECT_TEST_FALSE_FM(expr, ...)          ((void)0)
#define XR_EXPECT_TEST_TRUE_FM( expr, ...)          ((void)0)
#define XR_EXPECT_TEST_EQ_FM(expr1, expr2, ...)     ((void)0)
#define XR_EXPECT_TEST_NE_FM(expr1, expr2, ...)     ((void)0)
#define XR_EXPECT_TEST_LT_FM(expr1, expr2, ...)     ((void)0)
#define XR_EXPECT_TEST_LE_FM(expr1, expr2, ...)     ((void)0)
#define XR_EXPECT_TEST_GT_FM(expr1, expr2, ...)     ((void)0)
#define XR_EXPECT_TEST_GE_FM(expr1, expr2, ...)     ((void)0)
#define XR_TEST_UNEXPECTED()                        ((void)0)
#define XR_TEST_UNEXPECTED_M(message)               ((void)0)
#define XR_TEST_UNEXPECTED_FM(...)                  ((void)0)
#define XR_TEST_FAIL()                              ((void)0)
#define XR_TEST_FAIL_M(message)                     ((void)0)
#define XR_TEST_FAIL_FM(...)                        ((void)0)
#endif

#if defined(XR_DEBUG_FEATURES_ENABLED) && !defined(XR_CODE_COVERAGE_BUILD) || defined(XR_ASSERT_INTERNAL_TESTS)
#define XR_ASSERT_DEBUG_FALSE(expr)                 XR_ASSERT_ALWAYS_FALSE(expr)
#define XR_ASSERT_DEBUG_TRUE(expr)                  XR_ASSERT_ALWAYS_TRUE(expr)
#define XR_ASSERT_DEBUG_EQ(expr1, expr2)            XR_ASSERT_ALWAYS_EQ(expr1, expr2)
#define XR_ASSERT_DEBUG_NE(expr1, expr2)            XR_ASSERT_ALWAYS_NE(expr1, expr2)
#define XR_ASSERT_DEBUG_LT(expr1, expr2)            XR_ASSERT_ALWAYS_LT(expr1, expr2)
#define XR_ASSERT_DEBUG_LE(expr1, expr2)            XR_ASSERT_ALWAYS_LE(expr1, expr2)
#define XR_ASSERT_DEBUG_GT(expr1, expr2)            XR_ASSERT_ALWAYS_GT(expr1, expr2)
#define XR_ASSERT_DEBUG_GE(expr1, expr2)            XR_ASSERT_ALWAYS_GE(expr1, expr2)
#define XR_ASSERT_DEBUG_FALSE_M(expr, message)      XR_ASSERT_ALWAYS_FALSE_M(expr, message)
#define XR_ASSERT_DEBUG_TRUE_M( expr, message)      XR_ASSERT_ALWAYS_TRUE_M( expr, message)
#define XR_ASSERT_DEBUG_EQ_M(expr1, expr2, message) XR_ASSERT_ALWAYS_EQ_M(expr1, expr2, message)
#define XR_ASSERT_DEBUG_NE_M(expr1, expr2, message) XR_ASSERT_ALWAYS_NE_M(expr1, expr2, message)
#define XR_ASSERT_DEBUG_LT_M(expr1, expr2, message) XR_ASSERT_ALWAYS_LT_M(expr1, expr2, message)
#define XR_ASSERT_DEBUG_LE_M(expr1, expr2, message) XR_ASSERT_ALWAYS_LE_M(expr1, expr2, message)
#define XR_ASSERT_DEBUG_GT_M(expr1, expr2, message) XR_ASSERT_ALWAYS_GT_M(expr1, expr2, message)
#define XR_ASSERT_DEBUG_GE_M(expr1, expr2, message) XR_ASSERT_ALWAYS_GE_M(expr1, expr2, message)
#define XR_ASSERT_DEBUG_FALSE_FM(expr, ...)         XR_ASSERT_ALWAYS_FALSE_FM(expr, __VA_ARGS__)
#define XR_ASSERT_DEBUG_TRUE_FM( expr, ...)         XR_ASSERT_ALWAYS_TRUE_FM( expr, __VA_ARGS__)
#define XR_ASSERT_DEBUG_EQ_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_EQ_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_DEBUG_NE_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_NE_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_DEBUG_LT_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_LT_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_DEBUG_LE_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_LE_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_DEBUG_GT_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_GT_FM(expr1, expr2, __VA_ARGS__)
#define XR_ASSERT_DEBUG_GE_FM(expr1, expr2, ...)    XR_ASSERT_ALWAYS_GE_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_DEBUG_FALSE(expr)                 XR_EXPECT_ALWAYS_FALSE(expr)
#define XR_EXPECT_DEBUG_TRUE(expr)                  XR_EXPECT_ALWAYS_TRUE(expr)
#define XR_EXPECT_DEBUG_EQ(expr1, expr2)            XR_EXPECT_ALWAYS_EQ(expr1, expr2)
#define XR_EXPECT_DEBUG_NE(expr1, expr2)            XR_EXPECT_ALWAYS_NE(expr1, expr2)
#define XR_EXPECT_DEBUG_LT(expr1, expr2)            XR_EXPECT_ALWAYS_LT(expr1, expr2)
#define XR_EXPECT_DEBUG_LE(expr1, expr2)            XR_EXPECT_ALWAYS_LE(expr1, expr2)
#define XR_EXPECT_DEBUG_GT(expr1, expr2)            XR_EXPECT_ALWAYS_GT(expr1, expr2)
#define XR_EXPECT_DEBUG_GE(expr1, expr2)            XR_EXPECT_ALWAYS_GE(expr1, expr2)
#define XR_EXPECT_DEBUG_FALSE_M(expr, message)      XR_EXPECT_ALWAYS_FALSE_M(expr, message)
#define XR_EXPECT_DEBUG_TRUE_M( expr, message)      XR_EXPECT_ALWAYS_TRUE_M( expr, message)
#define XR_EXPECT_DEBUG_EQ_M(expr1, expr2, message) XR_EXPECT_ALWAYS_EQ_M(expr1, expr2, message)
#define XR_EXPECT_DEBUG_NE_M(expr1, expr2, message) XR_EXPECT_ALWAYS_NE_M(expr1, expr2, message)
#define XR_EXPECT_DEBUG_LT_M(expr1, expr2, message) XR_EXPECT_ALWAYS_LT_M(expr1, expr2, message)
#define XR_EXPECT_DEBUG_LE_M(expr1, expr2, message) XR_EXPECT_ALWAYS_LE_M(expr1, expr2, message)
#define XR_EXPECT_DEBUG_GT_M(expr1, expr2, message) XR_EXPECT_ALWAYS_GT_M(expr1, expr2, message)
#define XR_EXPECT_DEBUG_GE_M(expr1, expr2, message) XR_EXPECT_ALWAYS_GE_M(expr1, expr2, message)
#define XR_EXPECT_DEBUG_FALSE_FM(expr, ...)         XR_EXPECT_ALWAYS_FALSE_FM(expr, __VA_ARGS__)
#define XR_EXPECT_DEBUG_TRUE_FM( expr, ...)         XR_EXPECT_ALWAYS_TRUE_FM( expr, __VA_ARGS__)
#define XR_EXPECT_DEBUG_EQ_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_EQ_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_DEBUG_NE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_NE_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_DEBUG_LT_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_LT_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_DEBUG_LE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_LE_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_DEBUG_GT_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_GT_FM(expr1, expr2, __VA_ARGS__)
#define XR_EXPECT_DEBUG_GE_FM(expr1, expr2, ...)    XR_EXPECT_ALWAYS_GE_FM(expr1, expr2, __VA_ARGS__)
#define XR_DEBUG_UNEXPECTED()                       XR_ALWAYS_UNEXPECTED()
#define XR_DEBUG_UNEXPECTED_M(message)              XR_ALWAYS_UNEXPECTED_M(message)
#define XR_DEBUG_UNEXPECTED_FM(...)                 XR_ALWAYS_UNEXPECTED_FM(__VA_ARGS__)
#define XR_DEBUG_FAIL()                             XR_ALWAYS_FAIL()
#define XR_DEBUG_FAIL_M(message)                    XR_ALWAYS_FAIL_M(message)
#define XR_DEBUG_FAIL_FM(...)                       XR_ALWAYS_FAIL_FM(__VA_ARGS__)

#else

#define XR_ASSERT_DEBUG_FALSE(expr)                 ((void)0)
#define XR_ASSERT_DEBUG_TRUE(expr)                  ((void)0)
#define XR_ASSERT_DEBUG_EQ(expr1, expr2)            ((void)0)
#define XR_ASSERT_DEBUG_NE(expr1, expr2)            ((void)0)
#define XR_ASSERT_DEBUG_LT(expr1, expr2)            ((void)0)
#define XR_ASSERT_DEBUG_LE(expr1, expr2)            ((void)0)
#define XR_ASSERT_DEBUG_GT(expr1, expr2)            ((void)0)
#define XR_ASSERT_DEBUG_GE(expr1, expr2)            ((void)0)
#define XR_ASSERT_DEBUG_FALSE_M(expr, message)      ((void)0)
#define XR_ASSERT_DEBUG_TRUE_M( expr, message)      ((void)0)
#define XR_ASSERT_DEBUG_EQ_M(expr1, expr2, message) ((void)0)
#define XR_ASSERT_DEBUG_NE_M(expr1, expr2, message) ((void)0)
#define XR_ASSERT_DEBUG_LT_M(expr1, expr2, message) ((void)0)
#define XR_ASSERT_DEBUG_LE_M(expr1, expr2, message) ((void)0)
#define XR_ASSERT_DEBUG_GT_M(expr1, expr2, message) ((void)0)
#define XR_ASSERT_DEBUG_GE_M(expr1, expr2, message) ((void)0)
#define XR_ASSERT_DEBUG_FALSE_FM(expr, ...)         ((void)0)
#define XR_ASSERT_DEBUG_TRUE_FM( expr, ...)         ((void)0)
#define XR_ASSERT_DEBUG_EQ_FM(expr1, expr2, ...)    ((void)0)
#define XR_ASSERT_DEBUG_NE_FM(expr1, expr2, ...)    ((void)0)
#define XR_ASSERT_DEBUG_LT_FM(expr1, expr2, ...)    ((void)0)
#define XR_ASSERT_DEBUG_LE_FM(expr1, expr2, ...)    ((void)0)
#define XR_ASSERT_DEBUG_GT_FM(expr1, expr2, ...)    ((void)0)
#define XR_ASSERT_DEBUG_GE_FM(expr1, expr2, ...)    ((void)0)
#define XR_EXPECT_DEBUG_FALSE(expr)                 ((void)0)
#define XR_EXPECT_DEBUG_TRUE(expr)                  ((void)0)
#define XR_EXPECT_DEBUG_EQ(expr1, expr2)            ((void)0)
#define XR_EXPECT_DEBUG_NE(expr1, expr2)            ((void)0)
#define XR_EXPECT_DEBUG_LT(expr1, expr2)            ((void)0)
#define XR_EXPECT_DEBUG_LE(expr1, expr2)            ((void)0)
#define XR_EXPECT_DEBUG_GT(expr1, expr2)            ((void)0)
#define XR_EXPECT_DEBUG_GE(expr1, expr2)            ((void)0)
#define XR_EXPECT_DEBUG_FALSE_M(expr, message)      ((void)0)
#define XR_EXPECT_DEBUG_TRUE_M( expr, message)      ((void)0)
#define XR_EXPECT_DEBUG_EQ_M(expr1, expr2, message) ((void)0)
#define XR_EXPECT_DEBUG_NE_M(expr1, expr2, message) ((void)0)
#define XR_EXPECT_DEBUG_LT_M(expr1, expr2, message) ((void)0)
#define XR_EXPECT_DEBUG_LE_M(expr1, expr2, message) ((void)0)
#define XR_EXPECT_DEBUG_GT_M(expr1, expr2, message) ((void)0)
#define XR_EXPECT_DEBUG_GE_M(expr1, expr2, message) ((void)0)
#define XR_EXPECT_DEBUG_FALSE_FM(expr, ...)         ((void)0)
#define XR_EXPECT_DEBUG_TRUE_FM( expr, ...)         ((void)0)
#define XR_EXPECT_DEBUG_EQ_FM(expr1, expr2, ...)    ((void)0)
#define XR_EXPECT_DEBUG_NE_FM(expr1, expr2, ...)    ((void)0)
#define XR_EXPECT_DEBUG_LT_FM(expr1, expr2, ...)    ((void)0)
#define XR_EXPECT_DEBUG_LE_FM(expr1, expr2, ...)    ((void)0)
#define XR_EXPECT_DEBUG_GT_FM(expr1, expr2, ...)    ((void)0)
#define XR_EXPECT_DEBUG_GE_FM(expr1, expr2, ...)    ((void)0)
#define XR_DEBUG_UNEXPECTED()                       ((void)0)
#define XR_DEBUG_UNEXPECTED_M(message)              ((void)0)
#define XR_DEBUG_UNEXPECTED_FM(...)                 ((void)0)
#define XR_DEBUG_FAIL()                             ((void)0)
#define XR_DEBUG_FAIL_M(message)                    ((void)0)
#define XR_DEBUG_FAIL_FM(...)                       ((void)0)
#endif
//@}

/// This can be used to denote an area that has not been implemented yet
/// and is not thought to be used yet. Hitting one of these means you should
/// probably implement the functionality yourself.
//@{
#define XR_UNIMPLEMENTED( ) XR_ALWAYS_UNEXPECTED_M("unimplemented");
#define XR_UNIMPLEMENTED_M( str ) XR_ALWAYS_UNEXPECTED_M("unimplemented");
//@}

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/// This is the action to be taken on an assert.
// ***************************************************************************************** - TYPE
enum AssertFailAction
{
    kActionBreak,          //!< Trigger debug break, or exit process.
    kActionContinue,       //!< Do nothing, keep going.
};
// ***************************************************************************************** - TYPE
/// Dispatch type is passed to the handler, its use it purely by convention. See values for details.
// ***************************************************************************************** - TYPE
enum AssertCallType
{
    kCallAssert,    //!< "Assert" statement failed. (Implies code will crash / misbehave in some way if not met)
    kCallExpect     //!< "Expect" statement failed. (Implies code can continue even if failed)
};
// ***************************************************************************************** - TYPE
/*! Implemented by higher level code to manage assert events intelligently.
    Default implementation will always print to stderr and break.
    */
// ***************************************************************************************** - TYPE
class XR_PURE_INTERFACE IAssertFailedHandler: public xr::COM::IUnknown{
public:
    XR_COM_CLASS_ID(0x0967d9b);

    virtual ~IAssertFailedHandler() {}

    // ------------------------------------------------------------------------------------  MEMBER
    /*! Called when an assert fails */
    // ------------------------------------------------------------------------------------  MEMBER
    virtual AssertFailAction AssertFailed(AssertCallType type,
                                    const char * file,
                                    const int32_t line,
                                    const char * message = nullptr)
                                    XR_WARN_IF_RETURN_UNUSED = 0;
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Called when an assert fails */
    // ------------------------------------------------------------------------------------  MEMBER

    virtual AssertFailAction AssertFailedFormatted(AssertCallType type,
                                            const char * file,
                                            const int32_t line,
#if defined(XR_COMPILER_MICROSOFT)
                                            _Printf_format_string_
#endif
                                            const char * format,
                                            ...)
                                            XR_WARN_IF_RETURN_UNUSED
#if defined(XR_COMPILER_GCC)
            __attribute__ ((format (printf, 5, 6))) // 5th arg, because of implicit "this"
#endif
        = 0;
};

// ***************************************************************************************** - TYPE
/// \copydoc xr/core/assert.h
/// \copydoc coreassert
// ***************************************************************************************** - TYPE
class Assert
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Sets the currently registered assert handler (returns the old value).*/
    // ------------------------------------------------------------------------------------  MEMBER
    static IAssertFailedHandler & SetFailHandler(IAssertFailedHandler & pNewHandler);
    // ------------------------------------------------------------------------------------  MEMBER
    /// return the currently registered assert failed handler.
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_SIDE_EFFECTS_PREFIX
    static IAssertFailedHandler & GetFailHandler() XR_NO_SIDE_EFFECTS_POSTFIX;
private:
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Variable to return the currently registered assert handler.
        Note that the global variable reduces code space. It should not be used
        directly though (and it unlikely to be misused).
    */
    // ------------------------------------------------------------------------------------  MEMBER
    static IAssertFailedHandler * sAssertFailHandler;
};

}} // namespace xr::Core
#endif //#ifndef XR_CORE_ASSERT_H
