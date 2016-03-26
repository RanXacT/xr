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
#ifndef XR_CORE_ALLOCATORS_LINEAR_H
#include "xr/core/allocators/linear.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_BIT_UTILS_H
#include "xr/core/bit_utils.h"
#endif
#ifndef XR_CORE_THREADING_ATOMIC_H
#include "xr/core/threading/atomic.h"
#endif
#ifndef XR_CORE_THREADING_MUTEX_H
#include "xr/core/threading/mutex.h"
#endif
#include <algorithm>    // std::max
// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/*! Implements a lockless linear allocator. Note that a lock is still used
    to prevent contention during expansion. Also note that the allocator
    may track the thread ID
    */
// ***************************************************************************************** - TYPE
class LinearAllocatorThreadSafeCOM : public ILinearAllocator {
public:
    // ------------------------------------------------------------------------------------  MEMBER
    // COM Boilerplate in all it's glory:
    // Can't use helper class because of inheritance hierarchy.
    // ------------------------------------------------------------------------------------  MEMBER
    void AddRef() const XR_OVERRIDE {}
    void Release() const XR_OVERRIDE {}
    size_t    UnsafeGetRefCount() const { return XR_SIZE_MAX; }
    xr::COM::IUnknown * QueryInterface(::xr::COM::InterfaceID id) XR_OVERRIDE
    {
        if( id == xr::COM::IUnknown::kIID ||
            id == ILinearAllocator::kIID ||
            id == IAllocator::kIID)
        {
            this->AddRef();
            return this;
        }
        return nullptr;
    }

    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    LinearAllocatorThreadSafeCOM(const LinearAllocatorParameters &params)
        : kName(params.mName)
        , kMetadataAllocator(params.mMetadataAllocator==nullptr ? GetGeneralAllocator() : *params.mMetadataAllocator)
        , kBucketAllocator(params.mBucketAllocator==nullptr ? GetGeneralAllocator() : *params.mBucketAllocator)
        , kMainBucketSize(params.mMainBucketSize)
        , kOverflowBucketSize(params.mOverflowBucketSize)
        , kBucketAlignment(params.mBucketAlignment)
        , mHighwaterMark(0)
        , mCurrentBucket(nullptr)
        , mMutex()
    {
        mCurrentBucket              = (Bucket*)kMetadataAllocator.Alloc(sizeof(Bucket),XR_ALIGN_OF(Bucket), kMemNormal, "Bucket", XR_FILE_LINE);
        mCurrentBucket->mBase       = (uint8_t*)kBucketAllocator.Alloc(kMainBucketSize, kBucketAlignment, kMemNormal, "Bucket", XR_FILE_LINE);
        mCurrentBucket->mCurrent    = mCurrentBucket->mBase;
        mCurrentBucket->mEnd        = mCurrentBucket->mBase  + kMainBucketSize;
        mCurrentBucket->mNextBucket = nullptr;
    }
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~LinearAllocatorThreadSafeCOM()
    {
        Clear();
        XR_ASSERT_DEBUG_EQ(mCurrentBucket->mNextBucket, nullptr);
        kBucketAllocator.Free(mCurrentBucket->mBase, 0, XR_FILE_LINE);
        kMetadataAllocator.Free(mCurrentBucket, 0, XR_FILE_LINE);
        mCurrentBucket = nullptr;
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// Alloc Function
    // ------------------------------------------------------------------------------------  MEMBER
    XR_RETURN_BYTES(size)
        XR_RESTRICT_RETURN virtual void * Alloc(
        size_t size,
        size_t alignment,
        MemFlags ,
        const char * ,
        const char * ,
        int32_t ) XR_OVERRIDE
    {
        alignment = alignment > XR_PLATFORM_PTR_SIZE ? alignment : XR_PLATFORM_PTR_SIZE;

        uint8_t * pCurrent = nullptr;
        uint8_t * pstart   = nullptr;
        uint8_t * pend     = nullptr;

        do{
            pCurrent = mCurrentBucket->mCurrent;
            pstart   = AlignPtr(pCurrent, alignment);
            pend     = pstart + size;

            while(pend > mCurrentBucket->mEnd)
            {
                // Overflow
                mMutex.Lock();
                // Make sure not to duplicate the allocation
                if(pCurrent == mCurrentBucket->mCurrent)
                {
                    size_t bucketSize  = std::max(size  + sizeof(Bucket) + sizeof(void*), kOverflowBucketSize);
                    bucketSize         = AlignSize(bucketSize, XR_ALIGN_OF(void*));
                    size_t bucketAlign = std::max(alignment, XR_ALIGN_OF(void*) );

                    Bucket * b = (Bucket *)kMetadataAllocator.Alloc(sizeof(Bucket),XR_ALIGN_OF(Bucket), kMemNormal, "Bucket", XR_FILE_LINE);
                    b->mBase    = (uint8_t*)kBucketAllocator.Alloc(bucketSize, bucketAlign, kMemNormal, "Bucket", XR_FILE_LINE);
                    b->mCurrent = b->mBase + size;
                    b->mEnd     = b->mBase  + bucketSize;
                    b->mNextBucket = nullptr;
                    // Link it in.
                    b->mNextBucket = mCurrentBucket;
                    mCurrentBucket = b;
                    mMutex.Unlock();

                    return b->mBase;
                }
                mMutex.Unlock();

                // Try again, someone has allocated a new bucket,
                pCurrent = mCurrentBucket->mCurrent;
                pstart = AlignPtr(pCurrent, alignment);
                pend   = pstart + size;
            }

            // We have the required space, now attempt to advance the pointer.
            // If this fails, someone else advanced the pointer, begin again.
        }while(pCurrent != AtomicCompareAndSwap<uint8_t *>(&mCurrentBucket->mCurrent,pCurrent,pend));

        return pstart;
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// Free function
    // ------------------------------------------------------------------------------------  MEMBER
    void   Free(XR_OUT_INVALIDATE void * ptr, size_t , const char * , int32_t ) XR_OVERRIDE
    {
        XR_UNUSED(ptr);
        XR_ASSERT_DEBUG_NE(ptr, nullptr);
        // Linear allocator, do nothing!
    }
    // ------------------------------------------------------------------------------------  MEMBER
    /// For debugging purposes.
    // ------------------------------------------------------------------------------------  MEMBER
    virtual const char * GetName() const XR_OVERRIDE
    {
        return kName;
    }

    // ------------------------------------------------------------------------------------  MEMBER
    /// This resets the linear allocator to the beginning.
    // ------------------------------------------------------------------------------------  MEMBER
    size_t Clear() XR_OVERRIDE
    {
        size_t totalBytes = 0;

        mMutex.Lock();
        Bucket * b = mCurrentBucket;
        while(b->mNextBucket != nullptr)
        {
            totalBytes += b->mCurrent - b->mBase;
            mCurrentBucket = b->mNextBucket;
            kBucketAllocator.Free(b->mBase, 0, XR_FILE_LINE);
            kMetadataAllocator.Free(b, 0, XR_FILE_LINE);
            b = mCurrentBucket;
        }
        totalBytes += b->mCurrent - b->mBase;
        mHighwaterMark = std::max(totalBytes, mHighwaterMark);
        b->mCurrent = b->mBase;
        mMutex.Unlock();

        return totalBytes;
    }

    // ------------------------------------------------------------------------------------  MEMBER
    /// This resets the linear allocator to the beginning.
    // ------------------------------------------------------------------------------------  MEMBER
    size_t GetHighwaterMark() XR_OVERRIDE
    {
        return mHighwaterMark;
    }


protected:
private:
    struct Bucket{
        uint8_t * mBase;                ///< Pointer to base of region.
        uint8_t * mCurrent;             ///< Pointer to next free byte. Atomically updated
        uint8_t * mEnd;                 ///< Pointer to first non owned byte.
        Bucket  * mNextBucket;          ///< Pointer to the next bucket (or nullptr)
    };

    const char * const kName;
    IAllocator & kMetadataAllocator;
    IAllocator & kBucketAllocator;
    const size_t kMainBucketSize;
    const size_t kOverflowBucketSize;
    const size_t kBucketAlignment;
    size_t       mHighwaterMark;
    Bucket     * mCurrentBucket;              ///< Pointer to First Bucket
    Mutex        mMutex;                    ///< Mutex, locked only when allocating a new bucket

    // ------------------------------------------------------------------------------------  MEMBER
    /// Unused assignment operator
    // ------------------------------------------------------------------------------------  MEMBER
    LinearAllocatorThreadSafeCOM & operator= (const LinearAllocatorThreadSafeCOM & other);
};


// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
ILinearAllocator * LinearAllocatorCreate(const LinearAllocatorParameters & allocatorParams)
{
    return XR_NEW(allocatorParams.mName) LinearAllocatorThreadSafeCOM(allocatorParams);
}


// ***************************************************************************************** - TYPE
/*  LinearAllocator
    */
// ***************************************************************************************** - TYPE
LinearAllocator::LinearAllocator( const char * name, void * buffer, size_t bufferSizeBytes, size_t overflowBucketSize ) :
    mCurrentBucket(&mMainBucket),
    mMainBucket(buffer, bufferSizeBytes),
    kOverflowBucketSize(overflowBucketSize),
    mHighwaterMark(0),
    mName(name)
{

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t LinearAllocator::Clear()
{
    size_t totalBytes = 0;

    Bucket * b = mCurrentBucket;
    while(b->mNextBucket != nullptr)
    {
        totalBytes += b->mCurrent - b->mBase;
        mCurrentBucket = b->mNextBucket;
        XR_FREE(b->mBase);
        b = mCurrentBucket;
    }
    totalBytes += b->mCurrent - b->mBase;
    mHighwaterMark = std::max(totalBytes, mHighwaterMark);
    b->mCurrent = b->mBase;
    return totalBytes;
}

XR_RESTRICT_RETURN void * LinearAllocator::Expand( size_t allocSize, size_t allocAlign )
{

    size_t bucketSize  = std::max(allocSize  + sizeof(Bucket) + sizeof(void*), kOverflowBucketSize);
    bucketSize         = AlignSize(bucketSize, XR_ALIGN_OF(void*));
    size_t bucketAlign = std::max(allocAlign, XR_ALIGN_OF(void*) );

    uint8_t * start = (uint8_t *)XR_ALLOC_ALIGN(bucketSize, mName, bucketAlign);
    uint8_t * end   = start + bucketSize - sizeof(Bucket);

    Bucket * b = (Bucket *)end;
    b->mBase    = start;
    b->mCurrent = start + allocSize;
    b->mEnd     = end;
    b->mNextBucket = mCurrentBucket;

    mCurrentBucket = b;

    return b->mBase;
}

}}//namespace xr::Core

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
