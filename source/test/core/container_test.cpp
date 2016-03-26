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
#ifndef XR_CORE_CONTAINERS_HASH
#include "xr/core/containers/hash.h"
#endif
#ifndef XR_CORE_CONTAINERS_VECTOR
#include "xr/core/containers/vector.h"
#endif
#ifndef XR_CORE_CONTAINERS_STACK_H
#include "xr/core/containers/stack.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_RANDOM_MERSENNE_H
#include "xr/core/random/mersenne.h"
#endif
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

struct TestEmpty
{
    inline uint32_t operator()()           const { return 0xFFFFFFFF;  }
    inline bool operator()(const uint32_t& a) const { return a == 0xFFFFFFFF; }
};

template <typename T> void DoubleCheckCount(T & s, const size_t kCount)
{
    size_t count = s.Count();
    XR_ASSERT_ALWAYS_EQ(count, kCount);
}


// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( Containers )
XR_UNITTEST_TEST_FUNC( HashMap_Simple )
{
    xr::Core::HashMap<int, int> s;

    xr::Core::HashInsertStatus status;

    DoubleCheckCount(s, 0);

    status = s.Insert(1,5);
    XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);
    DoubleCheckCount(s, 1);

    status = s.Insert(2,3);
    XR_ASSERT_ALWAYS_EQ(status, xr ::Core::kInsertWasNew);
    DoubleCheckCount(s, 2);

    // Check for things that should be there.
    const int* found = s.Find(1);
    XR_ASSERT_ALWAYS_NE(found, nullptr);
    XR_ASSERT_ALWAYS_EQ(*found, 5);

    found = s.Find(2);
    XR_ASSERT_ALWAYS_NE(found, nullptr);
    XR_ASSERT_ALWAYS_EQ(*found, 3);

    // Check for things that should NOT be there.
    found = s.Find(3);
    XR_ASSERT_ALWAYS_EQ(found, nullptr);

    found = s.Find(5);
    XR_ASSERT_ALWAYS_EQ(found, nullptr);

    // Make sure insert detects prior usage
    status = s.Insert(1,5);
    XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertReplacedExisting);
    DoubleCheckCount(s, 2);

    status = s.Insert(2,3);
    XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertReplacedExisting);
    DoubleCheckCount(s, 2);

    // Basic test of traversal
    bool found1 = false;
    bool found2 = false;
    s.ForEach( [&found1, &found2] (const int &key, const int &value) { 
        switch(key)
        {
        case 1: found1 = true; XR_ASSERT_ALWAYS_EQ(value, 5); break;
        case 2: found2 = true; XR_ASSERT_ALWAYS_EQ(value, 3); break;
        default: XR_ALWAYS_FAIL();
        }
    });

    XR_ASSERT_ALWAYS_TRUE(found1);
    XR_ASSERT_ALWAYS_TRUE(found2);
}

XR_UNITTEST_TEST_FUNC( HashMap_Expand )
{
    struct Contents
    {
        uint32_t key;
        uint32_t random;
    };
    xr::Core::HashMap<uint32_t, Contents> s;

    xr::Core::MersenneTwisterPRNG rng;
    rng.Seed(0xb1a3fbe8);


    const uint32_t kCount = 10000;
    uint32_t * values = (uint32_t*) XR_ALLOC(sizeof(uint32_t) * kCount+1, "TestValue");

    for(uint32_t i = 1; i <= kCount; ++i)
    {
        values[i] = rng.Next();
        Contents c = {i, values[i]};
        auto status = s.Insert(i, c);
        XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);

        size_t count = s.Count();
        XR_ASSERT_ALWAYS_TRUE(count == i);
    }

    for(uint32_t i = 1; i <= kCount; ++i)
    {
        const Contents * c = s.Find(i);
        XR_ASSERT_ALWAYS_NE(c, nullptr);
        XR_ASSERT_ALWAYS_EQ(c->key, i);
        XR_ASSERT_ALWAYS_EQ(c->random, values[i]);
    }

    for(uint32_t i = 1; i <= kCount; ++i)
    {
        const Contents * c = s.Find(i+kCount);
        XR_ASSERT_ALWAYS_EQ(c, nullptr);
    }


    size_t count = 0;

    s.ForEach( [&count, &values, kCount] (const uint32_t &key, const Contents &c) { 
        ++count;
        XR_ASSERT_ALWAYS_EQ(c.key, key);
        XR_ASSERT_ALWAYS_LE(key, kCount);
        XR_ASSERT_ALWAYS_EQ(c.random, values[key]);
    });

    XR_ASSERT_ALWAYS_EQ(count, kCount);
    
    for(uint32_t i = 1; i <= kCount; ++i)
    {
        values[i] = ~i;
        Contents c = {i, values[i]};
        auto status = s.Insert(i, c);
        XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertReplacedExisting);
    }

    count = 0;

    s.ForEach( [&count, &values, kCount] (const uint32_t &key, const Contents &c) { 
        ++count;
        XR_ASSERT_ALWAYS_EQ(c.key, key);
        XR_ASSERT_ALWAYS_LE(key, kCount);
        XR_ASSERT_ALWAYS_EQ(c.random, values[key]);
    });

    XR_ASSERT_ALWAYS_EQ(count, kCount);

    XR_FREE(values);
}

