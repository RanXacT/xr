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
#ifndef XR_CORE_MEMORY_H
#include "xr/core/memory.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif

#if defined(XR_PLATFORM_WINDOWS)
// Required for VirtualAlloc, VirtualProtect and the like.
// Could just pull in declarations, but let's not for now.
XR_DISABLE_ALL_WARNINGS()
#include <windows.h>
XR_RESTORE_ALL_WARNINGS()
#endif

#if defined(XR_PLATFORM_DARWIN)
#include <stdlib.h>
#include <mach/mach.h>
#else
#include <malloc.h>
#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ######################################################################################### - FILE
// Memory Page functions
// ######################################################################################### - FILE
#if defined(XR_PLATFORM_WINDOWS)
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
XR_RETURN_BYTES(size)
void *Memory::PageAlloc(size_t size, PageAccess eProtection)
{
    unsigned long flProtect = PAGE_READWRITE;

    switch(eProtection)
    {
    default:
        XR_ALWAYS_FAIL_FM("Invalid PageAlloc protection :0x%" XR_INT32_PRINTx, int32_t(eProtection));
        // Fall through to kPageAccessNone is intentional.
    case kPageAccessNone:
        // Leave the READ access for debugger use
        // Put on the guard modifier to prevent code access (will generate
        // an exception, "STATUS_GUARD_PAGE_VIOLATION")
        flProtect = PAGE_READONLY | PAGE_GUARD;
        break;
    case kPageAccessRead:
        flProtect = PAGE_READONLY;
        break;
    case kPageAccessReadWrite:
        flProtect = PAGE_READWRITE;
        break;
    //case kPageAccessExecute:
    //    flProtect = PAGE_EXECUTE;
    //    break;
    }
    void * p = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, flProtect);

    if(p == nullptr)
    {
        // This is always expected to succeed, we cannot return without memory.
        XR_ALWAYS_FAIL_FM("VirtualAlloc failed %lx", GetLastError());
    }

    return p;
}
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
void  Memory::PageFree(void * p, size_t size)
{
    XR_UNUSED(size);
    if(VirtualFree(p, 0, MEM_RELEASE) == 0)
    {
        // This is bad, but the program will likely continue to execute.
        XR_ALWAYS_UNEXPECTED_FM("VirtualFree failed %lx", GetLastError());
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief  changes protection on memory allocated with SystemPageAlloc. */
// --------------------------------------------------------------------------------------  FUNCTION
void  Memory::PageProtect(void * p, size_t size, PageAccess eProtection)
{
    unsigned long flProtect = PAGE_READWRITE;
    unsigned long oldProtect = PAGE_READWRITE;

    switch(eProtection)
    {
    default:
        XR_ALWAYS_FAIL_FM("Invalid PageAlloc protection :0x%" XR_INT32_PRINTx, int32_t(eProtection));
        // Fall through to kPageAccessNone is intentional.
    case kPageAccessNone:
        // Leave the READ access for debugger use
        // Put on the guard modifier to prevent code access (will generate
        // an exception, "STATUS_GUARD_PAGE_VIOLATION")
        flProtect = PAGE_READONLY | PAGE_GUARD;
        break;
    case kPageAccessRead:
        flProtect = PAGE_READONLY;
        break;
    case kPageAccessReadWrite:
        flProtect = PAGE_READWRITE;
        break;
    //case kPageAccessExecute:
    //    flProtect = PAGE_EXECUTE;
    //    break;
    }


    if(VirtualProtect(p, size, flProtect, &oldProtect) == 0)
    {
        // This is always expected to succeed.
        XR_ALWAYS_FAIL_FM("VirtualProtect failed %lx", GetLastError());
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t Memory::PageSize()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    return si.dwPageSize;
}

#elif defined(XR_PLATFORM_DARWIN)
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
void *Memory::PageAlloc(size_t size, PageAccess eProtection)
{
    vm_address_t p;
    kern_return_t ret = vm_allocate(mach_task_self(),
        &p,
        (vm_size_t) size,
        VM_FLAGS_ANYWHERE);

    if(ret != KERN_SUCCESS)
    {
        // This is always expected to succeed, we cannot return without memory.
        XR_ALWAYS_FAIL_FM("vm_allocate failed %"XR_UINT32_PRINTX, (uint32_t)ret);
    }

    if(eProtection != kPageAccessReadWrite)
    {
        PageProtect((void*)p, size, eProtection);
    }

    return (void*)p;
}
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
void  Memory::PageFree(void * p, size_t size)
{
    kern_return_t ret = vm_deallocate(mach_task_self(),
        (vm_address_t)p,
        (vm_size_t) size);

    if(ret != KERN_SUCCESS)
    {
        // This is bad, but the program will likely continue to execute.
        XR_ALWAYS_UNEXPECTED_FM("vm_deallocate failed %"XR_UINT32_PRINTX, (uint32_t)ret);
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief  changes protection on memory allocated with SystemPageAlloc. */
// --------------------------------------------------------------------------------------  FUNCTION
void  Memory::PageProtect(void * p, size_t size, PageAccess eProtection)
{

    vm_prot_t protect = VM_PROT_READ | VM_PROT_WRITE;

    switch(eProtection)
    {
    default:
        XR_ALWAYS_FAIL_FM("Invalid PageAlloc protection :0x%" XR_INT32_PRINTx, int32_t(eProtection));
        // Fall through to kPageAccessNone is intentional.
    case kPageAccessNone:
        protect = 0;
        break;
    case kPageAccessRead:
        protect = VM_PROT_READ;
        break;
    case kPageAccessReadWrite:
        protect = VM_PROT_READ | VM_PROT_WRITE;
        break;
    //case kPageAccessExecute:
    //    protect = VM_PROT_EXECUTE;
    //    break;
    }

    kern_return_t ret = vm_protect(mach_task_self(),
        (vm_address_t)p,
        (vm_size_t) size,
        false,
        protect
        );

    if(ret != KERN_SUCCESS)
    {
        // This is always expected to succeed.
        XR_ALWAYS_FAIL_FM("vm_protect failed %"XR_UINT32_PRINTX, (uint32_t)ret);
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t Memory::PageSize()
{
    return sysconf(_SC_PAGESIZE);
}

#else

// Other systems may not have a special page allocation system, just use malloc free.
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
void *Memory::PageAlloc(size_t size, PageAccess eProtection)
{
    XR_UNUSED(eProtection);
    switch(eProtection)
    {
    default:
        XR_ALWAYS_FAIL_FM("Invalid PageAlloc protection :0x%" XR_INT32_PRINTx, int32_t(eProtection));
        // Fall through to kPageAccessNone is intentional.
    case kPageAccessNone:
    case kPageAccessRead:
    case kPageAccessReadWrite:
        break;
    }

    void * p = malloc(size);

    if(p == nullptr)
    {
        // This is always expected to succeed, we cannot return without memory.
        XR_ALWAYS_FAIL();
    }

    return p;
}
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
void  Memory::PageFree(void * p, size_t size)
{
    free(p);
}

// --------------------------------------------------------------------------------------  FUNCTION
/*! \brief  changes protection on memory allocated with SystemPageAlloc. */
// --------------------------------------------------------------------------------------  FUNCTION
void  Memory::PageProtect(void * p, size_t size, PageAccess eProtection)
{
    switch(eProtection)
    {
    default:
        XR_ALWAYS_FAIL_FM("Invalid PageAlloc protection :0x%" XR_INT32_PRINTx, int32_t(eProtection));
        // Fall through to kPageAccessNone is intentional.
    case kPageAccessNone:
    case kPageAccessRead:
    case kPageAccessReadWrite:
        break;
    }

    XR_UNUSED(p);
    XR_UNUSED(size);
    XR_UNUSED(eProtection);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t Memory::PageSize()
{
    return sysconf(_SC_PAGESIZE);
}

#endif
}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
