// ######################################################################################### - FILE
/*! \file

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_HASH_MD5_H
#define XR_CORE_HASH_MD5_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

// ######################################################################################### - FILE
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
/*!
    */
// ***************************************************************************************** - TYPE
class MD5
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    static const size_t kDigestSizeInBits  = 128;
    static const size_t kDigestSizeInBytes = kDigestSizeInBits/8;
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    MD5();

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    void Append(XR_IN_BYTES(sizeInBytes) const void * buffer, size_t sizeInBytes);
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    void Finalize();
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    struct Digest
    {
        uint8_t result[kDigestSizeInBytes];
    };
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    void GetResult(Digest *d);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Reset the internal state to operate on a new buffer
    // ------------------------------------------------------------------------------------  MEMBER
    void Reset();
private:
    void Transform(const uint8_t *pBlock);

    uint8_t  mData[64+8];
    size_t   mCount;
    uint32_t mRegs[4];
    bool     mFinalized;
};
}}
#endif //#ifndef XR_CORE_HASH_MD5_H
