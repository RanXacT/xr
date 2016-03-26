// ######################################################################################### - FILE
/*!
    This file contains a slightly modified version of sha2 from
    http://www.ouah.org/ogay/sha2/

    Licence Below:

\author (Modifications) Daniel Craig \par Copyright 2016, All Rights reserved.
*/


/*
 * FIPS 180-2 SHA-224/256/384/512 implementation
 * Last update: 02/02/2007
 * Issue date:  04/30/2005
 *
 * Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_HASH_SHA_H
#include "xr/core/hash/sha.h"
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
#if 1
#define UNROLL_LOOPS /* Enable loops unrolling */
#endif

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

#define SHA512_F1(x) (ROTR(x, 28) ^ ROTR(x, 34) ^ ROTR(x, 39))
#define SHA512_F2(x) (ROTR(x, 14) ^ ROTR(x, 18) ^ ROTR(x, 41))
#define SHA512_F3(x) (ROTR(x,  1) ^ ROTR(x,  8) ^ SHFR(x,  7))
#define SHA512_F4(x) (ROTR(x, 19) ^ ROTR(x, 61) ^ SHFR(x,  6))

#define UNPACK32(x, str)                      \
{                                             \
    *((str) + 3) = (uint8_t) ((x)      );       \
    *((str) + 2) = (uint8_t) ((x) >>  8);       \
    *((str) + 1) = (uint8_t) ((x) >> 16);       \
    *((str) + 0) = (uint8_t) ((x) >> 24);       \
}

#define PACK32(str, x)                        \
{                                             \
    *(x) =   ((uint32_t) *((str) + 3)      )    \
    | ((uint32_t) *((str) + 2) <<  8)    \
    | ((uint32_t) *((str) + 1) << 16)    \
    | ((uint32_t) *((str) + 0) << 24);   \
}

#define UNPACK64(x, str)                      \
{                                             \
    *((str) + 7) = (uint8_t) ((x)      );       \
    *((str) + 6) = (uint8_t) ((x) >>  8);       \
    *((str) + 5) = (uint8_t) ((x) >> 16);       \
    *((str) + 4) = (uint8_t) ((x) >> 24);       \
    *((str) + 3) = (uint8_t) ((x) >> 32);       \
    *((str) + 2) = (uint8_t) ((x) >> 40);       \
    *((str) + 1) = (uint8_t) ((x) >> 48);       \
    *((str) + 0) = (uint8_t) ((x) >> 56);       \
}

#define PACK64(str, x)                        \
{                                             \
    *(x) =   ((uint64_t) *((str) + 7)      )    \
    | ((uint64_t) *((str) + 6) <<  8)    \
    | ((uint64_t) *((str) + 5) << 16)    \
    | ((uint64_t) *((str) + 4) << 24)    \
    | ((uint64_t) *((str) + 3) << 32)    \
    | ((uint64_t) *((str) + 2) << 40)    \
    | ((uint64_t) *((str) + 1) << 48)    \
    | ((uint64_t) *((str) + 0) << 56);   \
}

/* Macros used for loops unrolling */

#define SHA256_SCR(i)                         \
{                                             \
    w[i] =  SHA256_F4(w[i -  2]) + w[i -  7]  \
    + SHA256_F3(w[i - 15]) + w[i - 16]; \
}

#define SHA512_SCR(i)                         \
{                                             \
    w[i] =  SHA512_F4(w[i -  2]) + w[i -  7]  \
    + SHA512_F3(w[i - 15]) + w[i - 16]; \
}

#define SHA256_EXP(a, b, c, d, e, f, g, mState, j)               \
{                                                           \
    t1 = wv[mState] + SHA256_F2(wv[e]) + CH(wv[e], wv[f], wv[g]) \
    + SHA1_256::sk[j] + w[j];                              \
    t2 = SHA256_F1(wv[a]) + MAJ(wv[a], wv[b], wv[c]);       \
    wv[d] += t1;                                            \
    wv[mState] = t1 + t2;                                        \
}

#define SHA512_EXP(a, b, c, d, e, f, g ,mState, j)               \
{                                                           \
    t1 = wv[mState] + SHA512_F2(wv[e]) + CH(wv[e], wv[f], wv[g]) \
    + SHA1_512::sk[j] + w[j];                              \
    t2 = SHA512_F1(wv[a]) + MAJ(wv[a], wv[b], wv[c]);       \
    wv[d] += t1;                                            \
    wv[mState] = t1 + t2;                                        \
}

