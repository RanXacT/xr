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

#if defined(XR_PLATFORM_WINDOWS)
#include "xr/platform/windows_lite.h"
#elif defined(XR_PLATFORM_DARWIN)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>
#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
#if defined(XR_PLATFORM_WINDOWS)
bool IsDebuggerConnected()
{
    return IsDebuggerPresent() != 0 ? true : false;
}

#elif defined(XR_PLATFORM_DARWIN)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
// Standard function to detect debugger in osx.
bool IsDebuggerConnected() {
    int mib[4];
    struct kinfo_proc info;
    size_t size;

    info.kp_proc.p_flag = 0;
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();

    size = sizeof(info);
    sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, nullptr, 0);

    return ((info.kp_proc.p_flag & P_TRACED) != 0);
}
#else
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool IsDebuggerConnected()
{
    return false;
}

#endif
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
static_assert( ( XR_INT8_MIN +  XR_INT8_MAX) == XR_INT8_C( -1) , "defines error");
static_assert( (XR_INT16_MIN + XR_INT16_MAX) == XR_INT16_C(-1) , "defines error");
static_assert( (XR_INT32_MIN + XR_INT32_MAX) == XR_INT32_C(-1) , "defines error");
static_assert( (XR_INT64_MIN + XR_INT64_MAX) == XR_INT64_C(-1) , "defines error");

static_assert( ( XR_INT8_MAX > 0), "defines error");
static_assert( (XR_INT16_MAX > 0), "defines error");
static_assert( (XR_INT32_MAX > 0), "defines error");
static_assert( (XR_INT64_MAX > 0), "defines error");

static_assert( ( XR_INT8_MIN < 0), "defines error");
static_assert( (XR_INT16_MIN < 0), "defines error");
static_assert( (XR_INT32_MIN < 0), "defines error");
static_assert( (XR_INT64_MIN < 0), "defines error");

#if XR_CHAR8_IS_SIGNED
static_assert( ( XR_CHAR8_MIN +  XR_CHAR8_MAX) == XR_CHAR8_C( -1) , "defines error");
#else
static_assert( ( XR_CHAR8_MIN) == 0 , "defines error");
static_assert( ( XR_CHAR8_MAX)  > 0 , "defines error");
#endif

#if defined(XR_CHAR16_IS_SIGNED)
static_assert( ( XR_CHAR16_MIN +  XR_CHAR16_MAX) == XR_CHAR16_C( -1) , "defines error");
#else
static_assert( ( XR_CHAR16_MIN) == 0 , "defines error");
static_assert( ( XR_CHAR16_MAX)  > 0 , "defines error");
#endif

#if defined(XR_CHAR32_IS_SIGNED)
static_assert( ( XR_CHAR32_MIN +  XR_CHAR32_MAX) == XR_CHAR32_C( -1) , "defines error");
#else
static_assert( ( XR_CHAR32_MIN) == 0 , "defines error");
static_assert( ( XR_CHAR32_MAX)  > 0 , "defines error");
#endif

static_assert( uint8_t( ~0) == XR_UINT8_MAX , "defines error");
static_assert( uint16_t(~0) == XR_UINT16_MAX, "defines error");
static_assert( uint32_t(~0) == XR_UINT32_MAX, "defines error");
static_assert( uint64_t(~0) == XR_UINT64_MAX, "defines error");


static_assert( sizeof(uint8_t) == 1 , "Size Failure");
static_assert( sizeof(uint16_t)== 2 , "Size Failure");
static_assert( sizeof(uint32_t)== 4 , "Size Failure");
static_assert( sizeof(uint64_t)== 8 , "Size Failure");

static_assert( sizeof(int8_t) == 1 , "Size Failure");
static_assert( sizeof(int16_t)== 2 , "Size Failure");
static_assert( sizeof(int32_t)== 4 , "Size Failure");
static_assert( sizeof(int64_t)== 8 , "Size Failure");

static_assert( sizeof(int_least8_t) >= 1 , "Size Failure");
static_assert( sizeof(int_least16_t)>= 2 , "Size Failure");
static_assert( sizeof(int_least32_t)>= 4 , "Size Failure");
static_assert( sizeof(int_least64_t)>= 8 , "Size Failure");

