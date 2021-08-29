// ######################################################################################### - FILE
/*!

\page scheduling Scheduling Service
The Scheduling::Manager takes a function or functor Object and returns a JobHandle for it.
The Job handle can be waited on or passed as a dependency (antecedent) to another job. 
Note that although multiple schedulers can be created and maintained, generally speaking 
this will result in lower performance and it is recommended to have only one scheduler per 
running process.

\par Jobs
Jobs are pieces of work which are not explicitly tied to any given thread. They
run at the direction of the scheduler. They should typically be relatively
short in duration (a few mS). Since they do not have a fixed thread context they
are much faster to create then a thread, but if a job takes 100's or 1000's of
mS, then a thread may be better suited.

\par Interaction
There are three ways to submit work to the scheduler.
\li Via Static functions of Type "Runnable" with an "Arguments" parameter
\li Via lambda function which captures no more than "Arguments" size of data from caller
\li Via a functor object which defines a operator() function and whose size is no more 
than the size of an "Arguments" object

Note that in all cases one should take great care to ensure the data passed does not 
go out of scope or get freed before the completion of the job. The job system will 
copy the arguments themselves (or the lambda object and captured values) and maintain those 
values. Any pointers or references passed to the job however are the responsibility of the 
user to ensure they are not deleted.

\file
\brief Job Scheduler Implementation
\copydoc scheduling

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_SERVICES_SCHEDULING_H
#define XR_SERVICES_SCHEDULING_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif
#ifndef XR_CORE_RUNNABLE_H
#include "xr/core/runnable.h"
#endif

#include <type_traits>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127)  // Conditional expression is constant
#endif

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Scheduling {
class IManager;
class JobInstance;
class JobHandle;
class JobThread;
}}
// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Scheduling {

// ***************************************************************************************** - TYPE
/*! Job based Completion specialization.
    */
// ***************************************************************************************** - TYPE
class JobHandle : public ::xr::Core::Completion
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Returns true if the job is done at that instant.
    // ------------------------------------------------------------------------------------  MEMBER
    bool           IsDone()  const;
    // ------------------------------------------------------------------------------------  MEMBER
    /// This returns only once the job is completed.
    // ------------------------------------------------------------------------------------  MEMBER
    void           WaitOn()  const;
    // ------------------------------------------------------------------------------------  MEMBER
    /// returns true if the JobHandle is valid. This is useful for asserts.
    // ------------------------------------------------------------------------------------  MEMBER
    inline bool    IsValid() const;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Invalidates the JobHandle, useful for asserts.
    // ------------------------------------------------------------------------------------  MEMBER
    inline void    Invalidate();
    // ------------------------------------------------------------------------------------  MEMBER
    /// Adds an Runnable event to the conclusion of th job. (may run
    /// immediately).
    // ------------------------------------------------------------------------------------  MEMBER
    inline void    AddCompletionRunnable(::xr::Core::Runnable runnable);

    // ------------------------------------------------------------------------------------  MEMBER
    /// internally used sentinel
    // ------------------------------------------------------------------------------------  MEMBER
    static const uint64_t kJobInstanceHandleInvalid = XR_UINT64_MAX;
    // ------------------------------------------------------------------------------------  MEMBER
    /// default constructor initializes it with explicit "invalid" value
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandle(): mXID(kJobInstanceHandleInvalid) {}
    // ------------------------------------------------------------------------------------  MEMBER
    /// constructor binding to a specific job.
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandle( uint64_t id, JobInstance * instance): mInstance(instance), mXID(id) {}

public:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Pointer to internal JobInstance from scheduler. The JobInstance
    /// implementation is not exposed publicly.
    // ------------------------------------------------------------------------------------  MEMBER
    JobInstance  * mInstance;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Unique identifier for linked job.
    // ------------------------------------------------------------------------------------  MEMBER
    uint64_t       mXID;
};


// ***************************************************************************************** - TYPE
/*! Special case Job handle used to manage jobs which are started out as
    blocked. This prevents usage by a non blocked handle. Call
    "ReleaseBarrier" when you are done adding barriers as when you obtain
    the JobHandleBlocked instance it starts with a barrier (so the job does
    not start before you set it up).
    */
// ***************************************************************************************** - TYPE
class JobHandleBlocked : public JobHandle
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Adds a barrier to the running of the job, return a Runnable that
    /// will clear. The runnable must be called \a count times. (
    /// ReleaseBarrier can also be used instead of the returned runnable)
    /// \note that \a args is not an array there is only one Args instance
    /// and the returned runnable should be run with that args object
    /// \a count times.
    // ------------------------------------------------------------------------------------  MEMBER
    Core::Runnable AddBarrier(xr::Core::Arguments &args, size_t count = 1)  const;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Releases a barrier
    // ------------------------------------------------------------------------------------  MEMBER
    void ReleaseBarrier(size_t count = 1);
    // ------------------------------------------------------------------------------------  MEMBER
    /// default constructor initializes it with explicit "invalid" value
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandleBlocked(): JobHandle() {}

    // ------------------------------------------------------------------------------------  MEMBER
    /// INTERNAL: constructor binding to a specific job.
    // ------------------------------------------------------------------------------------  MEMBER
    JobHandleBlocked( const JobHandle &jh): JobHandle(jh.mXID,jh.mInstance) {}