XR_UNITTEST_TEST_FUNC( HashMap_InPlace )
{
    struct Contents
    {
        Contents(uint32_t k, uint32_t r) : key(k), random(r) {}

        uint32_t key;
        uint32_t random;
    };
    xr::Core::HashMap<uint32_t, Contents> s;

    const uint32_t kCount = 1000;

    for(uint32_t i = 1; i < kCount; ++i)
    {
        Contents * c = s.Insert(i);
        c->key = i;
        c->random = i+1;

        size_t count = s.Count();
        XR_ASSERT_ALWAYS_TRUE(count == i);
    }

    size_t count = s.Count();
    XR_ASSERT_ALWAYS_TRUE(count == (kCount-1));

    count = 0;

    s.ForEach( [&count, kCount] (const uint32_t &key, const Contents &c) { 
        ++count;
        XR_ASSERT_ALWAYS_EQ(c.key, key);
        XR_ASSERT_ALWAYS_LE(c.random, key+1);
    });

    XR_ASSERT_ALWAYS_EQ(count, kCount-1);
}

XR_UNITTEST_TEST_FUNC( HashMap_Placement )
{
    struct Contents
    {
        Contents(uint32_t k, uint32_t r) : key(k), random(r) {}

        uint32_t key;
        uint32_t random;
    };
    xr::Core::HashMap<uint32_t, Contents> s;

    const uint32_t kCount = 1000;

    for(uint32_t i = 1; i < kCount; ++i)
    {
        new(s.Insert(i)) Contents(i,i+1);

        size_t count = s.Count();
        XR_ASSERT_ALWAYS_TRUE(count == i);
    }

    size_t count = 0;

    s.ForEach( [&count, kCount] (const uint32_t &key, const Contents &c) { 
        ++count;
        XR_ASSERT_ALWAYS_EQ(c.key, key);
        XR_ASSERT_ALWAYS_LE(c.random, key+1);
    });

    XR_ASSERT_ALWAYS_EQ(count, kCount-1);
}

XR_UNITTEST_TEST_FUNC( HashMap_Random )
{
    struct Contents
    {
        uint32_t key;
        uint32_t random;
    };
    xr::Core::HashMap<uint32_t, Contents> s;

    xr::Core::MersenneTwisterPRNG rng;
    rng.Seed(0xb1a3fbe8);

    // NOTE: you need to make sure (empirically) that the value kCount does not include any
    // repeated numbers from the mersenne twister, if so you need to find a new seed with no repeats for that count
    // (or weed out duplicates).
    const uint32_t kCount = 10000;
    uint32_t * values = (uint32_t*) XR_ALLOC(sizeof(uint32_t) * kCount+1, "TestValue");

    for(uint32_t i = 1; i <= kCount; ++i)
    {
        uint32_t r = rng.Next();
        if(r == 0)
        {
            --i;
            continue;
        }

        values[i] = r;
        Contents c = {r, i};
        auto status = s.Insert(r, c);
        XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);
    }

    for(uint32_t i = 1; i <= kCount; ++i)
    {
        const Contents * c = s.Find(values[i]);
        XR_ASSERT_ALWAYS_NE(c, nullptr);
        XR_ASSERT_ALWAYS_EQ(c->key, values[i]);
        XR_ASSERT_ALWAYS_EQ(c->random, i);
    }


    size_t count = 0;

    s.ForEach( [&count, &values, kCount] (const uint32_t &key, const Contents &c) { 
        ++count;
        XR_ASSERT_ALWAYS_EQ(c.key, key);
        XR_ASSERT_ALWAYS_LE(c.random, kCount);
        XR_ASSERT_ALWAYS_EQ(key,    values[c.random]);
    });

    XR_ASSERT_ALWAYS_EQ(count, kCount);

    XR_FREE(values);
}


