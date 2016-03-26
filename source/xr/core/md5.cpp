// ######################################################################################### - FILE
/*!
    This file contains a slightly modified version of cryptmd5.c which is
    obtained from http://gpl.ea.com. The license for that code is copied below.
    note that it is not in fact gpl, but 3 clause bsd.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/


/*
Copyright (C) 2002,2009-2010 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_HASH_MD5_H
#include "xr/core/hash/md5.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_MEM_UTILS_H
#include "xr/core/mem_utils.h"
#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE
#define FF(x, y, z) (z ^ ((y ^ z) & x))
#define GG(x, y, z) (y ^ ((x ^ y) & z))
#define HH(x, y, z) (x ^ y ^ z)
#define II(x, y, z) (y ^ (x | ~z))

#define ACC(r, t1, t2, t3, s, x) \
    r += (t1); r += (t2); r+= (t3); r = (r<<s) | (r>>(32-s)); r += x;

// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MD5::Transform(const uint8_t *pBlock)
{
    register uint32_t a;
    register uint32_t b;
    register uint32_t c;
    register uint32_t d;
    uint32_t uData[16];
    volatile uint32_t *pVector = nullptr;

    {
        pBlock += 64;
        pVector = uData+16;
        while (pVector != uData)
        {
            b = *--pBlock;
            b = (b << 8) + *--pBlock;
            b = (b << 8) + *--pBlock;
            b = (b << 8) + *--pBlock;
            *--pVector = b;
        }
		// This statement is redundant, but static code testing
		// gets confused and things pVector could overflow due to loop.
		pVector = uData;
    }

    a = mRegs[0];
    b = mRegs[1];
    c = mRegs[2];
    d = mRegs[3];

    ACC(a, FF(b, c, d), pVector[ 0],0xd76aa478,  7, b);
    ACC(d, FF(a, b, c), pVector[ 1],0xe8c7b756, 12, a);
    ACC(c, FF(d, a, b), pVector[ 2],0x242070db, 17, d);
    ACC(b, FF(c, d, a), pVector[ 3],0xc1bdceee, 22, c);
    ACC(a, FF(b, c, d), pVector[ 4],0xf57c0faf,  7, b);
    ACC(d, FF(a, b, c), pVector[ 5],0x4787c62a, 12, a);
    ACC(c, FF(d, a, b), pVector[ 6],0xa8304613, 17, d);
    ACC(b, FF(c, d, a), pVector[ 7],0xfd469501, 22, c);
    ACC(a, FF(b, c, d), pVector[ 8],0x698098d8,  7, b);
    ACC(d, FF(a, b, c), pVector[ 9],0x8b44f7af, 12, a);
    ACC(c, FF(d, a, b), pVector[10],0xffff5bb1, 17, d);
    ACC(b, FF(c, d, a), pVector[11],0x895cd7be, 22, c);
    ACC(a, FF(b, c, d), pVector[12],0x6b901122,  7, b);
    ACC(d, FF(a, b, c), pVector[13],0xfd987193, 12, a);
    ACC(c, FF(d, a, b), pVector[14],0xa679438e, 17, d);
    ACC(b, FF(c, d, a), pVector[15],0x49b40821, 22, c);

    ACC(a, GG(b, c, d), pVector[ 1],0xf61e2562,  5, b);
    ACC(d, GG(a, b, c), pVector[ 6],0xc040b340,  9, a);
    ACC(c, GG(d, a, b), pVector[11],0x265e5a51, 14, d);
    ACC(b, GG(c, d, a), pVector[ 0],0xe9b6c7aa, 20, c);
    ACC(a, GG(b, c, d), pVector[ 5],0xd62f105d,  5, b);
    ACC(d, GG(a, b, c), pVector[10],0x02441453,  9, a);
    ACC(c, GG(d, a, b), pVector[15],0xd8a1e681, 14, d);
    ACC(b, GG(c, d, a), pVector[ 4],0xe7d3fbc8, 20, c);
    ACC(a, GG(b, c, d), pVector[ 9],0x21e1cde6,  5, b);
    ACC(d, GG(a, b, c), pVector[14],0xc33707d6,  9, a);
    ACC(c, GG(d, a, b), pVector[ 3],0xf4d50d87, 14, d);
    ACC(b, GG(c, d, a), pVector[ 8],0x455a14ed, 20, c);
    ACC(a, GG(b, c, d), pVector[13],0xa9e3e905,  5, b);
    ACC(d, GG(a, b, c), pVector[ 2],0xfcefa3f8,  9, a);
    ACC(c, GG(d, a, b), pVector[ 7],0x676f02d9, 14, d);
    ACC(b, GG(c, d, a), pVector[12],0x8d2a4c8a, 20, c);

    ACC(a, HH(b, c, d), pVector[ 5],0xfffa3942,  4, b);
    ACC(d, HH(a, b, c), pVector[ 8],0x8771f681, 11, a);
    ACC(c, HH(d, a, b), pVector[11],0x6d9d6122, 16, d);
    ACC(b, HH(c, d, a), pVector[14],0xfde5380c, 23, c);
    ACC(a, HH(b, c, d), pVector[ 1],0xa4beea44,  4, b);
    ACC(d, HH(a, b, c), pVector[ 4],0x4bdecfa9, 11, a);
    ACC(c, HH(d, a, b), pVector[ 7],0xf6bb4b60, 16, d);
    ACC(b, HH(c, d, a), pVector[10],0xbebfbc70, 23, c);
    ACC(a, HH(b, c, d), pVector[13],0x289b7ec6,  4, b);
    ACC(d, HH(a, b, c), pVector[ 0],0xeaa127fa, 11, a);
    ACC(c, HH(d, a, b), pVector[ 3],0xd4ef3085, 16, d);
    ACC(b, HH(c, d, a), pVector[ 6],0x04881d05, 23, c);
    ACC(a, HH(b, c, d), pVector[ 9],0xd9d4d039,  4, b);
    ACC(d, HH(a, b, c), pVector[12],0xe6db99e5, 11, a);
    ACC(c, HH(d, a, b), pVector[15],0x1fa27cf8, 16, d);
    ACC(b, HH(c, d, a), pVector[ 2],0xc4ac5665, 23, c);

    ACC(a, II(b, c, d), pVector[ 0],0xf4292244,  6, b);
    ACC(d, II(a, b, c), pVector[ 7],0x432aff97, 10, a);
    ACC(c, II(d, a, b), pVector[14],0xab9423a7, 15, d);
    ACC(b, II(c, d, a), pVector[ 5],0xfc93a039, 21, c);
    ACC(a, II(b, c, d), pVector[12],0x655b59c3,  6, b);
    ACC(d, II(a, b, c), pVector[ 3],0x8f0ccc92, 10, a);
    ACC(c, II(d, a, b), pVector[10],0xffeff47d, 15, d);
    ACC(b, II(c, d, a), pVector[ 1],0x85845dd1, 21, c);
    ACC(a, II(b, c, d), pVector[ 8],0x6fa87e4f,  6, b);
    ACC(d, II(a, b, c), pVector[15],0xfe2ce6e0, 10, a);
    ACC(c, II(d, a, b), pVector[ 6],0xa3014314, 15, d);
    ACC(b, II(c, d, a), pVector[13],0x4e0811a1, 21, c);
    ACC(a, II(b, c, d), pVector[ 4],0xf7537e82,  6, b);
    ACC(d, II(a, b, c), pVector[11],0xbd3af235, 10, a);
    ACC(c, II(d, a, b), pVector[ 2],0x2ad7d2bb, 15, d);
    ACC(b, II(c, d, a), pVector[ 9],0xeb86d391, 21, c);

    mRegs[0] += a;
    mRegs[1] += b;
    mRegs[2] += c;
    mRegs[3] += d;
}


/*** Public Functions ******************************************************************/


