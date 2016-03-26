// ######################################################################################### - FILE
/*! \file
\page corethread Threading System

The threading system provide a basic threading abstractions.
\li \ref xr::Core::Thread
\li \ref xr::Core::Mutex
\li \ref xr::Core::ThreadLocalStorage
\li \ref xr::Core::Monitor
\li \ref coreatomic
\li \ref xr::Core::Queue (Thread safe, Blocking Queue)
\li \ref xr::Core::Stack (Thread safe, Blocking Stack)

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_THREADING_THREAD_H
#define XR_CORE_THREADING_THREAD_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif
#include "xr/core/threading/monitor.h"
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
/*!  \brief Class to Wrap basic Thread functionality.

    \note that there is no function to forcibly end a thread. Threads are
    expected to exit normally. There really is no safe way to end a thread,
    nor is there a guaranteed mechanism to do so even unsafely in all
    architectures. Instead there is a voluntary system of requesting a quit.

    Exrmple:
    \code
    class MyThread : public xr::CoreThread::Thread{
        int Run()
        {
            while(!IsQuitRequested())
            {
            // Do something useful.
            }
            return 0;
        }
    };

    function main()
    {
        MyThread temp("TestThread");
        temp.Start();
        temp.Join();
    }
    \endcode
*/
// ***************************************************************************************** - TYPE
class Thread{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Platform independent Thread ID type
    // ------------------------------------------------------------------------------------  MEMBER
    typedef uintptr_t ThreadID;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Platform independent Thread priority type
    // ------------------------------------------------------------------------------------  MEMBER
    typedef uintptr_t ThreadPriority;


    // ------------------------------------------------------------------------------------  MEMBER
    /// Constructor. Can set name here or after the fact.
    Thread(const char * name);
    /// Constructor. Name should be provided later.
    Thread();
    /// virtual destructor
    virtual ~Thread();
    // ------------------------------------------------------------------------------------  MEMBER

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Start the thread.
    /// \warning If the thread fails to start, the process will fail.
    // ------------------------------------------------------------------------------------  MEMBER
    void Start();

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Return an ID to use in other xr::Core::Thread calls.
    /// The returned ID is not guaranteed to map to any system ID.
    /// \return Return this Thread's ID
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_SIDE_EFFECTS_PREFIX
    ThreadID       GetID() const XR_NO_SIDE_EFFECTS_POSTFIX;
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Return this thread's present priority
    /// \return Return this thread's present priority
    /// \sa SetPriority
    // ------------------------------------------------------------------------------------  MEMBER
    ThreadPriority GetPriority() const;
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Return the thread's present name
    /// \note The name may not be reflected in the system / debugger depending
    //        on the environment
    /// \return Return the thread's present name
    /// \sa SetName
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_SIDE_EFFECTS_PREFIX
    const char *    GetName() const XR_NO_SIDE_EFFECTS_POSTFIX;

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Set the present Thread's Name
    /// \note Setting a thread's name in the system / debugger may not be
    ///       possible on all systems
    /// \param[in] newName
    // ------------------------------------------------------------------------------------  MEMBER
    void    SetName(const char * newName);
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief returns whether the thread could have started yet.
    /// \retval true if the thread has started
    /// \retval false if the thread has not yet started
    // ------------------------------------------------------------------------------------  MEMBER
    bool HasStarted() {return mHasStarted;}
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief returns the current execution status.
    /// \note This is Set to true after the run function completes (system thread may not have actually exited)
    /// \retval true if the thread has completed
    /// \retval false if the thread is still running
    // ------------------------------------------------------------------------------------  MEMBER
    bool HasCompleted();
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief returns the value returned by the thread function.
    // ------------------------------------------------------------------------------------  MEMBER
    uintptr_t GetReturnCode();

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Blocks the calling thread until the referenced thread exits
    /// \note Will log an error if called from the thread it is attempting to join (and then return)
    // ------------------------------------------------------------------------------------  MEMBER
    void Join();

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Blocks the calling thread until the referenced thread exits ot timeout expires
    /// \note Will log an error if called from the thread it is attempting to join (and then return)
    /// \return true = thread was joined, false meand timeout occurred.
    /// \param timeout_ms
    // ------------------------------------------------------------------------------------  MEMBER
    bool Join(uint32_t timeout_ms);

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Set a new priority
    /// \param newPriority
    /// \sa GetPriority
    // ------------------------------------------------------------------------------------  MEMBER
    void SetPriority(ThreadPriority newPriority);

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Call this to request a cooperative quit. Client code should check this value periodically
    /// \sa IsQuitRequested
    // ------------------------------------------------------------------------------------  MEMBER
    virtual void RequestQuit() { mHasRequestedQuit = true; }

    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief returns true if RequestQuit has been called.
    /// \sa RequestQuit
    // ------------------------------------------------------------------------------------  MEMBER
    virtual bool IsQuitRequested() {return mHasRequestedQuit; }


    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief return the current Thread Object
    // ------------------------------------------------------------------------------------  MEMBER
    static Thread       & GetCurrentThread();
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief return the current Thread ID
    // ------------------------------------------------------------------------------------  MEMBER
    static ThreadID       GetCurrentThreadID();
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief return the current Thread Name
    // ------------------------------------------------------------------------------------  MEMBER
    static const char   * GetCurrentThreadName();
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief return the current Thread Priority
    // ------------------------------------------------------------------------------------  MEMBER
    static ThreadPriority GetCurrentPriority();
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Yield the Calling Thread
    /// \note would be called "Yield", but Windows defines that as a macro in winbase.h, grumble grumble.
    // ------------------------------------------------------------------------------------  MEMBER
    static void YieldCurrentThread();
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Yield the Calling Thread for passed timeout
    /// \note Timing is only a guidance and is not guaranteed.
    /// Specifically the thread may return much later or even much earlier
    /// then is requested. For consistent results, use a better mechanism (Mutex, semaphore, etc)
    // ------------------------------------------------------------------------------------  MEMBER
    static void YieldCurrentThread(uint32_t timeout_ms);

protected:
    // ------------------------------------------------------------------------------------  MEMBER
    /// \brief Run Function OVerridden by derived classes
    /// \return Thread Return Value
    // ------------------------------------------------------------------------------------  MEMBER
    virtual uintptr_t Run() = 0;

private:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Internal function to deal with cross platform thread completion
    /// work.
    // ------------------------------------------------------------------------------------  MEMBER
    void OnExit(uintptr_t retValue);