private:

};

// ***************************************************************************************** - TYPE
/*! \copydoc scheduling
    */
// ***************************************************************************************** - TYPE
class XR_PURE_INTERFACE IManager{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    virtual ~IManager() {}
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Create a ready job for this runnable. */
    // ------------------------------------------------------------------------------------  MEMBER
    virtual JobHandle InsertReady(
        Core::Runnable r,
        const Core::Arguments *args) = 0;

    // ------------------------------------------------------------------------------------  MEMBER
    /*! Create a ready job for this runnable. */
    // ------------------------------------------------------------------------------------  MEMBER
    template <class T>
    inline 
    JobHandle InsertReady(T lambda);

    // ------------------------------------------------------------------------------------  MEMBER
    /*! Create a ready job for this runnable. */
    // ------------------------------------------------------------------------------------  MEMBER
    inline 
    JobHandle InsertReady(xr::Core::Runnable r);

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Create Ready jobs for the array of Runnable Objects. \a r0 is a pointer to
    an array of Runnable objects (of size \a runnableCount). The returned job handle
    waits on the completion of all of the jobs.
    \note this can be much more faster than Inserting individually.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    virtual JobHandle InsertReady(
        size_t runnableCount,
        Core::Runnable* runnableArray,
        size_t argumentsCount = 0,                  ///< Can be 0 (no args), 1 (same args for every runnable), or runnableCount (each has it's own)
        const Core::Arguments * argsArray = nullptr) = 0;

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Insert the runnable, but make sure it does not run until enabled. */
    // ------------------------------------------------------------------------------------  MEMBER
    virtual JobHandleBlocked InsertBlocked(
        Core::Runnable r,
        const Core::Arguments *args) = 0;

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Insert the runnable, but make sure it does not run until enabled. */
    // ------------------------------------------------------------------------------------  MEMBER
    template <class T>
    inline 
    JobHandleBlocked InsertBlocked(T lambda);

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Insert the runnable, but make sure it does not run until enabled. */
    // ------------------------------------------------------------------------------------  MEMBER
    inline 
    JobHandleBlocked InsertBlocked(xr::Core::Runnable r);

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Create Ready jobs for the array of Runnable Objects. \a r0 is a pointer to
    an array of Runnable objects (of size \a runnableCount). The returned job handle
    waits on the completion of all of the jobs.
    \note this can be much more faster than Inserting individually.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    virtual JobHandleBlocked InsertBlocked(
        size_t runnableCount,
        Core::Runnable* runnableArray,
        size_t argumentsCount = 0,                  ///< Can be 0 (no args), 1 (same args for every runnable), or runnableCount (each has it's own)
        const Core::Arguments * argsArray = nullptr) = 0;

    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Insert the runnable, but make sure it does not run until after
            the passed JobHandle objects are complete. \a handle0 is an array
            of JobHandles (of size \a handleCount). This is a convenience
            function.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    virtual  JobHandle InsertAfter(
        Core::Runnable r,
        const Core::Arguments *args,
        JobHandle * antecedentArray,
        size_t arrayCount) = 0;
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  Insert the runnable, but make sure it does not run until after
            the passed JobHandle objects are complete. \a handle0 is an array
            of JobHandles (of size \a handleCount). This is a convenience
            function.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    template<typename T>
    inline
    JobHandle InsertAfter(
        T lambda,
        JobHandle * antecedentArray,
        size_t arrayCount);

    // ------------------------------------------------------------------------------------  MEMBER
    /// Wraps initialization options for Manager object
    // ------------------------------------------------------------------------------------  MEMBER
    struct InitializeOptions{
        size_t mNumThreads;         ///< Number of threads the Scheduler should create (may be ignored depending on system)
        size_t mReadyListSize;      ///< Maximum Number of outstanding jobs at any instant (if too low will cause blocking, and potentially deadlock)
        size_t mFreeListSize;       ///< Number of job instances to create in total.
    };
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Initializes the a scheduler instance with the passed \a options.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    static IManager * Initialize(InitializeOptions * options);

