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
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif

#if defined(XR_COMPILER_MINGW)
#define __DO_ALIGN_DEFINES 1
#endif

#if defined(XR_PLATFORM_DARWIN)
#include <stdlib.h>
#else
#include <string.h>
#include <malloc.h>
#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {
// ***************************************************************************************** - TYPE
/// Pass through allocator for system allocator. actual instance is
/// static and is used by default when no other allocator can allocate.
// ***************************************************************************************** - TYPE
class AllocatorCLib : public xr::Core::IUnknownHelper1<AllocatorCLib, Core::IAllocator, Core::NotRefCounted>
{
public:
    XR_COM_CLASS_ID(0x3f2b2e2);

    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief This is intended to be derived from to create custom allocators.
        Allocators will typically be instantiated by the config XML file.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    inline AllocatorCLib() {}
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    inline ~AllocatorCLib() {}

    /// Used by the Manager to select this Allocator factory (referenced as the name in xml file).
    const char * GetName() const XR_OVERRIDE
    {
        return "clib";
    }

    // ------------------------------------------------------------------------------------  MEMBER
    /// Alloc Function
    // ------------------------------------------------------------------------------------  MEMBER
    XR_RETURN_BYTES(size)
    void * Alloc(size_t size, size_t alignment, xr::Core::MemFlags eFlags,
        const char * description,
        const char * filename,
        int32_t line_number) XR_OVERRIDE;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Free function
    // ------------------------------------------------------------------------------------  MEMBER
    void   Free(XR_OUT_INVALIDATE void * ptr, size_t size, const char * filename, int32_t line_number) XR_OVERRIDE;

private:
};

// --------------------------------------------------------------------------------------  FUNCTION
/// Alloc Function
// --------------------------------------------------------------------------------------  FUNCTION
#if defined(XR_COMPILER_MICROSOFT)
// Supress bogus potential null return warning.
#pragma warning(disable: 28183)
#pragma warning(disable: 6387)
#pragma warning(disable: 28196)
#endif

XR_RETURN_BYTES(size)
void * AllocatorCLib::Alloc(size_t size, size_t alignment, Core::MemFlags,
    const char * ,
    const char * ,
    int32_t )
{
    XR_ASSERT_DEBUG_NE(alignment, 0);
    void * p;
#if defined(XR_PLATFORM_WINDOWS)
    p = _aligned_malloc(size, alignment);
#elif defined(XR_PLATFORM_DARWIN)
    // Cannot support this without manually padding each allocation
    // can resolve by using dlmalloc as backing.
    // Note that alignment > 16 is not generally necessary on OSX.
    XR_EXPECT_ALWAYS_TRUE(alignment <= 16);
    p = malloc(size);
#elif defined(XR_PLATFORM_LINUX)
    p = memalign(alignment, size);
#else
#error "What to do?"
    // Should probably sort something out for this platform.
    XR_ALWAYS_UNEXPECTED();
    XR_UNUSED(alignment);
    p = malloc(size);
#endif
    XR_ASSERT_ALWAYS_NE(p, nullptr);
    return p;
}
// --------------------------------------------------------------------------------------  FUNCTION
/// Free function
// --------------------------------------------------------------------------------------  FUNCTION
void   AllocatorCLib::Free(XR_OUT_INVALIDATE void * ptr, size_t , const char * , int32_t )
{
#if defined(XR_PLATFORM_WINDOWS)
    _aligned_free(ptr);
#elif defined(XR_PLATFORM_DARWIN)
    free(ptr);
#elif defined(XR_PLATFORM_LINUX)
    free(ptr);
#else
#error "what to do?"
    free(ptr);
#endif
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
/// \internal
/// Need to make sure this allocator is initialized quite early to prevent
/// init order issues.
#if defined(XR_COMPILER_MICROSOFT)
__pragma(warning(push))
__pragma(warning(disable: 4075)) //  initializers put in unrecognized initialization area
#pragma init_seg( ".CRT$XCB" )
static AllocatorCLib sCLibAllocator;
__pragma(warning(pop))
#elif defined(XR_COMPILER_GCC)
static AllocatorCLib __attribute__((init_priority (101))) sCLibAllocator;
#else
#error "This needs to initialize first on most systems"
static AllocatorCLib sCLibAllocator;
#endif

Core::IAllocator * Core::detail::sGeneralAllocator = &sCLibAllocator;

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_RESTRICT_RETURN wchar_t * StrDupHelper(const wchar_t * source, IAllocator & allocator, MemFlags eFlags, const char * name, const char * filename, int32_t line_number)
{
    size_t numCharsplusNull = StringLengthWithNull(source);
    void * allocated = allocator.Alloc(numCharsplusNull * sizeof(source[0]), XR_ALIGN_OF(wchar_t), eFlags, name, filename, line_number);
    // Use memcpy instead of strcpy since se already know the size.
    memcpy(allocated, source, numCharsplusNull * sizeof(source[0]));
    // cast / return.
    return reinterpret_cast<wchar_t*>(allocated);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_RESTRICT_RETURN char * StrDupHelper(const char * source, IAllocator & allocator, MemFlags eFlags, const char * name, const char * filename, int32_t line_number)
{
    size_t numCharsplusNull = StringLengthWithNull(source);
    void * allocated = allocator.Alloc(numCharsplusNull * sizeof(source[0]), XR_ALIGN_OF(char), eFlags, name, filename, line_number);
    // Use memcpy instead of strcpy since se already know the size.
    memcpy(allocated, source, numCharsplusNull * sizeof(source[0]));
    // cast / return.
    return reinterpret_cast<char*>(allocated);
}

}} // namespace

