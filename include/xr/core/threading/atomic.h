// ######################################################################################### - FILE
/*! \file
Atomic operations.

\page coreatomic Atomic Access System Overview
The atomic access functions allow one to use atomic access functions. They
provide similar service to Interlocked* functions in windows and to __sync_*
functions). All functions consistently return the original value (i.e. the value
that was read before being atomically replaced).

\code
Exrmples:
originalValue = AtomicIncrement(&protectedValue);
originalValue = AtomicDecrement(&protectedValue);
originalValue = AtomicCompareAndSwap(&protectedValue, compareWith, newValueIfMatches);
\endcode

\note that platform functions are inconsistent as to return value being
the original value or the written value (for exrmple in windows
InterlockedCompareExchange returning the original value and InterlockedAdd
type functions returning the written value). This library is consistent, but
note that naive porting from system functions by search replace may cause issues.

\par
\note there are also inconsistencies across platforms as to argument order.
order is clearly stated in the function arguments and in the documentation,
but again if porting from another library be sure to verify argument order.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_THREADING_ATOMIC_H
#define XR_CORE_THREADING_ATOMIC_H

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
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127)  // Conditional expression is constant
#endif

// -----------------------------------------------------------------------------------------  MACRO
/// Alignment requirement for AtomicDoublePointer object
// -----------------------------------------------------------------------------------------  MACRO
#if XR_PLATFORM_PTR_SIZE == 4
#   define XR_ATOMIC_DOUBLE_POINTER_ALIGN 8
#else
#   define XR_ATOMIC_DOUBLE_POINTER_ALIGN 16
#endif

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE

#if defined(XR_COMPILER_MICROSOFT)
// Microsoft compiler intrinsic shims
// No need to include windows.h for a couple functions.
// Sadly some of these are intrinsics, some are emulated.
extern "C" {
    extern long __cdecl _InterlockedCompareExchange (long volatile *, long, long);
    extern __int64 __cdecl _InterlockedCompareExchange64(__int64 volatile *, __int64, __int64);
    extern long __cdecl _InterlockedIncrement (long volatile *);
    extern long __cdecl _InterlockedDecrement (long volatile *);
    extern long __cdecl _InterlockedExchangeAdd (long volatile *, long);

#if defined(XR_CPU_X64)
    unsigned char __cdecl _InterlockedCompareExchange128(__int64 volatile * Destination, __int64 ExchangeHigh,__int64 ExchangeLow, __int64 * ComparandResult);
    extern __int64 __cdecl _InterlockedIncrement64 (__int64 volatile *);
    extern __int64 __cdecl _InterlockedDecrement64 (__int64 volatile *);
    extern __int64 __cdecl _InterlockedExchangeAdd64 (__int64 volatile *, __int64);
#endif

#if !defined(XR_COMPILER_INTEL)
    // Tell the compiler to make these intrinsics (or else they will be function calls)
#   pragma intrinsic(_InterlockedCompareExchange)
#   pragma intrinsic(_InterlockedCompareExchange64)
#   pragma intrinsic(_InterlockedIncrement)
#   pragma intrinsic(_InterlockedDecrement)
#   pragma intrinsic(_InterlockedExchangeAdd)
#endif

#if defined(XR_CPU_X64)
#   pragma intrinsic(_InterlockedCompareExchange128)
#   pragma intrinsic(_InterlockedIncrement64)
#   pragma intrinsic(_InterlockedDecrement64)
#   pragma intrinsic(_InterlockedExchangeAdd64)
#endif

#if !defined(XR_CPU_X64)
    // Emulated functions. (from winbase.h)
    __forceinline __int64 _XR_InterlockedIncrement64 ( __int64 volatile *Addend )
    {
        __int64 Old;
        do {
            Old = *Addend;
        } while (_InterlockedCompareExchange64(Addend, Old + 1, Old) != Old);
        return Old + 1;
    }

    __forceinline __int64 _XR_InterlockedDecrement64 ( __int64 volatile *Addend )
    {
        __int64 Old;
        do {
            Old = *Addend;
        } while (_InterlockedCompareExchange64(Addend, Old - 1, Old) != Old);
        return Old - 1;
    }

    __forceinline __int64 _XR_InterlockedExchangeAdd64( __int64 volatile *Addend, __int64 Value )
    {
        __int64 Old;
        do {
            Old = *Addend;
        } while (_InterlockedCompareExchange64(Addend, Old + Value, Old) != Old);
        return Old;
    }
#else
#define _XR_InterlockedIncrement64 _InterlockedIncrement64
#define _XR_InterlockedDecrement64 _InterlockedDecrement64
#define _XR_InterlockedExchangeAdd64 _InterlockedExchangeAdd64
#endif
};
#endif
// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

XR_ALIGN_PREFIX(XR_ATOMIC_DOUBLE_POINTER_ALIGN)
/// Type is used internally to encapsulate double pointer CAS operations
/// It can be used externally, but may be of little use.
union AtomicDoublePointer {
    intptr_t  mInts[2]; ///< Access as integers
    void     *mPtrs[2]; ///< Access as pointers
}
XR_ALIGN_POSTFIX(XR_ATOMIC_DOUBLE_POINTER_ALIGN);

// ######################################################################################### - FILE
// ######################################################################################### - FILE
#if defined(XR_COMPILER_MICROSOFT)

// ######################################################################################### - FILE
// internal
// ######################################################################################### - FILE
namespace detail {

// -----------------------------------------------------------------------------------------  MACRO
// Help defining specialized templates to deal with otherwise messy atomics.
// -----------------------------------------------------------------------------------------  MACRO
#define XR_ATOMIC_OP_DEFINE( OP, impl4, impl8, ... )    \
    template<typename T, size_t size>                   \
    struct Atomic##OP {                                 \
        static T DoIt(__VA_ARGS__);                     \
    };                                                  \
    template<typename T> struct Atomic##OP<T, 4>        \
    {   static T DoIt(__VA_ARGS__)                      \
        {                                               \
            return impl4;                               \
        }                                               \
    };                                                  \
    template<typename T>                                \
    struct Atomic##OP<T, 8>                             \
    {   static T DoIt(__VA_ARGS__)                      \
        {                                               \
            return impl8;                               \
        }                                               \
    }


// ***************************************************************************************** - TYPE
// guts of AtomicIncrement. Note the flipped return value (we return original)
// ***************************************************************************************** - TYPE
XR_ATOMIC_OP_DEFINE(Inc,
    (T)_InterlockedIncrement((long volatile *) __ptr)-T(1),
    (T)_XR_InterlockedIncrement64((__int64 volatile *)__ptr) - T(1),
    T volatile * __ptr);
// ***************************************************************************************** - TYPE
// guts of AtomicDecrement. Note the flipped return value (we return original)
// ***************************************************************************************** - TYPE
XR_ATOMIC_OP_DEFINE(Dec,
    (T)_InterlockedDecrement((long volatile *) __ptr)+T(1),
    (T)_XR_InterlockedDecrement64((__int64 volatile *) __ptr)+T(1),
    T volatile * __ptr);
// ***************************************************************************************** - TYPE
// guts of AtomicAdd
// ***************************************************************************************** - TYPE
XR_ATOMIC_OP_DEFINE(Add,
    (T)_InterlockedExchangeAdd((long volatile *) __ptr, value),
    (T)_XR_InterlockedExchangeAdd64( (__int64 volatile *) __ptr, value),
    T volatile * __ptr, T value);
// ***************************************************************************************** - TYPE
// guts of AtomicSubtract
// ***************************************************************************************** - TYPE
XR_ATOMIC_OP_DEFINE(Sub,
    (T)_InterlockedExchangeAdd((long volatile *) __ptr, 0 - value),
    (T)_XR_InterlockedExchangeAdd64( (__int64 volatile *) __ptr, __int64(0) - value),
    T volatile * __ptr, T value);

#undef XR_ATOMIC_OP_DEFINE

// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
template<typename T, size_t size>
struct AtomicCAS {
    static T DoIt(T volatile* __ptr, const T &__comparand, const T &__replacement);
};
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
template<typename T> struct AtomicCAS<T, 4>
{   static T DoIt(T volatile* __ptr, const T &__comparand, const T &__replacement)
    {
        // Need to fill in temporary values to do this.
        union {
            T       asPassed;
            long    asNeeded;
        }replacement, compare, ret;

        replacement.asPassed = __replacement;
        compare.asPassed     = __comparand;

        ret.asNeeded = _InterlockedCompareExchange((long volatile *) __ptr,
            replacement.asNeeded,
            compare.asNeeded);

        return ret.asPassed;
    }
};
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
template<typename T>
struct AtomicCAS<T, 8>
{   static T DoIt(T volatile* __ptr, const T &__comparand, const T &__replacement)
    {
        // Need to fill in temporary values to do this.
        union {
            T       asPassed;
            __int64 asNeeded;
        }replacement, compare, ret;

        replacement.asPassed = __replacement;
        compare.asPassed     = __comparand;

        ret.asNeeded = _InterlockedCompareExchange64((__int64 volatile *) __ptr,
            replacement.asNeeded,
            compare.asNeeded);
        return ret.asPassed;
    }
};
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
#if XR_PLATFORM_PTR_SIZE == 8
template<typename T>
struct AtomicCAS<T, 16>
{   static T DoIt(T volatile* __ptr, const T &__comparand, const T &__replacement)
    {
        // Need to fill in temporary values to do this.
        union {
            T    asPassed;
            xr::Core::AtomicDoublePointer asNeeded;
        } replacement, compare;

        replacement.asPassed = __replacement;
        compare.asPassed     = __comparand;

        _InterlockedCompareExchange128((__int64 volatile *) __ptr,
            (__int64)replacement.asNeeded.mInts[1] ,
            (__int64)replacement.asNeeded.mInts[0] ,
            (__int64*)& compare.asNeeded.mInts);

        return compare.asPassed;
    }
};
#endif
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicCompareAndSwap(T volatile* __ptr, const T &__comparand, const T &__replacement)
{
#if XR_PLATFORM_PTR_SIZE == 4
    static_assert( sizeof(T) == 4 || sizeof(T) == 8, "Size Check Failed");
#elif XR_PLATFORM_PTR_SIZE == 8
    static_assert( sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16, "Size Check Failed");
#endif
    return detail::AtomicCAS<T, sizeof(T)>::DoIt(__ptr, __comparand, __replacement);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicIncrement(T volatile* __ptr)
{
    static_assert( sizeof(T) == 4 || sizeof(T) == 8, "Size Check Failed");
    return detail::AtomicInc<T, sizeof(T)>::DoIt(__ptr);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicDecrement(T volatile* __ptr)
{
    static_assert( sizeof(T) == 4 || sizeof(T) == 8, "Size Check Failed");
    return detail::AtomicDec<T, sizeof(T)>::DoIt(__ptr);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicAdd(T volatile* __ptr, T value)
{
    static_assert( sizeof(T) == 4 || sizeof(T) == 8, "Size Check Failed");
    return detail::AtomicAdd<T, sizeof(T)>::DoIt(__ptr, value);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicSubtract(T volatile* __ptr, T value)
{
    static_assert( sizeof(T) == 4 || sizeof(T) == 8, "Size Check Failed");
    return detail::AtomicSub<T, sizeof(T)>::DoIt(__ptr, value);
}
// ######################################################################################### - FILE
// ######################################################################################### - FILE
#elif defined(XR_COMPILER_GCC)
// Much simpler :).
template<typename T>
inline T AtomicIncrement(T volatile* __ptr)
{
    return __sync_fetch_and_add( (__ptr), 1 );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicDecrement(T volatile* __ptr)
{
    return __sync_fetch_and_sub( (__ptr), 1 );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicAdd(T volatile* __ptr, T value)
{
    return __sync_fetch_and_add( (__ptr), (value) );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicSubtract(T volatile* __ptr, T value)
{
    return __sync_fetch_and_sub( (__ptr), (value) );
}

namespace detail
{
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
template<typename T, size_t size>
struct AtomicCAS {
    static T DoIt(T volatile* __ptr, const T &__comparand, const T &__replacement);
};
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
template<typename T> struct AtomicCAS<T, 4>
{   static T DoIt(T volatile* __ptr, const T &__comparand, const T &__replacement)
    {
        // Need to fill in temporary values to do this.
        union {
            T       asPassed;
            int32_t asNeeded;
        }replacement, compare, ret;

        replacement.asPassed = __replacement;
        compare.asPassed     = __comparand;

        ret.asNeeded = __sync_val_compare_and_swap((int32_t volatile *) __ptr,
            compare.asNeeded,
            replacement.asNeeded);
        return ret.asPassed;
    }
};
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
template<typename T>
struct AtomicCAS<T, 8>
{   static T DoIt(T volatile* __ptr, const T &__comparand, const T &__replacement)
    {
        // Need to fill in temporary values to do this.
        union {
            T       asPassed;
            int64_t asNeeded;
        }replacement, compare, ret;

        replacement.asPassed = __replacement;
        compare.asPassed     = __comparand;

        ret.asNeeded = __sync_val_compare_and_swap((int64_t volatile *) __ptr,
            compare.asNeeded,
            replacement.asNeeded);
        return ret.asPassed;
    }
};
// ***************************************************************************************** - TYPE
// ***************************************************************************************** - TYPE
#if XR_PLATFORM_PTR_SIZE == 8
template<typename T>
struct AtomicCAS<T, 16>
{   static T DoIt(T volatile* basep, const T &__comparand, const T &__replacement)
    {
#if XR_CPU_X64
        // Need to fill in temporary values to do this.
        union {
            T                       asPassed;
            xr::Core::AtomicDoublePointer asNeeded;
        } replacement, compare;

        replacement.asPassed = __replacement;
        compare.asPassed     = __comparand;

        bool result;
        __asm__ __volatile__
            (
            "lock cmpxchg16b %1\n\t"
            "setz %0"
            : "=q" ( result )
            , "+m" ( * basep )
            , "+d" ( compare.asNeeded.mInts[1] )
            , "+a" ( compare.asNeeded.mInts[0] )
            : "c" ( replacement.asNeeded.mInts[1] )
            , "b" ( replacement.asNeeded.mInts[0] )
            : "cc"
            );

        return compare.asPassed;
#else
#   error "CPU Not supported, but later gcc versions  (4.3+) may be able to use __sync_val_compare_and_swap"
#endif
    }
};
#endif

}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicCompareAndSwap(T volatile* __ptr, const T &__comparand, const T &__replacement)
{
#if XR_PLATFORM_PTR_SIZE == 4
    static_assert( sizeof(T) == 4 || sizeof(T) == 8, "Size Check Failed");
#elif XR_PLATFORM_PTR_SIZE == 8
    static_assert( sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16, "Size Check Failed");
#endif
    return detail::AtomicCAS<T, sizeof(T)>::DoIt(__ptr, __comparand, __replacement);
}
#elif defined(XR_COMPILER_DOXYGEN)
//@{
// --------------------------------------------------------------------------------------  FUNCTION
/*!
Performs an atomic operation on the specified address. This returns the
original value in all cases. Essentially this is the equivalent of
__sync_fetch_and_* functions from GCC.


\note that Windows Interlocked functions are inconsistent in whether they
return the previous or new values, so the convention will be different
for some Interlocked calls.
*/
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicIncrement(T volatile* __ptr)
{
    return __sync_fetch_and_add( (__ptr), 1 );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicDecrement(T volatile* __ptr)
{
    return __sync_fetch_and_sub( (__ptr), 1 );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicAdd(T volatile* __ptr, T value)
{
    return __sync_fetch_and_add( (__ptr), (value) );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template<typename T>
inline T AtomicSubtract(T volatile* __ptr, T value)
{
    return __sync_fetch_and_sub( (__ptr), (value) );
}
//@}
// --------------------------------------------------------------------------------------  FUNCTION
/*!
Performs an atomic "compare and swap" on the specified address. This works on
4, 8 byte values, and 16 byte values on 64-bit architectures.

\note the order of arguments is from GCC not windows functions. The returned
value is the original value.
\note This only supports 4 byte and 8 byte operands on 32 bit architectures
\note To compare / swap a double pointer please align the set to
XR_ATOMIC_DOUBLE_POINTER_ALIGN. Failure to do so will throw an exception.
Alternatively you can use use the provided AtomicDoublePointer struct.
\note 5+ year old AMD processors may not support the required instruction for,
16 byte CAS. These must be run in 32-bit mode (which is probably not a
major issue given their age.)
*/
template<typename T>
inline T AtomicCompareAndSwap(T volatile * __ptr, const T &__comparand, const T &__replacement)
{
    return 0;
}

#else
#error "Need atomic funcitons for this compiler."
#endif
}}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif //#ifndef XR_CORE_THREADING_ATOMIC_H
