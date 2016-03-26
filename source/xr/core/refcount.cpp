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
#ifndef XR_CORE_REFCOUNT_H
#include "xr/core/refcount.h"
#endif
#ifndef XR_CORE_UNKNOWN_H
#include "xr/core/unknown.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_THREADING_ATOMIC_H
#include "xr/core/threading/atomic.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
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
RefCounted::RefCounted() :
    mReferenceCount(1)
{

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
RefCounted::~RefCounted()
{
    XR_ASSERT_ALWAYS_TRUE(mReferenceCount == 0);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RefCounted::AddRef() const
{
    intptr_t originalValue = xr::Core::AtomicIncrement(&mReferenceCount);
    XR_UNUSED(originalValue);
    XR_ASSERT_ALWAYS_GT(originalValue, 0);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void RefCounted::Release() const
{
    intptr_t originalValue = xr::Core::AtomicDecrement(&mReferenceCount);
    XR_ASSERT_ALWAYS_GT(originalValue, 0);
    if(originalValue == 1)
    {
        RefCounted * temp = const_cast<RefCounted *>(this);
        XR_DELETE(temp);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
NonThreadSafeRefCounted::NonThreadSafeRefCounted()  :
    mReferenceCount(0)
{

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
NonThreadSafeRefCounted::~NonThreadSafeRefCounted()
{
    XR_ASSERT_ALWAYS_TRUE(mReferenceCount == 0);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void NonThreadSafeRefCounted::AddRef() const
{
    XR_ASSERT_ALWAYS_GT( mReferenceCount, 0 );
    ++mReferenceCount;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void NonThreadSafeRefCounted::Release() const
{
    --mReferenceCount;
    XR_ASSERT_ALWAYS_GE(mReferenceCount, 0);

    if(mReferenceCount == 0)
    {
        NonThreadSafeRefCounted * temp = const_cast<NonThreadSafeRefCounted *>(this);
        XR_DELETE(temp);
    }
}

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
