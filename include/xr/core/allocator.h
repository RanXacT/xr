// ######################################################################################### - FILE
/*! 
\page corealloc Allocation System Overview

Simple general allocator. This provides an allocation interface to a 
potentially much more complex allocation system.

The default core allocator uses the standard C++ allocator. Thus 
without any initialization the allocation system will work naturally. The 
benefit is that the back end can be easily made much more intelligent.

\code
MyClass * bar = XR_NEW( "name") MyClass();
void    * foo = XR_ALLOC(1024, "bar");
XR_FREE(foo);
XR_DELETE(bar);
\endcode

\section  extensibility Extensibility
The system is intended to have a back end which segments allocations by size
(or potentially by name). For the most efficiency, small blocks should have a 
dedicated allocator separate from larger allocations. 


\note XR_NEW uses the default system alignment, not the alignment of the 
created class. Use XR_NEW_ALIGN to create with non-standard alignments.

\note global new / delete operators actually work normally, but lack file / line 
debug information, XR_NEW its is used internally, but use is not 
required at application level.

\note XR_DELETE uses standard delete since the delete operator cannot be 
overridden (actually it can, but doing so changes it's behavior). 
standard delete is the only safe way to delete multiply inherited items.
This means delete calls do not log debug information.


\file
\copydoc corealloc



\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_ALLOCATOR_H
#define XR_CORE_ALLOCATOR_H

#if defined( _MSC_VER ) 
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

#include <type_traits>

#ifndef XR_CORE_UNKNOWN_H
#include "xr/core/unknown.h"
#endif

// For placement new
#include <new>
// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// -----------------------------------------------------------------------------------------  MACRO
//@{
/*! Alloc memory and use placement new to initialize an object (calls constructors)
 Usage:
 \code
// For a class with standard alignment (aligned to a pointer sized boundary)
MyClass * bar = XR_NEW( "bar") MyClass();
bar->Foo();
XR_DELETE(bar);

// For a class with a non-standard alignment (greater than pointer size alignment), do this:
MyClass * bar = XR_NEW_ALIGN( "bar", XR_ALIGN_OF(MyClass) ) MyClass();
bar->Foo();
XR_DELETE(bar);

// For an array, the syntax is the same to create, but be sure to call XR_DELETE_ARRAY
MyClass * bar = XR_NEW( "bar") MyClass [5];
bar[2].Foo();;
XR_DELETE_ARRAY(bar); // Note that one most call XR_DELETE_ARRAY

 \endcode
 */
// -----------------------------------------------------------------------------------------  MACRO
#define XR_NEW(             name)                   new (size_t(XR_PLATFORM_PTR_SIZE)        , ::xr::Core::kMemNormal, name, XR_FILE_LINE)
#define XR_NEW_ALIGN(       name, alignment)        new (size_t(alignment), ::xr::Core::kMemNormal, name, XR_FILE_LINE)
#define XR_NEW_FLAGS(       name, flags    )        new (size_t(XR_PLATFORM_PTR_SIZE)        , flags                 , name, XR_FILE_LINE)
#define XR_NEW_ALIGN_FLAGS( name, alignment, flags) new (size_t(alignment), flags                 , name, XR_FILE_LINE)
//@}                                                                          
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/*! \brief Map through normal delete operator (cannot overload it)  */
#define XR_DELETE(       object)      delete object, object = nullptr
#define XR_DELETE_ARRAY( object )     delete [] object, object = nullptr
//@}

// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/*! Alloc memory and return it. (does not call any constructor on memory)
 \code
    xr::Memory::CachedAllocationChannel h("foo");
    size_t chars = 500;
    char_t bar[] = (char*)XR_ALLOC( chars * sizeof(char), "bar");
    bar[0] = '\0';
    XR_FREE(bar);
 \endcode
 */
#define XR_ALLOC(             size, name)                     ::xr::Core::GetGeneralAllocator().Alloc(size, XR_PLATFORM_PTR_SIZE, ::xr::Core::kMemNormal, name, XR_FILE_LINE)
#define XR_ALLOC_ALIGN(       size, name, alignment)          ::xr::Core::GetGeneralAllocator().Alloc(size, alignment,            ::xr::Core::kMemNormal, name, XR_FILE_LINE)
#define XR_ALLOC_FLAGS(       size, name, flags)              ::xr::Core::GetGeneralAllocator().Alloc(size, XR_PLATFORM_PTR_SIZE, flags,                  name, XR_FILE_LINE)
#define XR_ALLOC_ALIGN_FLAGS( size, name, alignment, flags)   ::xr::Core::GetGeneralAllocator().Alloc(size, alignment,            flags,                  name, XR_FILE_LINE)
//@}
// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/*! replacement for strdup function using our internal allocator.
 */