    // ------------------------------------------------------------------------------------  MEMBER
    /*! destroys the passed scheduler instance. */
    // ------------------------------------------------------------------------------------  MEMBER
    static void Shutdown(IManager * sched);
};

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template <class T>
inline 
JobHandle IManager::InsertReady(T lambda)
{
    typedef void (T::*ExpectedFunctionType)() const;

    // Compile time type validation
    static_assert( sizeof(T) <= sizeof(xr::Core::Arguments), "Object size exceed the size of the \"Arguments\" Object. In the case of Lamba functions, this consists of captured variables" );
    static_assert( std::is_convertible<ExpectedFunctionType,decltype(&T::operator())>::value, "Scheduler cannot take Lambdas with arguments, use captures instead." );
    static_assert( std::is_trivially_destructible<T>::value, "Can only pass trivially destructed object types to job." );

    // This is not supported by GCC at this time.
    //static_assert( std::is_trivially_copyable<T>::value, "Types must be trivially copyable to pass to job.");

    xr::Core::Arguments a;

    // This casts to the arguments.
    uintptr_t * x = (uintptr_t *)&lambda;

    if(sizeof(lambda) >0)
        a.a0 = x[0];
    if(sizeof(lambda) > sizeof(uintptr_t))
        a.a1 = x[1];
    if(sizeof(lambda) > (sizeof(uintptr_t)*2))
        a.a2 = x[2];
    if(sizeof(lambda) > (sizeof(uintptr_t)*3))
        a.a3 = x[3];

    ExpectedFunctionType f = &T::operator();

    //This casts to the function pointer
    union {
        ExpectedFunctionType e;
        Core::Runnable       r;
    } u;
    u.e = f;

    // This inserts the pair
    return InsertReady( u.r, &a);
}


// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline 
JobHandle IManager::InsertReady(xr::Core::Runnable r)
{
    return InsertReady( r, nullptr);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template <class T>
inline 
JobHandleBlocked IManager::InsertBlocked(T lambda)
{
    typedef void (T::*ExpectedFunctionType)() const;
    static_assert( sizeof(T) <= sizeof(xr::Core::Arguments), "Captured variables exceed the size of the \"Arguments\" Object. Refactor and capture less data" );
    static_assert( std::is_convertible<ExpectedFunctionType,decltype(&T::operator())>::value, "Scheduler cannot take Lambdas with arguments, use captures instead." );
    static_assert( std::is_trivially_destructible<T>::value, "Can only pass trivially destructed object types to job." );

    xr::Core::Arguments a;

    // This casts to the arguments.
    uintptr_t * x = (uintptr_t *)&lambda;

    if(sizeof(lambda) >0)
        a.a0 = x[0];
    if(sizeof(lambda) > sizeof(uintptr_t))
        a.a1 = x[1];
    if(sizeof(lambda) > (sizeof(uintptr_t)*2))
        a.a2 = x[2];
    if(sizeof(lambda) > (sizeof(uintptr_t)*3))
        a.a3 = x[3];

    ExpectedFunctionType f = &T::operator();

    //This casts to the function pointer
    union {
        ExpectedFunctionType e;
        Core::Runnable       r;
    } u;
    u.e = f;

    // This inserts the pair
    return InsertBlocked( u.r, &a);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
inline 
JobHandleBlocked IManager::InsertBlocked(xr::Core::Runnable r)
{
    return InsertBlocked( r, nullptr);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
template <class T>
inline 
JobHandle IManager::InsertAfter(T lambda, JobHandle * antecedentArray,size_t arrayCount)
{
    typedef void (T::*ExpectedFunctionType)() const;
    static_assert( sizeof(T) <= sizeof(xr::Core::Arguments), "Captured variables exceed the size of the \"Arguments\" Object. Refactor and capture less data" );
    static_assert( std::is_convertible<ExpectedFunctionType,decltype(&T::operator())>::value, "Scheduler cannot take Lambdas with arguments, use captures instead." );
    static_assert( std::is_trivially_destructible<T>::value, "Can only pass trivially destructed object types to job." );

    xr::Core::Arguments a;

    // This casts to the arguments.
    uintptr_t * x = (uintptr_t *)&lambda;

    if(sizeof(lambda) >0)
        a.a0 = x[0];
    if(sizeof(lambda) > sizeof(uintptr_t))
        a.a1 = x[1];
    if(sizeof(lambda) > (sizeof(uintptr_t)*2))
        a.a2 = x[2];
    if(sizeof(lambda) > (sizeof(uintptr_t)*3))
        a.a3 = x[3];

    ExpectedFunctionType f = &T::operator();

    //This casts to the function pointer
    union {
        ExpectedFunctionType e;
        Core::Runnable       r;
    } u;
    u.e = f;

    // This inserts the pair
    return InsertAfter( u.r, &a, antecedentArray, arrayCount);
}

// ***************************************************************************************** - TYPE
// JobHandle inline functions.
// ***************************************************************************************** - TYPE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool           JobHandle::IsValid() const { return mXID != kJobInstanceHandleInvalid; }
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void           JobHandle::Invalidate() { mXID = kJobInstanceHandleInvalid; }



}}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif //#ifndef XR_SERVICES_SCHEDULING_H