XR_UNITTEST_TEST_FUNC( HashMap_Remove_Simple )
{
    xr::Core::HashMap<int, int> s;

    xr::Core::HashInsertStatus status;

    DoubleCheckCount(s, 0);

    status = s.Insert(1,5);
    XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);
    DoubleCheckCount(s, 1);

    s.Remove(1);
    DoubleCheckCount(s, 0);

    // Check for things that should be there.
    const int* found = s.Find(1);
    XR_ASSERT_ALWAYS_EQ(found, nullptr);
}

XR_UNITTEST_TEST_FUNC( HashMap_Remove_Random)
{
    const uint32_t theSizes[] =
    {
        5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u,
        37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u,
        83u, 89u, 97u, 103u, 109u, 113u, 127u, 137u, 139u, 149u,
        250
    };

    for ( uint32_t size : theSizes)
    {
        xr::Core::HashMap<uint32_t, uint32_t> s;

        xr::Core::MersenneTwisterPRNG rng;
        rng.Seed(0xb1a3fbe8);

        uint32_t * values = (uint32_t*) XR_ALLOC(sizeof(uint32_t) * size, "TestValue");
        for(uint32_t i = 0; i < size; ++i)
        {
            values[i] = rng.Next();
        }

        xr::Core::HashInsertStatus status;
        DoubleCheckCount(s, 0);

        for(uint32_t i = 0; i < size; ++i)
        {
            uint32_t x = values[i];
            status = s.Insert(x,x);
            XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);
            DoubleCheckCount(s, i+1);
        }

        // pull them out in another order
        std::random_shuffle(&values[0], &values[size-1]);

        // Pull them out one at a time and make sure all the remaining are still visible.
        for(uint32_t i = 0; i < size; ++i)
        {
            {
                uint32_t x = values[i];
                s.Remove(x);
                DoubleCheckCount(s, size - (i+1) );

                // Make sure it's gone.
                const uint32_t* found = s.Find(x);
                XR_ASSERT_ALWAYS_EQ(found, nullptr);
            }

            // Make sure the rest are still there.
            for(uint32_t remaining = (i+1); remaining < size; ++remaining)
            {
                uint32_t x = values[remaining];
                const uint32_t* found = s.Find(x);
                XR_ASSERT_ALWAYS_NE(found, nullptr);
                XR_ASSERT_ALWAYS_EQ(*found, x);
            }
        }

        XR_FREE(values);
    }
}

// This is a white box test. It specifically tests for aliasing issues and collision worst case 
// by creating situations where we shove data in that will create collisions. This assumes prime number 
// binning, if that changes the test will still work, but may not be as useful.
XR_UNITTEST_TEST_FUNC( HashMap_AbuseCollisions )
{
    const uint32_t thePrimes[] =
    {
        5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u,
        37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u,
        83u, 89u, 97u, 103u, 109u, 113u, 127u, 137u, 139u, 149u,
    };

    for ( uint32_t prime : thePrimes)
    {
        struct Contents
        {
            uint32_t keyTest;
            uint32_t random;
        };
        xr::Core::HashMap<uint32_t, Contents> s;
        const uint32_t kCount = prime * 2;

        for( uint32_t i = 1; i < kCount; ++i )
        {
            {
                Contents c = {i*prime, ~(i*prime)};
                auto status = s.Insert(i*prime, c);
                XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);
            }

            for( uint32_t x = 1; x <= i; ++x )
            {
                auto v = s.Find(x*prime);
                XR_ASSERT_ALWAYS_NE(v, nullptr);
                XR_ASSERT_ALWAYS_EQ(v->keyTest, x*prime);
                XR_ASSERT_ALWAYS_EQ(v->random, ~(x*prime));

                // With linear probing this will alias a collision. (but should not be matched)
                v = s.Find((x*prime)+1);
                XR_ASSERT_ALWAYS_EQ(v, nullptr);

                // Try something else... (why not)
                v = s.Find((x*prime)-1);
                XR_ASSERT_ALWAYS_EQ(v, nullptr);
            }

            for( uint32_t x = 1; x <= i; ++x )
            {
                Contents c = {i*prime, ~(i*prime)};
                auto status = s.Insert(i*prime, c);
                XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertReplacedExisting);
            }
        }


        DoubleCheckCount(s, kCount-1);
    }
}