#define XR_STRDUP(       str, name)                     ::xr::Core::StrDupHelper(str, ::xr::Core::GetGeneralAllocator(), ::xr::Core::kMemNormal, name, XR_FILE_LINE)
#define XR_STRDUP_FLAGS( str, name, flags)              ::xr::Core::StrDupHelper(str, ::xr::Core::GetGeneralAllocator(),                  flags, name, XR_FILE_LINE)
//@}


// -----------------------------------------------------------------------------------------  MACRO
// -----------------------------------------------------------------------------------------  MACRO
//@{
/*! \brief free macros do not call the destructors on deleted objects.   */
#define XR_FREE(         ptr)                ::xr::Core::GetGeneralAllocator().Free(ptr,    0, XR_FILE_LINE)
#define XR_FREE_SZ(      ptr, size)          ::xr::Core::GetGeneralAllocator().Free(ptr, size, XR_FILE_LINE)
//@}
// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {
class IAllocator;

namespace detail {
    /// \internal 
    /// This is declared in the header to allow GetGeneralAllocator to be
    /// inlined, but should not be accessed directly.
    extern IAllocator * sGeneralAllocator;
}
}} 
// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/*! Clues to the allocator to help it place the allocation accordingly. */
// ***************************************************************************************** - TYPE
enum MemFlags
{
    kMemNormal,     ///< Memory is of normal lifetime and will be help onto for an unknown amount of time (default)
    kMemTemporary,  ///< Memory is of short lifetime and will be free'd soon.
    kMemPermanent,  ///< Memory is probably not going to be freed until shutdown
};

// ***************************************************************************************** - TYPE
/*! \brief Interface all allocators must implement */
// ***************************************************************************************** - TYPE
class XR_PURE_INTERFACE IAllocator : public xr::COM::IUnknown{
public:
    XR_COM_CLASS_ID(0x36460ce);

    /*---------------------------------------------------------------------------------- FUNCTION*/
    /*---------------------------------------------------------------------------------- FUNCTION*/
    inline IAllocator(){}
    /*---------------------------------------------------------------------------------- FUNCTION*/
    /*---------------------------------------------------------------------------------- FUNCTION*/
    virtual inline ~IAllocator() {}
    /*---------------------------------------------------------------------------------- FUNCTION*/
    /// Alloc Function
    /*---------------------------------------------------------------------------------- FUNCTION*/
    XR_RETURN_BYTES(size) 
    XR_RESTRICT_RETURN virtual void * Alloc(
        size_t size, 
        size_t alignment, 
        MemFlags eFlags, 
        const char * description, 
        const char * filename, 
        int32_t line_number) = 0;
    /*---------------------------------------------------------------------------------- FUNCTION*/
    /// Free function
    /*---------------------------------------------------------------------------------- FUNCTION*/
    virtual void   Free(XR_OUT_INVALIDATE void * ptr, size_t size, const char * filename, int32_t line_number) = 0;
    /*---------------------------------------------------------------------------------- FUNCTION*/
    /// For debugging purposes.
    /*---------------------------------------------------------------------------------- FUNCTION*/
    virtual const char * GetName() const = 0;
};

// ***************************************************************************************** - TYPE
/*! \brief Optional Validation interface for allocators */
// ***************************************************************************************** - TYPE
class XR_PURE_INTERFACE IAllocatorValidation : public xr::COM::IUnknown{
public:
    XR_COM_CLASS_ID(0xf5c673b5);

    /*---------------------------------------------------------------------------------- FUNCTION*/
    /*---------------------------------------------------------------------------------- FUNCTION*/
    inline IAllocatorValidation(){}
    /*---------------------------------------------------------------------------------- FUNCTION*/
    /*---------------------------------------------------------------------------------- FUNCTION*/
    virtual inline ~IAllocatorValidation() {}

    /*---------------------------------------------------------------------------------- FUNCTION*/
    /// This validates the heap and breaks immediately if an issue 
    /// is detected (no need for a return variable).
    /*---------------------------------------------------------------------------------- FUNCTION*/
    virtual void Validate() = 0;
};

