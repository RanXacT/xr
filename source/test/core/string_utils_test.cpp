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
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif

// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( StringUtils )

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_UNITTEST_TEST_FUNC( StringTests )
{
    using namespace xr::Core;

    {
        const char * test1 = "";
        const char * test2 = "2";
        const char * test3 = "23";

        XR_ASSERT_ALWAYS_EQ(StringLengthWithNull(test1), 1 );
        XR_ASSERT_ALWAYS_EQ(StringLengthWithNull(test2), 2 );
        XR_ASSERT_ALWAYS_EQ(StringLengthWithNull(test3), 3 );

        const wchar_t * wtest1 = L"";
        const wchar_t * wtest2 = L"2";
        const wchar_t * wtest3 = L"23";

        XR_ASSERT_ALWAYS_EQ(StringLengthWithNull(wtest1), 1 );
        XR_ASSERT_ALWAYS_EQ(StringLengthWithNull(wtest2), 2 );
        XR_ASSERT_ALWAYS_EQ(StringLengthWithNull(wtest3), 3 );
    }
    {
        char buffer256[256];
        StringCopy(buffer256, "");
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, ""), 0 );

        StringCopy(buffer256, "Hello World");
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, "Hello World"), 0 );

        StringCopy(buffer256, "", 256);
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, ""), 0 );
        XR_ASSERT_ALWAYS_NE(StringCompare(buffer256, "Hello World"), 0 );

        StringCopy(buffer256, "Hello World", 256);
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, "Hello World"), 0 );

        StringCopy(buffer256, "Hello World", 1);
        XR_ASSERT_ALWAYS_NE(StringCompare(buffer256, "Hello World"), 0 );
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, ""), 0 );
        StringCopy(buffer256, "Hello World", 2);
        XR_ASSERT_ALWAYS_NE(StringCompare(buffer256, "Hello World"), 0 );
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, "H"), 0 );

        XR_ASSERT_ALWAYS_EQ( StringPrintf(buffer256, 256, "%s", "Hello World"), xr::Core::kSuccess);
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, "Hello World"), 0 );

        XR_ASSERT_ALWAYS_EQ( StringPrintf(buffer256, 0, "%s", "Something Else"), xr::Core::kFailure);
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, "Hello World"), 0 );

        XR_ASSERT_ALWAYS_EQ( StringPrintf(buffer256, 2, "%s", "Hello World"), xr::Core::kFailure);
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, "H"), 0 );
        XR_ASSERT_ALWAYS_EQ( StringPrintf(buffer256, 1, "%s", "Hello World"), xr::Core::kFailure);
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, ""), 0 );

        XR_ASSERT_ALWAYS_EQ( StringPrintf(buffer256, 2, ""), xr::Core::kSuccess);
        XR_ASSERT_ALWAYS_EQ(StringCompare(buffer256, ""), 0 );
    }
    {
        wchar_t wbuffer256[256];
        StringCopy(wbuffer256, L"");
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L""), 0 );
        StringCopy(wbuffer256, L"Hello World");
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L"Hello World"), 0 );

        StringCopy(wbuffer256, L"", 256);
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L""), 0 );
        XR_ASSERT_ALWAYS_NE(StringCompare(wbuffer256, L"Hello World"), 0 );

        StringCopy(wbuffer256, L"Hello World", 256);
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L"Hello World"), 0 );

        StringCopy(wbuffer256, L"Hello World", 1);
        XR_ASSERT_ALWAYS_NE(StringCompare(wbuffer256, L"Hello World"), 0 );
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L""), 0 );
        StringCopy(wbuffer256, L"Hello World", 2);
        XR_ASSERT_ALWAYS_NE(StringCompare(wbuffer256, L"Hello World"), 0 );
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L"H"), 0 );


        XR_ASSERT_ALWAYS_EQ(StringPrintf(wbuffer256, 256, L"%s", L"Hello World"), xr::Core::kSuccess);
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L"Hello World"), 0 );

        XR_ASSERT_ALWAYS_EQ(StringPrintf(wbuffer256, 0, L"%s", L"Something Else"), xr::Core::kFailure);
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L"Hello World"), 0 );

        XR_ASSERT_ALWAYS_EQ(StringPrintf(wbuffer256, 2, L"%s", L"Hello World"), xr::Core::kFailure);
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L"H"), 0 );
        XR_ASSERT_ALWAYS_EQ(StringPrintf(wbuffer256, 1, L"%s", L"Hello World"), xr::Core::kFailure);
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L""), 0 );

        XR_ASSERT_ALWAYS_EQ(StringPrintf(wbuffer256, 2, L""), xr::Core::kSuccess);
        XR_ASSERT_ALWAYS_EQ(StringCompare(wbuffer256, L""), 0 );
    }

    XR_ASSERT_ALWAYS_EQ(StringCompareNoCase("Hello", "heLLo"), 0);
    XR_ASSERT_ALWAYS_NE(StringCompareNoCase("Hello", "h3LLo"), 0);

    XR_ASSERT_ALWAYS_EQ(StringCompareNoCase(L"Hello", L"heLLo"), 0);
    XR_ASSERT_ALWAYS_NE(StringCompareNoCase(L"Hello", L"h3LLo"), 0);
}
XR_UNITTEST_TEST_FUNC( StringToBoolTests )
{
    using namespace xr::Core;
    XR_ASSERT_ALWAYS_TRUE(StringToBool("true", false));
    XR_ASSERT_ALWAYS_TRUE(StringToBool("True", false));
    XR_ASSERT_ALWAYS_TRUE(StringToBool("TRUE", false));
    XR_ASSERT_ALWAYS_TRUE(StringToBool("1", false));
    XR_ASSERT_ALWAYS_TRUE(StringToBool("yes", false));
    XR_ASSERT_ALWAYS_TRUE(StringToBool("Yes", false));
    XR_ASSERT_ALWAYS_TRUE(StringToBool("YES", false));

    XR_ASSERT_ALWAYS_FALSE(StringToBool("false", true));
    XR_ASSERT_ALWAYS_FALSE(StringToBool("False", true));
    XR_ASSERT_ALWAYS_FALSE(StringToBool("FALSE", true));
    XR_ASSERT_ALWAYS_FALSE(StringToBool("0", true));
    XR_ASSERT_ALWAYS_FALSE(StringToBool("no", true));
    XR_ASSERT_ALWAYS_FALSE(StringToBool("nO", true));
    XR_ASSERT_ALWAYS_FALSE(StringToBool("NO", true));


    // Make sure default values are returned properly.
    XR_ASSERT_ALWAYS_FALSE(StringToBool("", false));
    XR_ASSERT_ALWAYS_TRUE( StringToBool("", true));

    XR_ASSERT_ALWAYS_FALSE(StringToBool("garbage", false));
    XR_ASSERT_ALWAYS_TRUE( StringToBool("garbage", true));

    XR_ASSERT_ALWAYS_FALSE(StringToBool("11", false));
    XR_ASSERT_ALWAYS_TRUE( StringToBool("11", true));

    XR_ASSERT_ALWAYS_FALSE(StringToBool("00", false));
    XR_ASSERT_ALWAYS_TRUE( StringToBool("00", true));

}
XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
