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
#ifndef XR_CORE_THREADING_TLS_H
#include "xr/core/threading/tls.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif


#if defined(XR_PLATFORM_WINDOWS)
XR_DISABLE_ALL_WARNINGS()
#include <windows.h>
#include <process.h>
XR_RESTORE_ALL_WARNINGS()
#else

// Unix variants.
#include <unistd.h>
#if defined(_POSIX_THREADS)
#include <pthread.h>
#endif
#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

#if defined(XR_PLATFORM_WINDOWS)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
detail::ThreadLocalStorageBase::ThreadLocalStorageBase()
{
    mTlsId = static_cast<uintptr_t>(TlsAlloc());
    XR_ASSERT_ALWAYS_NE_FM(mTlsId, TLS_OUT_OF_INDEXES, "Error: Unable to Alloc Thread Local Storage GetLastError:0x%lX", GetLastError());
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
detail::ThreadLocalStorageBase::~ThreadLocalStorageBase()
{
    BOOL b;
    b = TlsFree(static_cast<DWORD>(mTlsId));
    XR_ASSERT_ALWAYS_NE_FM(b, 0, "Error: Unable to Free Thread Local Storage GetLastError:0x%lX", GetLastError());
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
uintptr_t detail::ThreadLocalStorageBase::GetValueInternal()
{
    uintptr_t value = uintptr_t(TlsGetValue(static_cast<DWORD>(mTlsId)));
    // Debug only assert as it is reasonable to expect that tls values can be retrieved assuming allocation succeeded.
    XR_ASSERT_DEBUG_EQ_FM(GetLastError(), ERROR_SUCCESS, "Error: Unable to Obtain Thread Local Storage Value GetLastError:0x%lX", GetLastError());
    return value;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void      detail::ThreadLocalStorageBase::SetValueInternal(uintptr_t value)
{
    BOOL b;
    b = TlsSetValue(static_cast<DWORD>(mTlsId), (void*)(value));
    XR_UNUSED(b);
    // Debug only assert as it is reasonable to expect that tls values can be retrieved assuming allocation succeeded.
    XR_ASSERT_DEBUG_NE_FM(b, 0, "Error: Unable to Free Thread Local Storage GetLastError:0x%lX", GetLastError());
}
#elif defined(_POSIX_THREADS)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline void HandleErrno(int errvalue, const char * where)
{
    XR_ASSERT_ALWAYS_EQ_FM(errvalue, 0, "Error: Error returned from %s. errno:%d", where, errvalue);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
detail::ThreadLocalStorageBase::ThreadLocalStorageBase()
{
    pthread_key_t temp;
    int errval = pthread_key_create(&temp, nullptr);
    mTlsId = static_cast<uintptr_t>(temp);
    HandleErrno(errval, "pthread_key_create");
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
detail::ThreadLocalStorageBase::~ThreadLocalStorageBase()
{
    pthread_key_t temp = static_cast<pthread_key_t>(mTlsId);
    int errval = pthread_key_delete(temp);
    HandleErrno(errval, "pthread_key_delete");

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
uintptr_t detail::ThreadLocalStorageBase::GetValueInternal()
{
    pthread_key_t temp = static_cast<pthread_key_t>(mTlsId);
    void * value = pthread_getspecific(temp);
    return (uintptr_t)(value);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void      detail::ThreadLocalStorageBase::SetValueInternal(uintptr_t value)
{
    pthread_key_t temp = static_cast<pthread_key_t>(mTlsId);
    int errval = pthread_setspecific(temp, (void *)value);
    HandleErrno(errval, "pthread_setspecific");
}
#else // Platform
#error "Need a TLS implementation for this platform (or addit to an existing platform)"
#endif

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