XR_UNITTEST_TEST_FUNC( HashSet_Simple )
{
    xr::Core::HashSet<int> s;
    xr::Core::HashInsertStatus status;
    DoubleCheckCount(s, 0);

    status = s.Insert(1);
    XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);
    DoubleCheckCount(s, 1);

    status = s.Insert(2);
    XR_ASSERT_ALWAYS_EQ(status, xr ::Core::kInsertWasNew);
    DoubleCheckCount(s, 2);

    bool found = s.Find(1);
    XR_ASSERT_ALWAYS_TRUE(found);

    found = s.Find(2);
    XR_ASSERT_ALWAYS_TRUE(found);

    found = s.Find(3);
    XR_ASSERT_ALWAYS_FALSE(found);

    found = s.Find(5);
    XR_ASSERT_ALWAYS_FALSE(found);

    // Make sure insert detects prior usage
    status = s.Insert(1);
    XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertReplacedExisting);
    DoubleCheckCount(s, 2);

    status = s.Insert(2);
    XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertReplacedExisting);
    DoubleCheckCount(s, 2);

    bool found1 = false;
    bool found2 = false;
    s.ForEach( [&found1, &found2] (const int &key) { 
        switch(key)
        {
        case 1: found1 = true; break;
        case 2: found2 = true; break;
        default: XR_ALWAYS_FAIL();
        }
    });

    XR_ASSERT_ALWAYS_TRUE(found1);
    XR_ASSERT_ALWAYS_TRUE(found2);
}

XR_UNITTEST_TEST_FUNC( HashSet_Expand )
{

    xr::Core::HashSet<uint32_t, TestEmpty> s;

    const uint32_t kCount = 10000;
    for(uint32_t i = 0; i < kCount; ++i)
    {
        auto status = s.Insert(i);
        XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);

        size_t count = s.Count();
        XR_ASSERT_ALWAYS_EQ(count, i+1);
    }

    for(uint32_t i = 0; i < kCount; ++i)
    {
        auto status = s.Insert(i);
        XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertReplacedExisting);

        size_t count = s.Count();
        XR_ASSERT_ALWAYS_EQ(count, kCount);
    }


    for(uint32_t i = 0; i < kCount; ++i)
    {
        bool found = s.Find(i);
        XR_ASSERT_ALWAYS_TRUE(found);
    }

    for(uint32_t i = 0; i < kCount; ++i)
    {
        bool found = s.Find(i+kCount);
        XR_ASSERT_ALWAYS_FALSE(found);
    }


    DoubleCheckCount(s, kCount);
}

// This is a white box test. It specifically tests for aliasing issues and collision worst case 
// by creating situations where we shove data in that will create collisions. This assumes prime number 
// binning, if that changes the test will still work, but may not be as useful.
XR_UNITTEST_TEST_FUNC( HashSet_AbuseCollisions )
{
    const uint32_t thePrimes[] =
    {
        5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u,
        37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u,
        83u, 89u, 97u, 103u, 109u, 113u, 127u, 137u, 139u, 149u,
    };

    for ( uint32_t prime : thePrimes)
    {
        xr::Core::HashSet<uint32_t, TestEmpty> s;
        const uint32_t kCount = prime * 2;

        for( uint32_t i = 1; i < kCount; ++i )
        {
            {
                auto status = s.Insert(i*prime);
                XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);
            }

            for( uint32_t x = 1; x <= i; ++x )
            {
                bool found = s.Find(x*prime);
                XR_ASSERT_ALWAYS_EQ(found, true);

                // With linear probing this will alias a collision. (but should not be matched)
                found = s.Find((x*prime)+1);
                XR_ASSERT_ALWAYS_EQ(found, false);

                // Try something else... (why not)
                found = s.Find((x*prime)-1);
                XR_ASSERT_ALWAYS_EQ(found, false);
            }

            for( uint32_t x = 1; x <= i; ++x )
            {
                auto status = s.Insert(x*prime);
                XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertReplacedExisting);
            }
        }

        
        DoubleCheckCount(s, kCount-1);
    }
}