    // ------------------------------------------------------------------------------------  MEMBER
    /// Thread Entry function (platform specific signature)
    // ------------------------------------------------------------------------------------  MEMBER
#if defined(XR_PLATFORM_WINDOWS)
    static uint32_t __stdcall ThreadEntry(void *);
#elif defined(_POSIX_THREADS)
    static void * ThreadEntry(void *);
#endif

    // ------------------------------------------------------------------------------------  MEMBER
    /// Is Set to true after the run function completes (system thread may not have actually exited)
    // ------------------------------------------------------------------------------------  MEMBER
    volatile bool      mHasExited;
    /// There is a brief timing issue between monitor broadcast and thread
    /// destruction, it is possible for a thread to Join() and then destroy
    /// the thread object while the monitor is still marked as in use by the
    /// broadcast operation, this "2" var ensures the monitor is free.
    volatile bool      mHasExited2;
    volatile uintptr_t mExitCode;
    xr::Core::Monitor  mMonitor;

    // ------------------------------------------------------------------------------------  MEMBER
    /// Name of thread
    /// \sa GetName()
    /// \sa SetName(const char *)
    // ------------------------------------------------------------------------------------  MEMBER
    const char * mName;
    // ------------------------------------------------------------------------------------  MEMBER
    /// ID of Thread, can be used in OS calls if needed, but such use is generally not needed.
    /// \sa GetID()
    // ------------------------------------------------------------------------------------  MEMBER
    ThreadID     mID;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    static const ThreadID kDefaultThreadID;
#if defined(XR_PLATFORM_WINDOWS)
    // ------------------------------------------------------------------------------------  MEMBER
    /// On windows we need a handle for many operations, cache one off here.
    /// This is not publicly available, nor does it need to be as it can be obtained readily from the ID.
    uintptr_t    mHandle;
    // ------------------------------------------------------------------------------------  MEMBER
#endif
    // ------------------------------------------------------------------------------------  MEMBER
    /// Is Set to True by the created thread prior to calling the "run" function
    // ------------------------------------------------------------------------------------  MEMBER
    volatile bool mHasStarted;

    // ------------------------------------------------------------------------------------  MEMBER
    /// Is Set to True by RequestQuit()
    /// /sa RequestQuit
    /// /sa IsQuitRequested
    // ------------------------------------------------------------------------------------  MEMBER
    volatile bool mHasRequestedQuit;
    // ------------------------------------------------------------------------------------  MEMBER
    // This is an internal object that is created when you call
    // GetCurrentThread from a thread not created via this system.
    // ------------------------------------------------------------------------------------  MEMBER
    friend class SystemCreatedThreadWrapper;
};
}}
#endif //#ifndef XR_CORE_THREADING_THREAD_H