namespace xr { namespace Core {

const uint32_t SHA1_224::sh0[8] =
            {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
             0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};

const uint32_t SHA1_256::sh0[8] =
            {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
             0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

const uint64_t SHA1_384::sh0[8] =
            {0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL,
             0x9159015a3070dd17ULL, 0x152fecd8f70e5939ULL,
             0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL,
             0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL};

const uint64_t SHA1_512::sh0[8] =
            {0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL,
             0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
             0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
             0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL};

const uint32_t SHA1_256::sk[64] =
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

const uint64_t SHA1_512::sk[80] =
            {0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
             0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
             0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
             0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
             0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
             0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
             0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
             0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
             0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
             0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
             0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
             0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
             0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
             0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
             0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
             0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
             0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
             0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
             0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
             0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
             0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
             0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
             0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
             0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
             0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
             0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
             0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
             0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
             0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
             0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
             0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
             0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
             0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
             0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
             0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
             0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
             0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
             0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
             0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
             0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL};

/* SHA-256 functions */

void SHA1_256::Transform(const uint8_t *message, size_t sizeInBytes)
{
    uint32_t w[64];
    uint32_t wv[8];
    uint32_t t1, t2;
    const uint8_t *sub_block;
    size_t i;

#ifndef UNROLL_LOOPS
    size_t j;
#endif

    for (i = 0; i < sizeInBytes; i++) {
        sub_block = message + (i << 6);

#ifndef UNROLL_LOOPS
        for (j = 0; j < 16; j++) {
            PACK32(&sub_block[j << 2], &w[j]);
        }

        for (j = 16; j < 64; j++) {
            SHA256_SCR(j);
        }

        for (j = 0; j < 8; j++) {
            wv[j] = this->mState[j];
        }

        for (j = 0; j < 64; j++) {
            t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6])
                + sk[j] + w[j];
            t2 = SHA256_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (j = 0; j < 8; j++) {
            this->mState[j] += wv[j];
        }
#else
        PACK32(&sub_block[ 0], &w[ 0]); PACK32(&sub_block[ 4], &w[ 1]);
        PACK32(&sub_block[ 8], &w[ 2]); PACK32(&sub_block[12], &w[ 3]);
        PACK32(&sub_block[16], &w[ 4]); PACK32(&sub_block[20], &w[ 5]);
        PACK32(&sub_block[24], &w[ 6]); PACK32(&sub_block[28], &w[ 7]);
        PACK32(&sub_block[32], &w[ 8]); PACK32(&sub_block[36], &w[ 9]);
        PACK32(&sub_block[40], &w[10]); PACK32(&sub_block[44], &w[11]);
        PACK32(&sub_block[48], &w[12]); PACK32(&sub_block[52], &w[13]);
        PACK32(&sub_block[56], &w[14]); PACK32(&sub_block[60], &w[15]);

        SHA256_SCR(16); SHA256_SCR(17); SHA256_SCR(18); SHA256_SCR(19);
        SHA256_SCR(20); SHA256_SCR(21); SHA256_SCR(22); SHA256_SCR(23);
        SHA256_SCR(24); SHA256_SCR(25); SHA256_SCR(26); SHA256_SCR(27);
        SHA256_SCR(28); SHA256_SCR(29); SHA256_SCR(30); SHA256_SCR(31);
        SHA256_SCR(32); SHA256_SCR(33); SHA256_SCR(34); SHA256_SCR(35);
        SHA256_SCR(36); SHA256_SCR(37); SHA256_SCR(38); SHA256_SCR(39);
        SHA256_SCR(40); SHA256_SCR(41); SHA256_SCR(42); SHA256_SCR(43);
        SHA256_SCR(44); SHA256_SCR(45); SHA256_SCR(46); SHA256_SCR(47);
        SHA256_SCR(48); SHA256_SCR(49); SHA256_SCR(50); SHA256_SCR(51);
        SHA256_SCR(52); SHA256_SCR(53); SHA256_SCR(54); SHA256_SCR(55);
        SHA256_SCR(56); SHA256_SCR(57); SHA256_SCR(58); SHA256_SCR(59);
        SHA256_SCR(60); SHA256_SCR(61); SHA256_SCR(62); SHA256_SCR(63);

        wv[0] = this->mState[0]; wv[1] = this->mState[1];
        wv[2] = this->mState[2]; wv[3] = this->mState[3];
        wv[4] = this->mState[4]; wv[5] = this->mState[5];
        wv[6] = this->mState[6]; wv[7] = this->mState[7];

        SHA256_EXP(0,1,2,3,4,5,6,7, 0); SHA256_EXP(7,0,1,2,3,4,5,6, 1);
        SHA256_EXP(6,7,0,1,2,3,4,5, 2); SHA256_EXP(5,6,7,0,1,2,3,4, 3);
        SHA256_EXP(4,5,6,7,0,1,2,3, 4); SHA256_EXP(3,4,5,6,7,0,1,2, 5);
        SHA256_EXP(2,3,4,5,6,7,0,1, 6); SHA256_EXP(1,2,3,4,5,6,7,0, 7);
        SHA256_EXP(0,1,2,3,4,5,6,7, 8); SHA256_EXP(7,0,1,2,3,4,5,6, 9);
        SHA256_EXP(6,7,0,1,2,3,4,5,10); SHA256_EXP(5,6,7,0,1,2,3,4,11);
        SHA256_EXP(4,5,6,7,0,1,2,3,12); SHA256_EXP(3,4,5,6,7,0,1,2,13);
        SHA256_EXP(2,3,4,5,6,7,0,1,14); SHA256_EXP(1,2,3,4,5,6,7,0,15);
        SHA256_EXP(0,1,2,3,4,5,6,7,16); SHA256_EXP(7,0,1,2,3,4,5,6,17);
        SHA256_EXP(6,7,0,1,2,3,4,5,18); SHA256_EXP(5,6,7,0,1,2,3,4,19);
        SHA256_EXP(4,5,6,7,0,1,2,3,20); SHA256_EXP(3,4,5,6,7,0,1,2,21);
        SHA256_EXP(2,3,4,5,6,7,0,1,22); SHA256_EXP(1,2,3,4,5,6,7,0,23);
        SHA256_EXP(0,1,2,3,4,5,6,7,24); SHA256_EXP(7,0,1,2,3,4,5,6,25);
        SHA256_EXP(6,7,0,1,2,3,4,5,26); SHA256_EXP(5,6,7,0,1,2,3,4,27);
        SHA256_EXP(4,5,6,7,0,1,2,3,28); SHA256_EXP(3,4,5,6,7,0,1,2,29);
        SHA256_EXP(2,3,4,5,6,7,0,1,30); SHA256_EXP(1,2,3,4,5,6,7,0,31);
        SHA256_EXP(0,1,2,3,4,5,6,7,32); SHA256_EXP(7,0,1,2,3,4,5,6,33);
        SHA256_EXP(6,7,0,1,2,3,4,5,34); SHA256_EXP(5,6,7,0,1,2,3,4,35);
        SHA256_EXP(4,5,6,7,0,1,2,3,36); SHA256_EXP(3,4,5,6,7,0,1,2,37);
        SHA256_EXP(2,3,4,5,6,7,0,1,38); SHA256_EXP(1,2,3,4,5,6,7,0,39);
        SHA256_EXP(0,1,2,3,4,5,6,7,40); SHA256_EXP(7,0,1,2,3,4,5,6,41);
        SHA256_EXP(6,7,0,1,2,3,4,5,42); SHA256_EXP(5,6,7,0,1,2,3,4,43);
        SHA256_EXP(4,5,6,7,0,1,2,3,44); SHA256_EXP(3,4,5,6,7,0,1,2,45);
        SHA256_EXP(2,3,4,5,6,7,0,1,46); SHA256_EXP(1,2,3,4,5,6,7,0,47);
        SHA256_EXP(0,1,2,3,4,5,6,7,48); SHA256_EXP(7,0,1,2,3,4,5,6,49);
        SHA256_EXP(6,7,0,1,2,3,4,5,50); SHA256_EXP(5,6,7,0,1,2,3,4,51);
        SHA256_EXP(4,5,6,7,0,1,2,3,52); SHA256_EXP(3,4,5,6,7,0,1,2,53);
        SHA256_EXP(2,3,4,5,6,7,0,1,54); SHA256_EXP(1,2,3,4,5,6,7,0,55);
        SHA256_EXP(0,1,2,3,4,5,6,7,56); SHA256_EXP(7,0,1,2,3,4,5,6,57);
        SHA256_EXP(6,7,0,1,2,3,4,5,58); SHA256_EXP(5,6,7,0,1,2,3,4,59);
        SHA256_EXP(4,5,6,7,0,1,2,3,60); SHA256_EXP(3,4,5,6,7,0,1,2,61);
        SHA256_EXP(2,3,4,5,6,7,0,1,62); SHA256_EXP(1,2,3,4,5,6,7,0,63);

        this->mState[0] += wv[0]; this->mState[1] += wv[1];
        this->mState[2] += wv[2]; this->mState[3] += wv[3];
        this->mState[4] += wv[4]; this->mState[5] += wv[5];
        this->mState[6] += wv[6]; this->mState[7] += wv[7];
#endif /* !UNROLL_LOOPS */
    }
}


SHA1_256::SHA1_256()
{
    Reset();
}

void SHA1_256::Reset()
{
    mFinalized = false;
#ifndef UNROLL_LOOPS
    size_t i;
    for (i = 0; i < 8; i++) {
        this->mState[i] = SHA1_256::sh0[i];
    }
#else
    this->mState[0] = SHA1_256::sh0[0]; this->mState[1] = SHA1_256::sh0[1];
    this->mState[2] = SHA1_256::sh0[2]; this->mState[3] = SHA1_256::sh0[3];
    this->mState[4] = SHA1_256::sh0[4]; this->mState[5] = SHA1_256::sh0[5];
    this->mState[6] = SHA1_256::sh0[6]; this->mState[7] = SHA1_256::sh0[7];
#endif /* !UNROLL_LOOPS */

    this->mRemainingLength = 0;
    this->mTotalLength = 0;
}

void SHA1_256::Append(XR_IN_BYTES(sizeInBytes) const void *appendBuffer, size_t sizeInBytes)
{
    XR_ASSERT_ALWAYS_FALSE(mFinalized);
    size_t block_nb;
    size_t new_len, rem_len, tmp_len;
    const uint8_t *message = (const uint8_t *)appendBuffer;
    const uint8_t *shifted_message;

    tmp_len = kBlockSizeBytes - this->mRemainingLength;
    rem_len = sizeInBytes < tmp_len ? sizeInBytes : tmp_len;

    MemCopy8(&this->mBlock[this->mRemainingLength], message, rem_len);

    if (this->mRemainingLength + sizeInBytes < kBlockSizeBytes) {
        this->mRemainingLength += sizeInBytes;
        return;
    }

    new_len = sizeInBytes - rem_len;
    block_nb = new_len / kBlockSizeBytes;

    shifted_message = message + rem_len;

    Transform(this->mBlock, 1);
    Transform(shifted_message, block_nb);

    rem_len = new_len % kBlockSizeBytes;

    MemCopy8(this->mBlock, &shifted_message[block_nb << 6],
           rem_len);

    this->mRemainingLength = rem_len;
    this->mTotalLength += (block_nb + 1) << 6;
}

void SHA1_256::Finalize()
{
    XR_ASSERT_ALWAYS_FALSE(mFinalized);
    size_t block_nb;
    size_t pm_len;
    size_t len_b;

    block_nb = (size_t(1) + ((kBlockSizeBytes - 9)
                     < (this->mRemainingLength % kBlockSizeBytes)));

    len_b = (this->mTotalLength + this->mRemainingLength) << 3;
    pm_len = block_nb << 6;

    MemClear8(this->mBlock + this->mRemainingLength, pm_len - this->mRemainingLength);
    this->mBlock[this->mRemainingLength] = 0x80;
    UNPACK32(len_b, this->mBlock + pm_len - 4);

    Transform(this->mBlock, block_nb);
    mFinalized = true;
}

void SHA1_256::GetResult(Digest *d)
{
    XR_ASSERT_ALWAYS_TRUE(mFinalized);
#ifndef UNROLL_LOOPS
    size_t i;
#endif

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 8; i++) {
        UNPACK32(this->mState[i], &d->result[i << 2]);
    }
#else
   UNPACK32(this->mState[0], &d->result[ 0]);
   UNPACK32(this->mState[1], &d->result[ 4]);
   UNPACK32(this->mState[2], &d->result[ 8]);
   UNPACK32(this->mState[3], &d->result[12]);
   UNPACK32(this->mState[4], &d->result[16]);
   UNPACK32(this->mState[5], &d->result[20]);
   UNPACK32(this->mState[6], &d->result[24]);
   UNPACK32(this->mState[7], &d->result[28]);
#endif /* !UNROLL_LOOPS */
}

/* SHA-512 functions */

void SHA1_512::Transform(const uint8_t *message,
                   size_t block_nb)
{
    uint64_t w[80];
    uint64_t wv[8];
    uint64_t t1, t2;
    const uint8_t *sub_block;
    size_t i, j;

    for (i = 0; i < block_nb; i++) {
        sub_block = message + (i << 7);

#ifndef UNROLL_LOOPS
        for (j = 0; j < 16; j++) {
            PACK64(&sub_block[j << 3], &w[j]);
        }

        for (j = 16; j < 80; j++) {
            SHA512_SCR(j);
        }

        for (j = 0; j < 8; j++) {
            wv[j] = this->mState[j];
        }

        for (j = 0; j < 80; j++) {
            t1 = wv[7] + SHA512_F2(wv[4]) + CH(wv[4], wv[5], wv[6])
                + sk[j] + w[j];
            t2 = SHA512_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (j = 0; j < 8; j++) {
            this->mState[j] += wv[j];
        }
#else
        PACK64(&sub_block[  0], &w[ 0]); PACK64(&sub_block[  8], &w[ 1]);
        PACK64(&sub_block[ 16], &w[ 2]); PACK64(&sub_block[ 24], &w[ 3]);
        PACK64(&sub_block[ 32], &w[ 4]); PACK64(&sub_block[ 40], &w[ 5]);
        PACK64(&sub_block[ 48], &w[ 6]); PACK64(&sub_block[ 56], &w[ 7]);
        PACK64(&sub_block[ 64], &w[ 8]); PACK64(&sub_block[ 72], &w[ 9]);
        PACK64(&sub_block[ 80], &w[10]); PACK64(&sub_block[ 88], &w[11]);
        PACK64(&sub_block[ 96], &w[12]); PACK64(&sub_block[104], &w[13]);
        PACK64(&sub_block[112], &w[14]); PACK64(&sub_block[120], &w[15]);

        SHA512_SCR(16); SHA512_SCR(17); SHA512_SCR(18); SHA512_SCR(19);
        SHA512_SCR(20); SHA512_SCR(21); SHA512_SCR(22); SHA512_SCR(23);
        SHA512_SCR(24); SHA512_SCR(25); SHA512_SCR(26); SHA512_SCR(27);
        SHA512_SCR(28); SHA512_SCR(29); SHA512_SCR(30); SHA512_SCR(31);
        SHA512_SCR(32); SHA512_SCR(33); SHA512_SCR(34); SHA512_SCR(35);
        SHA512_SCR(36); SHA512_SCR(37); SHA512_SCR(38); SHA512_SCR(39);
        SHA512_SCR(40); SHA512_SCR(41); SHA512_SCR(42); SHA512_SCR(43);
        SHA512_SCR(44); SHA512_SCR(45); SHA512_SCR(46); SHA512_SCR(47);
        SHA512_SCR(48); SHA512_SCR(49); SHA512_SCR(50); SHA512_SCR(51);
        SHA512_SCR(52); SHA512_SCR(53); SHA512_SCR(54); SHA512_SCR(55);
        SHA512_SCR(56); SHA512_SCR(57); SHA512_SCR(58); SHA512_SCR(59);
        SHA512_SCR(60); SHA512_SCR(61); SHA512_SCR(62); SHA512_SCR(63);
        SHA512_SCR(64); SHA512_SCR(65); SHA512_SCR(66); SHA512_SCR(67);
        SHA512_SCR(68); SHA512_SCR(69); SHA512_SCR(70); SHA512_SCR(71);
        SHA512_SCR(72); SHA512_SCR(73); SHA512_SCR(74); SHA512_SCR(75);
        SHA512_SCR(76); SHA512_SCR(77); SHA512_SCR(78); SHA512_SCR(79);

        wv[0] = this->mState[0]; wv[1] = this->mState[1];
        wv[2] = this->mState[2]; wv[3] = this->mState[3];
        wv[4] = this->mState[4]; wv[5] = this->mState[5];
        wv[6] = this->mState[6]; wv[7] = this->mState[7];

        j = 0;

        do {
            SHA512_EXP(0,1,2,3,4,5,6,7,j); j++;
            SHA512_EXP(7,0,1,2,3,4,5,6,j); j++;
            SHA512_EXP(6,7,0,1,2,3,4,5,j); j++;
            SHA512_EXP(5,6,7,0,1,2,3,4,j); j++;
            SHA512_EXP(4,5,6,7,0,1,2,3,j); j++;
            SHA512_EXP(3,4,5,6,7,0,1,2,j); j++;
            SHA512_EXP(2,3,4,5,6,7,0,1,j); j++;
            SHA512_EXP(1,2,3,4,5,6,7,0,j); j++;
        } while (j < 80);

        this->mState[0] += wv[0]; this->mState[1] += wv[1];
        this->mState[2] += wv[2]; this->mState[3] += wv[3];
        this->mState[4] += wv[4]; this->mState[5] += wv[5];
        this->mState[6] += wv[6]; this->mState[7] += wv[7];
#endif /* !UNROLL_LOOPS */
    }
}

SHA1_512::SHA1_512()
{
    Reset();
}

void SHA1_512::Reset()
{
    mFinalized = false;
#ifndef UNROLL_LOOPS
    size_t i;
    for (i = 0; i < 8; i++) {
        this->mState[i] = SHA1_512::sh0[i];
    }
#else
    this->mState[0] = SHA1_512::sh0[0]; this->mState[1] = SHA1_512::sh0[1];
    this->mState[2] = SHA1_512::sh0[2]; this->mState[3] = SHA1_512::sh0[3];
    this->mState[4] = SHA1_512::sh0[4]; this->mState[5] = SHA1_512::sh0[5];
    this->mState[6] = SHA1_512::sh0[6]; this->mState[7] = SHA1_512::sh0[7];
#endif /* !UNROLL_LOOPS */

    this->mRemainingLength = 0;
    this->mTotalLength = 0;
}

void SHA1_512::Append(XR_IN_BYTES(sizeInBytes) const void *appendBuffer, size_t sizeInBytes)
{
    XR_ASSERT_ALWAYS_FALSE(mFinalized);
    size_t block_nb;
    size_t new_len, rem_len, tmp_len;
    const uint8_t *message = (const uint8_t *)appendBuffer;
    const uint8_t *shifted_message;

    tmp_len = kBlockSizeBytes - this->mRemainingLength;
    rem_len = sizeInBytes < tmp_len ? sizeInBytes : tmp_len;

    MemCopy8(&this->mBlock[this->mRemainingLength], message, rem_len);

    if (this->mRemainingLength + sizeInBytes < kBlockSizeBytes) {
        this->mRemainingLength += sizeInBytes;
        return;
    }

    new_len = sizeInBytes - rem_len;
    block_nb = new_len / kBlockSizeBytes;

    shifted_message = message + rem_len;

    Transform(this->mBlock, 1);
    Transform(shifted_message, block_nb);

    rem_len = new_len % kBlockSizeBytes;

    MemCopy8(this->mBlock, &shifted_message[block_nb << 7],
           rem_len);

    this->mRemainingLength = rem_len;
    this->mTotalLength += (block_nb + 1) << 7;
}

void SHA1_512::Finalize()
{
    XR_ASSERT_ALWAYS_FALSE(mFinalized);
    size_t block_nb;
    size_t pm_len;
    size_t len_b;

    block_nb = size_t(1) + ((kBlockSizeBytes - 17)
                     < (this->mRemainingLength % kBlockSizeBytes));

    len_b = (this->mTotalLength + this->mRemainingLength) << 3;
    pm_len = block_nb << 7;

    MemClear8(this->mBlock + this->mRemainingLength, pm_len - this->mRemainingLength);
    this->mBlock[this->mRemainingLength] = 0x80;
    UNPACK32(len_b, this->mBlock + pm_len - 4);

    Transform(this->mBlock, block_nb);
    mFinalized = true;
}

void SHA1_512::GetResult(Digest *d)
{
    XR_ASSERT_ALWAYS_TRUE(mFinalized);
#ifndef UNROLL_LOOPS
    size_t i;
#endif

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 8; i++) {
        UNPACK64(this->mState[i], &d->result[i << 3]);
    }
#else
    UNPACK64(this->mState[0], &d->result[ 0]);
    UNPACK64(this->mState[1], &d->result[ 8]);
    UNPACK64(this->mState[2], &d->result[16]);
    UNPACK64(this->mState[3], &d->result[24]);
    UNPACK64(this->mState[4], &d->result[32]);
    UNPACK64(this->mState[5], &d->result[40]);
    UNPACK64(this->mState[6], &d->result[48]);
    UNPACK64(this->mState[7], &d->result[56]);
#endif /* !UNROLL_LOOPS */
}


/* SHA-384 functions */
SHA1_384::SHA1_384()
{
    Reset();
}

void SHA1_384::Reset()
{
    mFinalized = false;
#ifndef UNROLL_LOOPS
    size_t i;
    for (i = 0; i < 8; i++) {
        this->mState[i] = SHA1_384::sh0[i];
    }
#else
    this->mState[0] = SHA1_384::sh0[0]; this->mState[1] = SHA1_384::sh0[1];
    this->mState[2] = SHA1_384::sh0[2]; this->mState[3] = SHA1_384::sh0[3];
    this->mState[4] = SHA1_384::sh0[4]; this->mState[5] = SHA1_384::sh0[5];
    this->mState[6] = SHA1_384::sh0[6]; this->mState[7] = SHA1_384::sh0[7];
#endif /* !UNROLL_LOOPS */

    this->mRemainingLength = 0;
    this->mTotalLength = 0;
}

void SHA1_384::Append(XR_IN_BYTES(sizeInBytes) const void *appendBuffer, size_t sizeInBytes)
{
    SHA1_512::Append(appendBuffer, sizeInBytes);
}

void SHA1_384::Finalize()
{
    SHA1_512::Finalize();
}

void SHA1_384::GetResult(Digest *d)
{
    XR_ASSERT_ALWAYS_TRUE(mFinalized);
#ifndef UNROLL_LOOPS
    size_t i;
#endif

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 6; i++) {
        UNPACK64(this->mState[i], &d->result[i << 3]);
    }
#else
    UNPACK64(this->mState[0], &d->result[ 0]);
    UNPACK64(this->mState[1], &d->result[ 8]);
    UNPACK64(this->mState[2], &d->result[16]);
    UNPACK64(this->mState[3], &d->result[24]);
    UNPACK64(this->mState[4], &d->result[32]);
    UNPACK64(this->mState[5], &d->result[40]);
#endif /* !UNROLL_LOOPS */
}