static_assert( sizeof(uint_least8_t) >= 1 , "Size Failure");
static_assert( sizeof(uint_least16_t)>= 2 , "Size Failure");
static_assert( sizeof(uint_least32_t)>= 4 , "Size Failure");
static_assert( sizeof(uint_least64_t)>= 8 , "Size Failure");


static_assert( sizeof(int_fast8_t) >= 1 , "Size Failure");
static_assert( sizeof(int_fast16_t)>= 2 , "Size Failure");
static_assert( sizeof(int_fast32_t)>= 4 , "Size Failure");
static_assert( sizeof(int_fast64_t)>= 8 , "Size Failure");

static_assert( sizeof(uint_fast8_t) >= 1 , "Size Failure");
static_assert( sizeof(uint_fast16_t)>= 2 , "Size Failure");
static_assert( sizeof(uint_fast32_t)>= 4 , "Size Failure");
static_assert( sizeof(uint_fast64_t)>= 8 , "Size Failure");

static_assert( sizeof(intmax_t) >= 4  , "Size Failure");
static_assert( sizeof(uintmax_t)>= 8  , "Size Failure");

static_assert( sizeof(intptr_t) == sizeof(void*)  , "Size Failure");
static_assert( sizeof(uintptr_t)== sizeof(void*)  , "Size Failure");

static_assert( XR_ALIGN_OF(uint8_t) == 1 , "Size Failure");
static_assert( XR_ALIGN_OF(uint16_t)== 2 , "Size Failure");
static_assert( XR_ALIGN_OF(uint32_t)== 4 , "Size Failure");

#if !defined(XR_COMPILER_CLANG)
// This generates a compiler error on clang. The value is different than
// GCC, but was independently verified.
static_assert( XR_ALIGN_OF(uint64_t)== XR_PLATFORM_UINT64_T_ALIGNMENT, "alignment validation" );
#endif

static_assert( sizeof(char8_t) == 1 , "Size Failure");
static_assert( sizeof(char16_t)== 2 , "Size Failure");
static_assert( sizeof(char32_t)== 4 , "Size Failure");


struct OffsetOfTester{
	uint8_t  a0;
	uint16_t a2;
	uint32_t a4;
	uint64_t a8;
};
static_assert( XR_OFFSET_OF(OffsetOfTester, a0)== 0  , "offsetof Failure");
static_assert( XR_OFFSET_OF(OffsetOfTester, a2)== 2  , "offsetof Failure");
static_assert( XR_OFFSET_OF(OffsetOfTester, a4)== 4  , "offsetof Failure");
static_assert( XR_OFFSET_OF(OffsetOfTester, a8)== 8  , "offsetof Failure");

// Does it work with a non-POD type? (common problem with Offset of implementations)
struct OffsetOfTester2{
	OffsetOfTester2() {}
	~OffsetOfTester2() {}
	uint8_t  a0;
	uint16_t a2;
	uint32_t a4;
	uint64_t a8;
};
static_assert( XR_OFFSET_OF(OffsetOfTester2, a0)== 0  , "offsetof Failure");
static_assert( XR_OFFSET_OF(OffsetOfTester2, a2)== 2  , "offsetof Failure");
static_assert( XR_OFFSET_OF(OffsetOfTester2, a4)== 4  , "offsetof Failure");
static_assert( XR_OFFSET_OF(OffsetOfTester2, a8)== 8  , "offsetof Failure");

// reports: structure was padded due to __declspec(align())
// But it is needed to test alignment logic.
#if defined(XR_COMPILER_MICROSOFT)
#pragma warning( push )
#pragma warning (disable : 4324)
#endif

XR_ALIGN_PREFIX(32)
struct AlignmentTesterInner {
	char c[4];
}
XR_ALIGN_POSTFIX(32);

struct AlignmentTesterOuter {
	char c[1];
	AlignmentTesterInner test;
};

static_assert( XR_ALIGN_OF(AlignmentTesterInner)== 32  , "alignof failure");
static_assert( XR_OFFSET_OF(AlignmentTesterOuter, test)== 32  , "alignof failure");
#if defined(XR_COMPILER_MICROSOFT)
#pragma warning( pop )
#endif