// ***************************************************************************************** - TYPE
/*! \brief Optional Validation interface for allocators */
// ***************************************************************************************** - TYPE
class XR_PURE_INTERFACE IAllocatorPrint : public xr::COM::IUnknown{
public:
    XR_COM_CLASS_ID(0xf5c673b5);

    /*---------------------------------------------------------------------------------- FUNCTION*/
    /*---------------------------------------------------------------------------------- FUNCTION*/
    inline IAllocatorPrint(){}
    /*---------------------------------------------------------------------------------- FUNCTION*/
    /*---------------------------------------------------------------------------------- FUNCTION*/
    virtual inline ~IAllocatorPrint() {}

    /*---------------------------------------------------------------------------------- FUNCTION*/
    /// Verbosity definition is left up to the implementation
    /// passed value should be easy to understand though.
    /*---------------------------------------------------------------------------------- FUNCTION*/
    enum Verbosity{
        kVerbosityMinimum,
        kVerbosityNormal,
        kVerbosityMaximum
    };

    /*---------------------------------------------------------------------------------- FUNCTION*/
    /// This prints the contents of the heap
    /*---------------------------------------------------------------------------------- FUNCTION*/
    virtual void PrintHeap(Verbosity) = 0;

    /*---------------------------------------------------------------------------------- FUNCTION*/
    /// This prints heap statistics
    /*---------------------------------------------------------------------------------- FUNCTION*/
    virtual void PrintStats(Verbosity) = 0;
};

// --------------------------------------------------------------------------------------  FUNCTION
/*! Sets the currently registered general allocator */
// --------------------------------------------------------------------------------------  FUNCTION
inline IAllocator & SetGeneralAllocator(IAllocator & newAllocator)
{
    IAllocator & temp = *detail::sGeneralAllocator; 
    detail::sGeneralAllocator = &newAllocator; 
    return temp;
}
// --------------------------------------------------------------------------------------  FUNCTION
/// return the currently registered general allocator
// --------------------------------------------------------------------------------------  FUNCTION
inline IAllocator & GetGeneralAllocator()
{ 
    return *detail::sGeneralAllocator;
}

// --------------------------------------------------------------------------------------  FUNCTION
/*! used internally behind \ref XR_STRDUP macro*/
// --------------------------------------------------------------------------------------  FUNCTION
XR_RESTRICT_RETURN char * StrDupHelper(const char * source, IAllocator & allocator, MemFlags eFlags, const char * name, const char * filename, int32_t line_number);
// --------------------------------------------------------------------------------------  FUNCTION
/*! used internally behind \ref XR_STRDUP macro*/
// --------------------------------------------------------------------------------------  FUNCTION
XR_RESTRICT_RETURN wchar_t * StrDupHelper(const wchar_t * source, IAllocator & allocator, MemFlags eFlags, const char * name, const char * filename, int32_t line_number);
    
}} // namespace xr::Core

// --------------------------------------------------------------------------------------  FUNCTION
// custom new operators need to be in global scope
// --------------------------------------------------------------------------------------  FUNCTION
//@{
/// Internally used operators
#if defined(XR_COMPILER_MICROSOFT)
_Ret_bytecap_(size)
#endif
void* operator new (size_t size, size_t alignment, ::xr::Core::MemFlags eFlags, const char * description, const char * filename, int32_t line_number) throw ();

#if defined(XR_COMPILER_MICROSOFT)
_Ret_bytecap_(size)
#endif
void* operator new [] (size_t size, size_t alignment, ::xr::Core::MemFlags eFlags, const char * description, const char * filename, int32_t line_number) throw ();
//@}
// --------------------------------------------------------------------------------------  FUNCTION
// custom delete operators need to be in global scope
// --------------------------------------------------------------------------------------  FUNCTION
//@{
/// This function exists only to pacify the compiler. C++ expects a matching delete for use if
/// the constructor in a "new" operation throws an exception for the purposes of automatically deleting
/// that newly created memory, but we only build with exceptions off. Although if exceptions were ever needed
/// this would function properly.
void operator delete (void * ptr, size_t , ::xr::Core::MemFlags, const char * , const char * , int32_t ) throw();
void operator delete [](void * ptr, size_t , ::xr::Core::MemFlags, const char * , const char * , int32_t ) throw();
//@}

#endif //#ifndef XR_CORE_ALLOCATOR_H
