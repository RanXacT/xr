// ######################################################################################### - FILE
/*!
    This file contains a slightly modified version of the standard mersenne
    twister random number generator

    Obtained from: 
    http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html

    License copied below:

    A C-program for MT19937, with initialization improved 2002/1/26.
    Coded by Takuji Nishimura and Makoto Matsumoto.

    Before using, initialize the state by using init_genrand(seed)
    or init_by_array(init_key, key_length).

    Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. The names of its contributors may not be used to endorse or promote
    products derived from this software without specific prior written
    permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


    Any feedback is very welcome.
    http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
    email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/

#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_RANDOM_MERSENNE_H
#include "xr/core/random/mersenne.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_RANDOM_CRYPT_RAND_H
#include "xr/core/random/crypt_rand.h"
#endif

namespace xr { namespace Core {
// --------------------------------------------------------------------------------------  FUNCTION
/* initializes mt[N] with a random number obtained from the OS */
// --------------------------------------------------------------------------------------  FUNCTION
void MersenneTwisterPRNG::SeedRandom()
{
    CryptRandFill(&mt[0], sizeof(uint32_t) * N);
}
// --------------------------------------------------------------------------------------  FUNCTION
/* initializes mt[N] with a random number obtained from the OS */
// --------------------------------------------------------------------------------------  FUNCTION
uint32_t MersenneTwisterPRNG::SeedRandomReproducible()
{
    uint32_t seed;
    CryptRandFill(&seed, sizeof(seed));
    Seed(seed);
    return seed;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
MersenneTwisterPRNG::MersenneTwisterPRNG()
{
    mti = N+1;
}


// --------------------------------------------------------------------------------------  FUNCTION
/* initializes mt[N] with a seed */
// --------------------------------------------------------------------------------------  FUNCTION
void MersenneTwisterPRNG::Seed(uint32_t s)
{
    mt[0]= s/* & 0xffffffffUL*/;
    for (mti=1; mti<N; mti++) {
        mt[mti] =
            (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + (uint32_t)mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        /*mt[mti] &= 0xffffffffUL;*/
        /* for >32 bit machines */
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
/* DC: Custom function. */
/* init_key is the array for initializing keys */
/* numWords is its length */
/* slight change for C++, 2004/2/26 */
// --------------------------------------------------------------------------------------  FUNCTION
void MersenneTwisterPRNG::Salt(XR_IN_COUNT(numWords) const uint32_t init_key[], size_t numWords)
{
    size_t i, j, k;

    // Can only salt after being seeded
    XR_ASSERT_DEBUG_NE(mti, N+1);
    // Force table re-generate upon next rand call.
    // (that way seed values are never directly returned)
    mti = N;

    i=1; j=0;
    k = (N>numWords ? N : numWords);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
            + init_key[j] + (uint32_t)j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=numWords) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
            - (uint32_t)i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void MersenneTwisterPRNG::Salt(uint32_t s)
{
    Salt(&s, 1);
}
// --------------------------------------------------------------------------------------  FUNCTION
/* generates a random number on [0,0xffffffff]-interval */
// --------------------------------------------------------------------------------------  FUNCTION
uint32_t MersenneTwisterPRNG::Next()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        size_t kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            Seed(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk-(N-M)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")