/* SHA-224 functions */

SHA1_224::SHA1_224()
{
    Reset();
}

void SHA1_224::Reset()
{
    mFinalized = false;
#ifndef UNROLL_LOOPS
    size_t i;
    for (i = 0; i < 8; i++) {
        this->mState[i] = SHA1_224::sh0[i];
    }
#else
    this->mState[0] = SHA1_224::sh0[0]; this->mState[1] = SHA1_224::sh0[1];
    this->mState[2] = SHA1_224::sh0[2]; this->mState[3] = SHA1_224::sh0[3];
    this->mState[4] = SHA1_224::sh0[4]; this->mState[5] = SHA1_224::sh0[5];
    this->mState[6] = SHA1_224::sh0[6]; this->mState[7] = SHA1_224::sh0[7];
#endif /* !UNROLL_LOOPS */

    this->mRemainingLength = 0;
    this->mTotalLength = 0;
}

void SHA1_224::Append(XR_IN_BYTES(sizeInBytes) const void *appendBuffer, size_t sizeInBytes)
{
    SHA1_256::Append(appendBuffer, sizeInBytes);
}

void SHA1_224::Finalize()
{
    SHA1_256::Finalize();
}

void SHA1_224::GetResult(Digest *d)
{
    XR_ASSERT_ALWAYS_TRUE(mFinalized);

#ifndef UNROLL_LOOPS
    size_t i;
#endif

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 7; i++) {
        UNPACK32(this->mState[i], &d->result[i << 2]);
    }