// --------------------------------------------------------------------------------------  FUNCTION
// Custom new operators (can't customize delete this way)
// --------------------------------------------------------------------------------------  FUNCTION
#if defined(XR_COMPILER_MICROSOFT)
_Ret_bytecap_(size)
#endif
void* operator new (size_t size, size_t alignment, ::xr::Core::MemFlags eFlags, const char * description, const char * filename, int32_t line_number) throw()
{
    return ::xr::Core::GetGeneralAllocator().Alloc(size, alignment, eFlags, description, filename, line_number);
}
#if defined(XR_COMPILER_MICROSOFT)
_Ret_bytecap_(size)
#endif
void* operator new [] (size_t size, size_t alignment, ::xr::Core::MemFlags eFlags, const char * description, const char * filename, int32_t line_number) throw()
{
    return ::xr::Core::GetGeneralAllocator().Alloc(size, alignment, eFlags, description, filename, line_number);
}
// --------------------------------------------------------------------------------------  FUNCTION
// Custom delete operators
// This is called only if the constructor throws an exceptions.
// Even though we have exceptions disabled, this will give you linker errors on some platforms if not present.
// --------------------------------------------------------------------------------------  FUNCTION
void operator delete (void * ptr, size_t size, ::xr::Core::MemFlags, const char * , const char * filename, int32_t line_number) throw()
{
    // This is called only if the constructor throws an exceptions.
    // Even though we have exceptions disabled, this will give you linker errors on some platforms if not present.
    XR_ALWAYS_UNEXPECTED();
    ::xr::Core::GetGeneralAllocator().Free(ptr, size, filename, line_number);
}
void operator delete [] (void * ptr, size_t size, ::xr::Core::MemFlags, const char * , const char * filename, int32_t line_number ) throw()
{
    // This is called only if the constructor throws an exceptions.
    // Even though we have exceptions disabled, this will give you linker errors on some platforms if not present.
    XR_ALWAYS_UNEXPECTED();
    ::xr::Core::GetGeneralAllocator().Free(ptr, size, filename, line_number);
}
// --------------------------------------------------------------------------------------  FUNCTION
// Standard new / delete operators
// --------------------------------------------------------------------------------------  FUNCTION
#if defined(XR_COMPILER_MICROSOFT)
#if _MSC_VER >= 1700
_Use_decl_annotations_
#endif
#endif
void* operator new (size_t size) throw()
{
    return ::xr::Core::GetGeneralAllocator().Alloc(size, 0, ::xr::Core::kMemNormal, nullptr, nullptr, 0);
}
#if defined(XR_COMPILER_MICROSOFT)
#if _MSC_VER >= 1700
_Use_decl_annotations_
#endif
#endif
void* operator new [] (size_t size) throw()
{
    return ::xr::Core::GetGeneralAllocator().Alloc(size, 0, ::xr::Core::kMemNormal, nullptr, nullptr, 0);
}

void operator delete (void * ptr) throw()
{
    // No file name or line number possible on this call.
    ::xr::Core::GetGeneralAllocator().Free(ptr, 0, nullptr, 0);
}
void operator delete [](void * ptr) throw()
{
    // No file name or line number possible on this call.
    ::xr::Core::GetGeneralAllocator().Free(ptr, 0, nullptr, 0);
}
#if defined(XR_COMPILER_MICROSOFT)
_Use_decl_annotations_
#endif
void* operator new (size_t size, const std::nothrow_t &) throw()
{
    return ::xr::Core::GetGeneralAllocator().Alloc(size, 0, ::xr::Core::kMemNormal, nullptr, nullptr, 0);
}
#if defined(XR_COMPILER_MICROSOFT)
_Use_decl_annotations_
#endif
void* operator new [] (size_t size, const std::nothrow_t &) throw()
{
    return ::xr::Core::GetGeneralAllocator().Alloc(size, 0, ::xr::Core::kMemNormal, nullptr, nullptr, 0);
}
void operator delete (void * ptr, const std::nothrow_t &) throw()
{
    // No file name or line number possible on this call.
    ::xr::Core::GetGeneralAllocator().Free(ptr, 0, nullptr, 0);
}
void operator delete [](void * ptr, const std::nothrow_t &) throw()
{
    // No file name or line number possible on this call.
    ::xr::Core::GetGeneralAllocator().Free(ptr, 0, nullptr, 0);
}


// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