XR_UNITTEST_TEST_FUNC( HashSet_Random )
{
    xr::Core::HashSet<uint32_t, TestEmpty> s;

    xr::Core::MersenneTwisterPRNG rng;
    rng.Seed(0xb1a3fbe8);

    // NOTE: you need to make sure (empirically) that the value kCount does not include any
    // repeated numbers from the mersenne twister, if so you need to find a new seed with no repeats for that count
    // (or weed out duplicates).
    const uint32_t kCount = 5000;

    for(uint32_t i = 0; i < kCount; ++i)
    {
        uint32_t r = rng.Next();
        if(r == 0xFFFFFFFF)
        {
            --i;
            continue;
        }
        auto status = s.Insert(r);
        XR_ASSERT_ALWAYS_EQ(status, xr::Core::kInsertWasNew);
    }

    // Sequence up to 10000 has no repeats.

    for(uint32_t i = 0; i < kCount; ++i)
    {
        uint32_t r = rng.Next();
        if(r == 0xFFFFFFFF)
        {
            --i;
            continue;
        }

        bool found = s.Find(r);
        XR_ASSERT_ALWAYS_FALSE(found);
    }

    // Start over sequence
    rng.Seed(0xb1a3fbe8);

    for(uint32_t i = 0; i < kCount; ++i)
    {
        uint32_t r = rng.Next();
        if(r == 0xFFFFFFFF)
        {
            --i;
            continue;
        }

        bool found = s.Find(r);
        XR_ASSERT_ALWAYS_TRUE(found);
    }

    DoubleCheckCount(s, kCount);
}

XR_UNITTEST_TEST_FUNC( Vector_Simple )
{
    xr::Core::Vector<uint32_t> s;

    xr::Core::MersenneTwisterPRNG rng;
    rng.Seed(0xb1a3fbe8);

    // NOTE: you need to make sure (empirically) that the value kCount does not include any
    // repeated numbers from the mersenne twister, if so you need to find a new seed with no repeats for that count
    // (or weed out duplicates).
    const uint32_t kCount = 5000;

    for(uint32_t i = 0; i < kCount; ++i)
    {
        uint32_t r = rng.Next();
        s.Insert(r);
    }

    // Start over sequence
    rng.Seed(0xb1a3fbe8);
    size_t count = 0;

    s.ForEach( [&count, &rng, kCount] (const uint32_t &x) { 
        ++count;
        uint32_t r = rng.Next();
        XR_ASSERT_ALWAYS_EQ(x, r);
    });

    XR_ASSERT_ALWAYS_EQ(count, kCount);
}

XR_UNITTEST_TEST_FUNC( List_Simple )
{
    xr::Core::Stack<uint32_t> s;

    const uint32_t kCount = 5000;

    for(uint32_t i = 0; i < kCount; ++i)
    {
        s.Insert(i);
        XR_ASSERT_ALWAYS_EQ(s.Count(), i+1);
    }

    for (uint32_t i = 0; i < kCount; ++i)
    {
        XR_ASSERT_ALWAYS_TRUE(s.Contains(i));
    }

    size_t count = 0;

    s.ForEach( [&count, kCount] (const uint32_t &x) { 
        ++count;
        XR_ASSERT_ALWAYS_EQ(x, kCount-count); // Read them out in reverse order
    });

    XR_ASSERT_ALWAYS_EQ(count, kCount);
}

XR_UNITTEST_TEST_FUNC( List_Inplace )
{
    xr::Core::Stack<uint32_t> s;
    XR_ASSERT_ALWAYS_EQ(s.Count(), 0);

    // NOTE: you need to make sure (empirically) that the value kCount does not include any
    // repeated numbers from the mersenne twister, if so you need to find a new seed with no repeats for that count
    // (or weed out duplicates).
    const uint32_t kCount = 100;

    for(uint32_t i = 0; i < kCount; ++i)
    {
        uint32_t * x = s.Insert();
        XR_ASSERT_ALWAYS_EQ(s.Count(), (i+1));
        *x = i;
    }

    for (uint32_t i = 0; i < kCount; ++i)
    {
        XR_ASSERT_ALWAYS_TRUE(s.Contains(i));
    }

    size_t count = 0;
    s.ForEach( [&count, kCount] (const uint32_t &x) { 
        ++count;
        XR_ASSERT_ALWAYS_EQ(x, kCount-count); // Read them out in reverse order
    });

    XR_ASSERT_ALWAYS_EQ(count, kCount);
}


