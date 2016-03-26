// ######################################################################################### - FILE
/*! \file

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_ALLOCATORS_LINEAR_H
#define XR_CORE_ALLOCATORS_LINEAR_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif

#ifndef XR_CORE_BIT_UTILS_H
#include "xr/core/bit_utils.h"
#endif

/* Public Macros */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
struct LinearAllocatorParameters{
    const char    * mName;                  ///< Name of allocator, returned by GetName() call.
    IAllocator    * mMetadataAllocator;     ///< Allocator used for allocator overhead and metadata.
    IAllocator    * mBucketAllocator;       ///< Allocator to get buckets from. (can be same as mMetadataAllocator)
    size_t          mMainBucketSize;        ///< Size of the initial bucket.
    size_t          mOverflowBucketSize;    ///< Size of each additional bucket as needed. (0 means do not overflow)
    size_t          mBucketAlignment;       ///< Minimum alignment of a bucket.
};
// ***************************************************************************************** - TYPE
/*! This is a simple Thread Safe linear allocator. There is a faster
    non-thread safe allocator below with an inlined alloc function for
    less opaque and more time critical uses.
*/
// ***************************************************************************************** - TYPE
class ILinearAllocator : public xr::Core::IAllocator
{
public:
    XR_COM_CLASS_ID(0x5b2619a);

    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    inline ILinearAllocator(){}
    /*---------------------------------------------------------------*/
    /*---------------------------------------------------------------*/
    virtual inline ~ILinearAllocator() {}
    /*---------------------------------------------------------------*/
    /// This resets the linear allocator to the beginning.
    /// Clear should only be called at a time where there is no
    /// possibility of thread contention. This function is not thread
    /// safe.
    /*---------------------------------------------------------------*/
    virtual size_t Clear() = 0;
    /*---------------------------------------------------------------*/
    /// This resets the linear allocator to the beginning.
    /*---------------------------------------------------------------*/
    virtual size_t GetHighwaterMark() = 0;
};


/// Note that created linear allocators (as with all allocators) are
/// not reference counted, and should be explicitly destroyed.
class ILinearAllocator * LinearAllocatorCreate(const LinearAllocatorParameters & allocatorParams);

// ***************************************************************************************** - TYPE
/*! Implements a non-thread-safe linear allocator. Just pass in a buffer,
    if the buffer overflows it will allocate in buckets from the heap.

    The passed buffer to be sub allocated is not deallocated, but any
    overflow buckets are deallocated in the "clear" function.

    The high water mark is only tracked in the clear function to
    keep the alloc function lean.

    This is a concrete and inlined implementation for use outside of com applications
    */
// ***************************************************************************************** - TYPE
class LinearAllocator{
public:
    /*-------------------------------------------------------------------*/
    /*-------------------------------------------------------------------*/
    LinearAllocator(const char * name, void * buffer, size_t bufferSizeBytes, size_t overflowBucketSize);

    /*-------------------------------------------------------------------*/
    /*-------------------------------------------------------------------*/
    XR_INLINE
    ~LinearAllocator() {Clear();}

    /*-------------------------------------------------------------------*/
    /// Alloc Function
    /*-------------------------------------------------------------------*/
    XR_INLINE
    XR_RETURN_BYTES(size)
    XR_RESTRICT_RETURN
    void * Alloc( size_t size, size_t alignment = XR_PLATFORM_PTR_SIZE)
    {
        alignment = alignment > XR_PLATFORM_PTR_SIZE ? alignment : XR_PLATFORM_PTR_SIZE;
        uint8_t * const pStart   = xr::Core::AlignPtr(mCurrentBucket->mCurrent, alignment);
        uint8_t * const pEnd     = pStart + size;

        if(pEnd > mCurrentBucket->mEnd)
        {
            return Expand(size, alignment);
        }

        mCurrentBucket->mCurrent = pEnd;
        return pStart;
    }
    /*-------------------------------------------------------------------*/
    /// Wrapper for standard Alloc function
    /*-------------------------------------------------------------------*/
    XR_INLINE
    XR_RETURN_BYTES(size)
    XR_RESTRICT_RETURN
    void * Alloc(
        size_t size,
        size_t alignment,
        MemFlags ,
        const char * ,
        const char * ,
        int32_t )
    {
        return Alloc(size, alignment);
    }
    /*-------------------------------------------------------------------*/
    /// Wrapper for standard Free function
    /*-------------------------------------------------------------------*/
    XR_INLINE
    void   Free(XR_OUT_INVALIDATE void * , size_t , const char * , int32_t )
    {

    }

    /*-------------------------------------------------------------------*/
    /// This resets the linear allocator to the beginning.
    /// This also frees any overflow buckets and returns the number of
    /// bytes *used* in it.
    /*-------------------------------------------------------------------*/
    size_t Clear();

    /*-------------------------------------------------------------------*/
    /// This returns the higher amount of bytes used at the time of a clear.
    /*-------------------------------------------------------------------*/
    XR_FORCE_INLINE
    size_t GetHighwaterMark() {return mHighwaterMark;}

    /*-------------------------------------------------------------------*/
    /// This returns the higher amount of bytes used at the time of a clear.
    /*-------------------------------------------------------------------*/
    XR_FORCE_INLINE
    const char * GetName() {return mName;}

protected:
    /*-------------------------------------------------------------------*/
    /// Expand Function, called when out of space.
    /*-------------------------------------------------------------------*/
    XR_RESTRICT_RETURN void * Expand(
        size_t size,
        size_t alignment);
private:
    struct Bucket{
        uint8_t * mBase;                ///< Pointer to base of region.
        uint8_t * mCurrent;             ///< Pointer to next free byte. Atomically updated
        uint8_t * mEnd;                 ///< Pointer to first non owned byte.
        Bucket  * mNextBucket;          ///< Pointer to the next bucket (or nullptr)

        Bucket(void * buffer, size_t bufferSizeBytes) :
            mBase((uint8_t *)buffer),
            mCurrent((uint8_t *)buffer),
            mEnd(mBase+bufferSizeBytes),
            mNextBucket(nullptr) {}

    };
    Bucket     * mCurrentBucket;       ///< Pointer to Current Bucket
    Bucket       mMainBucket;          ///< This is the first main bucket, i.e. the passed in buffer.
    const size_t kOverflowBucketSize;  ///< This is the size allocated at overflow.
    size_t       mHighwaterMark;       ///< This is the highest "cleared" value. Note that high water is only tracked at clear.
    const char * mName;
    /*-------------------------------------------------------------------*/
    /// Unused assignment operator
    /*-------------------------------------------------------------------*/
    LinearAllocator & operator= (const LinearAllocator & other);
};

}}
#endif //#ifndef XR_CORE_ALLOCATORS_LINEAR_H
