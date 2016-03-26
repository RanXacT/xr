// ######################################################################################### - FILE
/*! \file
    Basic CRC32 implementation. Not for secure usage!

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_HASH_CRC_H
#define XR_CORE_HASH_CRC_H

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
class CRC32
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    static const size_t kDigestSizeInBits  = 32;
    static const size_t kDigestSizeInBytes = kDigestSizeInBits/8;

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    CRC32();

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
    typedef uint32_t Digest;
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    void GetResult(Digest *d);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Reset the internal state to operate on a new buffer
    // ------------------------------------------------------------------------------------  MEMBER
    void Reset();

    // ------------------------------------------------------------------------------------  MEMBER
    /// Generate for passed string (convenience function).
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_SIDE_EFFECTS_PREFIX
    static uint32_t Process(XR_IN_STRING const char * s) XR_NO_SIDE_EFFECTS_POSTFIX;

    // ------------------------------------------------------------------------------------  MEMBER
    /// Generate for passed string (convenience function).
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_SIDE_EFFECTS_PREFIX
    static uint32_t ProcessAppend(uint32_t previousCRC, const char * append) XR_NO_SIDE_EFFECTS_POSTFIX;

private:
    static const uint32_t crc_table[256];
    uint32_t mState;
    bool     mFinalized;
};

}}
#endif //#ifndef XR_CORE_HASH_CRC_H