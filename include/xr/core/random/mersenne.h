// ######################################################################################### - FILE
/*! \file
    "Mersenne Twister" is a fast PRNG. It manages good distribution of numbers
    across the spectrum and is reasonably fast, but it is not a secure random number
    generator. For secure random numbers use \ref xr::CryptRandFill which
    uses the operating system to create random numbers.

    http://en.wikipedia.org/wiki/Mersenne_twister

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_RANDOM_MERSENNE_H
#define XR_CORE_RANDOM_MERSENNE_H

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
class MersenneTwisterPRNG
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    MersenneTwisterPRNG();

    // ------------------------------------------------------------------------------------  MEMBER
    /*! Seed with random values. There is no means of reproducing this stream. */
    // ------------------------------------------------------------------------------------  MEMBER
    void SeedRandom();

    // ------------------------------------------------------------------------------------  MEMBER
    /*! Seed with random value (and return seed). */
    // ------------------------------------------------------------------------------------  MEMBER
    uint32_t SeedRandomReproducible();

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Seed with the passed value. */
    // ------------------------------------------------------------------------------------  MEMBER
    void Seed(uint32_t s);

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Salt internal seed state array with the passed value.
            Use this to add entropy to the sequence.

            http://en.wikipedia.org/wiki/Information_entropy
        */
    // ------------------------------------------------------------------------------------  MEMBER
    void Salt(uint32_t s);

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Salt internal seed state array with the passed value.
            Use this to add entropy to the sequence.

            http://en.wikipedia.org/wiki/Information_entropy
        */
    // ------------------------------------------------------------------------------------  MEMBER
    void Salt(XR_IN_COUNT(numWords) const uint32_t s[], size_t numWords);

    // ------------------------------------------------------------------------------------  MEMBER
    /*! Return the next random value in the progression */
    // ------------------------------------------------------------------------------------  MEMBER
    uint32_t Next();

private:

    static const size_t N = 624;                     /*!< Period parameters */
    static const size_t M = 397;                     /*!< Period parameters */
    static const uint32_t MATRIX_A = 0x9908b0dfUL;   /*!< constant vector a */
    static const uint32_t UPPER_MASK = 0x80000000UL; /*!< most significant w-r bits */
    static const uint32_t LOWER_MASK = 0x7fffffffUL; /*!< least significant r bits */

    uint32_t mt[N]; /* the array for the state vector  */
    size_t   mti;    /* mti==N+1 means mt[N] is not initialized */
};
}}
#endif //#ifndef XR_CORE_RANDOM_MERSENNE_H