#else
   UNPACK32(this->mState[0], &d->result[ 0]);
   UNPACK32(this->mState[1], &d->result[ 4]);
   UNPACK32(this->mState[2], &d->result[ 8]);
   UNPACK32(this->mState[3], &d->result[12]);
   UNPACK32(this->mState[4], &d->result[16]);
   UNPACK32(this->mState[5], &d->result[20]);
   UNPACK32(this->mState[6], &d->result[24]);
#endif /* !UNROLL_LOOPS */
}

/*
 *  FIPS-180-1 compliant SHA-1 implementation
 *
 *  Based on XySSL: Copyright (C) 2006-2008  Christophe Devine
 *
 *  Copyright (C) 2009  Paul Bakker <polarssl_maintainer at polarssl dot org>
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of PolarSSL or XySSL nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

SHA1_160::SHA1_160()
{
    Reset();
}
/*
 * SHA-1 context setup
 */
void SHA1_160::Reset( )
{
    this->total = 0;

    this->state[0] = 0x67452301;
    this->state[1] = 0xEFCDAB89;
    this->state[2] = 0x98BADCFE;
    this->state[3] = 0x10325476;
    this->state[4] = 0xC3D2E1F0;
}

void SHA1_160::Transform(  const uint8_t data[64] )
{
    unsigned long temp, W[16], A, B, C, D, E;

    GET_ULONG_BE( W[ 0], data,  0 );
    GET_ULONG_BE( W[ 1], data,  4 );
    GET_ULONG_BE( W[ 2], data,  8 );
    GET_ULONG_BE( W[ 3], data, 12 );
    GET_ULONG_BE( W[ 4], data, 16 );
    GET_ULONG_BE( W[ 5], data, 20 );
    GET_ULONG_BE( W[ 6], data, 24 );
    GET_ULONG_BE( W[ 7], data, 28 );
    GET_ULONG_BE( W[ 8], data, 32 );
    GET_ULONG_BE( W[ 9], data, 36 );
    GET_ULONG_BE( W[10], data, 40 );
    GET_ULONG_BE( W[11], data, 44 );
    GET_ULONG_BE( W[12], data, 48 );
    GET_ULONG_BE( W[13], data, 52 );
    GET_ULONG_BE( W[14], data, 56 );
    GET_ULONG_BE( W[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R(t)                                            \
(                                                       \
    temp = W[(t -  3) & 0x0F] ^ W[(t - 8) & 0x0F] ^     \
           W[(t - 14) & 0x0F] ^ W[ t      & 0x0F],      \
    ( W[t & 0x0F] = S(temp,1) )                         \
)

#define P(a,b,c,d,e,x)                                  \
{                                                       \
    e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);        \
}

    A = this->state[0];
    B = this->state[1];
    C = this->state[2];
    D = this->state[3];
    E = this->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5A827999

    P( A, B, C, D, E, W[0]  );
    P( E, A, B, C, D, W[1]  );
    P( D, E, A, B, C, W[2]  );
    P( C, D, E, A, B, W[3]  );
    P( B, C, D, E, A, W[4]  );
    P( A, B, C, D, E, W[5]  );
    P( E, A, B, C, D, W[6]  );
    P( D, E, A, B, C, W[7]  );
    P( C, D, E, A, B, W[8]  );
    P( B, C, D, E, A, W[9]  );
    P( A, B, C, D, E, W[10] );
    P( E, A, B, C, D, W[11] );
    P( D, E, A, B, C, W[12] );
    P( C, D, E, A, B, W[13] );
    P( B, C, D, E, A, W[14] );
    P( A, B, C, D, E, W[15] );
    P( E, A, B, C, D, R(16) );
    P( D, E, A, B, C, R(17) );
    P( C, D, E, A, B, R(18) );
    P( B, C, D, E, A, R(19) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1

    P( A, B, C, D, E, R(20) );
    P( E, A, B, C, D, R(21) );
    P( D, E, A, B, C, R(22) );
    P( C, D, E, A, B, R(23) );
    P( B, C, D, E, A, R(24) );
    P( A, B, C, D, E, R(25) );
    P( E, A, B, C, D, R(26) );
    P( D, E, A, B, C, R(27) );
    P( C, D, E, A, B, R(28) );
    P( B, C, D, E, A, R(29) );
    P( A, B, C, D, E, R(30) );
    P( E, A, B, C, D, R(31) );
    P( D, E, A, B, C, R(32) );
    P( C, D, E, A, B, R(33) );
    P( B, C, D, E, A, R(34) );
    P( A, B, C, D, E, R(35) );
    P( E, A, B, C, D, R(36) );
    P( D, E, A, B, C, R(37) );
    P( C, D, E, A, B, R(38) );
    P( B, C, D, E, A, R(39) );

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

    P( A, B, C, D, E, R(40) );
    P( E, A, B, C, D, R(41) );
    P( D, E, A, B, C, R(42) );
    P( C, D, E, A, B, R(43) );
    P( B, C, D, E, A, R(44) );
    P( A, B, C, D, E, R(45) );
    P( E, A, B, C, D, R(46) );
    P( D, E, A, B, C, R(47) );
    P( C, D, E, A, B, R(48) );
    P( B, C, D, E, A, R(49) );
    P( A, B, C, D, E, R(50) );
    P( E, A, B, C, D, R(51) );
    P( D, E, A, B, C, R(52) );
    P( C, D, E, A, B, R(53) );
    P( B, C, D, E, A, R(54) );
    P( A, B, C, D, E, R(55) );
    P( E, A, B, C, D, R(56) );
    P( D, E, A, B, C, R(57) );
    P( C, D, E, A, B, R(58) );
    P( B, C, D, E, A, R(59) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

    P( A, B, C, D, E, R(60) );
    P( E, A, B, C, D, R(61) );
    P( D, E, A, B, C, R(62) );
    P( C, D, E, A, B, R(63) );
    P( B, C, D, E, A, R(64) );
    P( A, B, C, D, E, R(65) );
    P( E, A, B, C, D, R(66) );
    P( D, E, A, B, C, R(67) );
    P( C, D, E, A, B, R(68) );
    P( B, C, D, E, A, R(69) );
    P( A, B, C, D, E, R(70) );
    P( E, A, B, C, D, R(71) );
    P( D, E, A, B, C, R(72) );
    P( C, D, E, A, B, R(73) );
    P( B, C, D, E, A, R(74) );
    P( A, B, C, D, E, R(75) );
    P( E, A, B, C, D, R(76) );
    P( D, E, A, B, C, R(77) );
    P( C, D, E, A, B, R(78) );
    P( B, C, D, E, A, R(79) );

#undef K
#undef F

    this->state[0] += A;
    this->state[1] += B;
    this->state[2] += C;
    this->state[3] += D;
    this->state[4] += E;
}

/*
 * SHA-1 process buffer
 */
void SHA1_160::Append( XR_IN_BYTES(sizeInBytes) const void *appendBuffer, size_t sizeInBytes )
{
    unsigned char *input = (unsigned char *)appendBuffer;
    unsigned long fill;
    unsigned long left;

    if( sizeInBytes <= 0 )
        return;

    left = this->total & 0x3F;
    fill = 64 - left;

    this->total += sizeInBytes;

    if( left && sizeInBytes >= fill )
    {
        MemCopy8( (void *) (this->buffer + left),
                (void *) input, fill );
        Transform( this->buffer );
        input += fill;
        sizeInBytes  -= fill;
        left = 0;
    }

    while( sizeInBytes >= 64 )
    {
        Transform( input );
        input += 64;
        sizeInBytes  -= 64;
    }

    if( sizeInBytes > 0 )
    {
        MemCopy8( (void *) (this->buffer + left),
                (void *) input, sizeInBytes );
    }
}

const uint8_t SHA1_160::sha1_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-1 final digest
 */
void SHA1_160::Finalize(  )
{
    uint32_t last, padn;

    uint64_t be_len = this->total << 3;

    be_len = XR_BIG_ENDIAN_TO_NATIVE_UINT64_T(be_len);

    last = this->total & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    Append( (const uint8_t *) sha1_padding, padn );
    Append( &be_len, 8 );
}

void SHA1_160::GetResult(Digest * d)
{
    PUT_ULONG_BE( this->state[0], d->result,  0 );
    PUT_ULONG_BE( this->state[1], d->result,  4 );
    PUT_ULONG_BE( this->state[2], d->result,  8 );
    PUT_ULONG_BE( this->state[3], d->result, 12 );
    PUT_ULONG_BE( this->state[4], d->result, 16 );
}

}}// end namespace xr
// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
