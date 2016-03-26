// ######################################################################################### - FILE
/*! \file
    A Monitor (also known as a convition variable) is a means of
    notification.
    \sa http://en.wikipedia.org/wiki/Monitor_%28synchronization%29

    Both POSIX and Windows provide functionally identical condition variable
    implementations. This is a wrapper over that functionality.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_THREADING_MONITOR_H
#define XR_CORE_THREADING_MONITOR_H

#if defined( _MSC_VER )
#pragma once
#endif

// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

#if defined(XR_PLATFORM_WINDOWS)
// Convenience type (size compatible with the windows definition)
// compatibility is verified via a compile time assert in the cpp file.

struct XR_INTERNAL_RTL_CONDITION_VARIABLE_SLIST {
private: // Don't access directly, these are undocumented, but fair game in debugging.
    /* 00 */ struct XR_INTERNAL_RTL_CONDITION_VARIABLE_SLIST * pNext;
#if XR_PLATFORM_PTR_SIZE == 4
    /* 04 */ uint32_t padding04;
#endif
    /* 08 */ uint64_t unknown08; // This is a pointer to another item in the list. not sure why. It is modified in the Optimize Routine.
    /* 10 */ uint64_t unknown10; // This is a pointer to another item in the list. not sure why. It is modified in the Optimize Routine.
    /* 18 */ uint32_t WaitingThreadID;
    /* 1c */ uint32_t unknown1c;
    /* 20 */ uint32_t unknown20; // This appears to be uninitialized
    /* 24 */ uint32_t isSleeping;
};

struct XR_INTERNAL_RTL_CONDITION_VARIABLE { 
private: // Don't access directly, these are undocumented, but fair game in debugging.
    uintptr_t WakeCount : 3;
    uintptr_t Waking : 1;
    uintptr_t Value : (XR_PLATFORM_PTR_SIZE * 8) - 4;
};

struct _RTL_CONDITION_VARIABLE;
#else

#include <unistd.h>
#if defined(_POSIX_THREADS)
// condition variable is defined in pthread.h
#include <pthread.h>
#endif

#endif

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE
namespace xr{
    namespace Core
    {
        class Mutex;
        class RecursiveMutex;
    }
}

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// ***************************************************************************************** - TYPE
/*! \brief Monitor (ConditionVariable) implementation.
    \copydoc monitor.h
    \sa http://en.wikipedia.org/wiki/Monitor_%28synchronization%29
*/
// ***************************************************************************************** - TYPE
class Monitor{
public:
    Monitor();
    ~Monitor();

    // ------------------------------------------------------------------------------------  MEMBER
    /// Wait for Signal.
    /// \warning Caller must have *already* locked the passed mutex!
    // ------------------------------------------------------------------------------------  MEMBER
    void Wait(XR_IN xr::Core::Mutex & mutex) const ;
    void Wait(XR_IN xr::Core::RecursiveMutex & mutex) const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Signal a single Thread
    // ------------------------------------------------------------------------------------  MEMBER
    void Signal() const ;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Signal a All Threads
    // ------------------------------------------------------------------------------------  MEMBER
    void Broadcast() const ;

    // ------------------------------------------------------------------------------------  MEMBER
    // These return the underlying system types for custom code.
    // These are not often used, but necessary in some circumstances.
    // ------------------------------------------------------------------------------------  MEMBER
#if defined(XR_PLATFORM_WINDOWS)
    inline _RTL_CONDITION_VARIABLE *UnderlyingSystemObject()
    {
        return  (_RTL_CONDITION_VARIABLE*)&mCondition;
    }
#elif defined(_POSIX_THREADS)
    inline pthread_cond_t         *UnderlyingSystemObject()
    {
        return &mCondition;
    }
#endif

private:
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
#if defined(XR_PLATFORM_WINDOWS)
    struct XR_INTERNAL_RTL_CONDITION_VARIABLE mCondition;
#elif defined(_POSIX_THREADS)
    mutable pthread_cond_t   mCondition;
#endif

};
}}
#endif //#ifndef XR_CORE_THREADING_MONITOR_H