XR_UNITTEST_TEST_FUNC(List_Remove_Simple)
{
    xr::Core::Stack<int> s;
    XR_ASSERT_ALWAYS_EQ(s.Count(), 0);

    s.Insert(1);
    XR_ASSERT_ALWAYS_EQ(s.Count(), 1);

    // Check for things that should be there.
    XR_ASSERT_ALWAYS_EQ(s.Contains(1), true);

    s.Remove(1);
    XR_ASSERT_ALWAYS_EQ(s.Count(), 0);
    XR_ASSERT_ALWAYS_EQ(s.Contains(1), false);

    s.Insert(2);
    XR_ASSERT_ALWAYS_EQ(s.Count(), 1);

    // Check for things that should be there.
    XR_ASSERT_ALWAYS_EQ(s.Contains(1), false);
    XR_ASSERT_ALWAYS_EQ(s.Contains(2), true);

    s.Remove(2);
    XR_ASSERT_ALWAYS_EQ(s.Count(), 0);
    XR_ASSERT_ALWAYS_EQ(s.Contains(2), false);
}


XR_UNITTEST_TEST_FUNC(Stack_Remove_Simple)
{
    xr::Core::Stack<int> s;
    XR_ASSERT_ALWAYS_EQ(s.Count(), 0);

    s.Push(1);
    XR_ASSERT_ALWAYS_EQ(s.Count(), 1);

    // Check for things that should be there.
    XR_ASSERT_ALWAYS_EQ(s.Contains(1), true);

    XR_ASSERT_ALWAYS_EQ(s.Pop(), 1);
    XR_ASSERT_ALWAYS_EQ(s.Count(), 0);
    XR_ASSERT_ALWAYS_EQ(s.Contains(1), false);

    s.Push(2);
    XR_ASSERT_ALWAYS_EQ(s.Count(), 1);

    // Check for things that should be there.
    XR_ASSERT_ALWAYS_EQ(s.Contains(1), false);
    XR_ASSERT_ALWAYS_EQ(s.Contains(2), true);

    XR_ASSERT_ALWAYS_EQ(s.Pop(), 2);
    XR_ASSERT_ALWAYS_EQ(s.Count(), 0);
    XR_ASSERT_ALWAYS_EQ(s.Contains(2), false);
}

XR_UNITTEST_TEST_FUNC(List_Remove_Random)
{
    const uint32_t theSizes[] =
    {
        5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u,
        37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u,
        83u, 89u, 97u, 103u, 109u, 113u, 127u, 137u, 139u, 149u,
        250
    };

    for (uint32_t size : theSizes)
    {
        xr::Core::Stack<uint32_t> s;

        xr::Core::MersenneTwisterPRNG rng;
        rng.Seed(0xb1a3fbe8);

        uint32_t * values = (uint32_t*)XR_ALLOC(sizeof(uint32_t) * size, "TestValue");
        for (uint32_t i = 0; i < size; ++i)
        {
            values[i] = rng.Next();
        }

        XR_ASSERT_ALWAYS_EQ(s.Count(), 0);

        for (uint32_t i = 0; i < size; ++i)
        {
            uint32_t x = values[i];
            s.Insert(x);
            XR_ASSERT_ALWAYS_EQ(s.Count(), i + 1);
        }

        // pull them out in another order
        std::random_shuffle(&values[0], &values[size - 1]);

        // Pull them out one at a time and make sure all the remaining are still visible.
        for (uint32_t i = 0; i < size; ++i)
        {
            {
                uint32_t x = values[i];
                s.Remove(x);
                XR_ASSERT_ALWAYS_EQ(s.Count(), size - (i + 1));

                // Make sure it's gone.
                XR_ASSERT_ALWAYS_FALSE(s.Contains(x));
            }

            // Make sure the rest are still there.
            for (uint32_t remaining = (i + 1); remaining < size; ++remaining)
            {
                uint32_t x = values[remaining];
                XR_ASSERT_ALWAYS_TRUE(s.Contains(x));
            }
        }

        XR_FREE(values);
    }
}


XR_UNITTEST_GROUP_END()
#endif // #if defined(XR_TEST_FEATURES_ENABLED)