MD5::MD5()
{
    Reset();
}

void MD5::Reset()
{
    mFinalized = false;
    mCount = 0;

    mRegs[0] = 0x67452301;
    mRegs[1] = 0xefcdab89;
    mRegs[2] = 0x98badcfe;
    mRegs[3] = 0x10325476;
}

void MD5::Append(XR_IN_BYTES(sizeInBytes) const void *_pBuffer, size_t sizeInBytes)
{
    XR_ASSERT_ALWAYS_FALSE(mFinalized);

    size_t uAdd;
    uint32_t uCount;
    const unsigned char *pBuffer = (const unsigned char *)_pBuffer;

    uCount = mCount&63;
    mCount += sizeInBytes;

    if (uCount > 0)
    {
        uAdd = 64-uCount;

        if (sizeInBytes < uAdd)
        {
            MemCopy8(mData+uCount, pBuffer, sizeInBytes);
            return;
        }

        MemCopy8(mData+uCount, pBuffer, uAdd);
        pBuffer += uAdd;
        sizeInBytes -= uAdd;
        Transform(mData);
    }

    while (sizeInBytes >= 64)
    {
        Transform(pBuffer);
        pBuffer += 64;
        sizeInBytes -= 64;
    }

    if (sizeInBytes > 0)
    {
        MemCopy8(mData, pBuffer, sizeInBytes);
    }
}

void MD5::Finalize()
{
    XR_ASSERT_ALWAYS_FALSE(mFinalized);

    size_t uIndex;
    size_t uZero;
    uint32_t *pZero;

    uIndex = mCount & 63;
    mData[uIndex++] = 0x80;

    if (uIndex > 56)
    {
        mData[uIndex+0] = 0;
        mData[uIndex+1] = 0;
        mData[uIndex+2] = 0;
        mData[uIndex+3] = 0;
        mData[uIndex+4] = 0;
        mData[uIndex+5] = 0;
        mData[uIndex+6] = 0;
        mData[uIndex+7] = 0;

        Transform(mData);
        uIndex = 0;
    }

    mData[uIndex+0] = 0;
    mData[uIndex+1] = 0;
    mData[uIndex+2] = 0;

    uZero = (uIndex+3)>>2;
    pZero = ((uint32_t *)mData)+uZero;
    do
    {
        *pZero++ = 0;
    }
    while (++uZero < 64/4);

    mData[56] = (unsigned char)(mCount<<3);
    mData[57] = (unsigned char)(mCount>>5);
    mData[58] = (unsigned char)(mCount>>13);
    mData[59] = (unsigned char)(mCount>>21);
    mData[60] = (unsigned char)(mCount>>29);

    Transform(mData);

    mFinalized = true;
}

void MD5::GetResult(Digest *d)
{
    XR_ASSERT_ALWAYS_TRUE(mFinalized);
    size_t uIndex;
    uint32_t uData = 0;

    for (uIndex = 0; uIndex < 16; ++uIndex)
    {
        if ((uIndex & 3) == 0)
        {
            uData = mRegs[uIndex>>2];
        }

        d->result[uIndex] = (unsigned char)uData;

        uData >>= 8;
    }
}

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
