// ######################################################################################### - FILE
/*!

\author Copyright 2011 by Daniel Craig. All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef SAMPLE_H
#include "sample.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif

// ######################################################################################### - FILE
/* Unit Tests */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

XR_UNITTEST_GROUP_BEGIN( Sample )

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( SampleTest1 )
{

}

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( SampleTest2 )
{

}

XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
