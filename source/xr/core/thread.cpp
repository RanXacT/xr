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
#ifndef XR_CORE_THREADING_THREAD_H
#include "xr/core/threading/thread.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_THREADING_TLS_H
#include "xr/core/threading/tls.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_THREADING_ATOMIC_H
#include "xr/core/threading/atomic.h"
#endif
#if defined(XR_PLATFORM_WINDOWS)
XR_DISABLE_ALL_WARNINGS()
#include <windows.h>
#include <process.h>
XR_RESTORE_ALL_WARNINGS()
#else
// Unix variants.
#if defined(_POSIX_THREADS)
#include <pthread.h>
#if defined(XR_PLATFORM_DARWIN)
#include <sys/time.h>
#endif
#endif
#endif

#include "xr/core/threading/mutex.h"

#include <stdlib.h> // atexit()
// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// Locks the SystemCreatedThreadWrapper list, also locks the
// monitor in posix systems
static Mutex                        sMutex;
static ThreadLocalStorage<Thread*>  sCurrentThread;
const Thread::ThreadID      Thread::kDefaultThreadID = ThreadID(-1);

// ***************************************************************************************** - TYPE
/*! \internal */
// ***************************************************************************************** - TYPE
class SystemCreatedThreadWrapper : public Thread
{
public:
    SystemCreatedThreadWrapper();
	~SystemCreatedThreadWrapper();

    uintptr_t Run() XR_OVERRIDE;

private:
    static void Link(SystemCreatedThreadWrapper * link);
    static void ClearCompleted();
    static void ClearAll();

    static SystemCreatedThreadWrapper * sFirst;
    SystemCreatedThreadWrapper * mNext;
};
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
SystemCreatedThreadWrapper * SystemCreatedThreadWrapper::sFirst = nullptr;
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
uintptr_t SystemCreatedThreadWrapper::Run()
{
    XR_DEBUG_FAIL_M("This should never be called.");
    return 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void SystemCreatedThreadWrapper::Link(SystemCreatedThreadWrapper * link)
{
    sMutex.Lock();
	if(sFirst == nullptr)
	{
		// Register a cleanup handler.
		atexit (ClearAll);
	}

    ClearCompleted();

    SystemCreatedThreadWrapper * temp = sFirst;
    link->mNext = temp;
    sFirst = link;
    sMutex.Unlock();
}
// --------------------------------------------------------------------------------------  FUNCTION
// Must own lock to call this.
// --------------------------------------------------------------------------------------  FUNCTION
void SystemCreatedThreadWrapper::ClearCompleted()
{
    SystemCreatedThreadWrapper * prev = nullptr;
    SystemCreatedThreadWrapper * temp = sFirst;
    while(temp != nullptr)
    {
        if(temp->HasCompleted())
        {
            if(prev == nullptr)
            {
                sFirst = temp->mNext;
            }
            else
            {
                prev->mNext = temp->mNext;
            }
            XR_DELETE(temp);
            temp = prev;
        }

        prev = temp;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6011) //  Dereferencing nullptr pointer 'temp'
#endif
        temp = temp->mNext;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void SystemCreatedThreadWrapper::ClearAll()
{
    // Don't use the mutex here, this is called "atexit" so the static mutex
    // may have been destructed!
    while(sFirst)
    {
        SystemCreatedThreadWrapper * temp;
        temp = sFirst;
        sFirst = temp->mNext;
        XR_DELETE(temp);
    }

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Thread::OnExit(uintptr_t retValue)
{
    // Clear out the TLS reference to this value before the thread gets
    // (potentially) deleted.
    sCurrentThread.SetValue(nullptr);

    // Update local variables
    sMutex.Lock();
    mExitCode = retValue;
    mHasExited = true;
    sMutex.Unlock();
    mMonitor.Broadcast();
    mHasExited2 = true;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool Thread::HasCompleted()
{
    return mHasExited;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
uintptr_t Thread::GetReturnCode()
{
    XR_ASSERT_ALWAYS_EQ(mHasExited, true);
    return mExitCode;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Thread::Join()
{
    // Could check mHasExited for early out if this is a performance issue.
    // which is unlikely.
    sMutex.Lock();
    while(!mHasExited)
    {
        mMonitor.Wait(sMutex);
    }
    sMutex.Unlock();
}

#if defined(XR_PLATFORM_WINDOWS)
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
SystemCreatedThreadWrapper::SystemCreatedThreadWrapper() : Thread() {
    mName = nullptr;
    mID = GetCurrentThreadID();
    mHandle = uintptr_t( OpenThread(SYNCHRONIZE | THREAD_QUERY_INFORMATION , FALSE, DWORD(mID) ));
    XR_ASSERT_ALWAYS_NE_FM(mHandle, uintptr_t(0), "Error: Attach thread object GetLastError:0x%lX", GetLastError());
    if(mHandle == 0)
    {
        mHandle = uintptr_t(INVALID_HANDLE_VALUE);
    }
    // The thread is running!
    mHasStarted = true;
    Link(this);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
SystemCreatedThreadWrapper::~SystemCreatedThreadWrapper()
{
	CloseHandle((HANDLE)mHandle);
}
/*#######################################################################*/
// Some basic validation of a fundamental assumption.
/*#######################################################################*/
static_assert(sizeof(Thread::ThreadID) >= sizeof(DWORD), "size check");

/*#######################################################################*/
/*#######################################################################*/
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::Thread() : mMonitor()
{
    mName       = nullptr;
    mID         = kDefaultThreadID;
    mHandle     = uintptr_t(INVALID_HANDLE_VALUE);
    mHasStarted = false;
    mHasExited = false;
    mHasExited2 = false;

    mExitCode   = 0;
    mHasRequestedQuit = false;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::Thread(const char * name): mMonitor()
{
    mName       = XR_STRDUP(name, "Thread::mName");
    mID         = kDefaultThreadID;
    mHandle     = uintptr_t(INVALID_HANDLE_VALUE);
    mHasStarted = false;
    mHasExited  = false;
    mHasExited2 = false;
    mExitCode   = 0;
    mHasRequestedQuit = false;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::~Thread()
{
    if(mName != nullptr)
    {
        XR_FREE( (void*)mName );
        mName = nullptr;
    }

    if(mID != GetCurrentThreadID() && mHandle != uintptr_t(INVALID_HANDLE_VALUE) )
    {
        // Note that Join() does not actually wait for the thread to
        // actually return, just for the user code in the thread to
        // finish, but before we DELETE the thread object, we need
        // the thread to have actually completed or else there are
        // timing issues with resources during this delete.
        // POSIX deals with this via mHasExited2.
        DWORD waitValue;
        waitValue = WaitForSingleObject(HANDLE(mHandle), INFINITE);
        XR_EXPECT_ALWAYS_EQ(waitValue, WAIT_OBJECT_0);

        BOOL retValue;
        retValue = CloseHandle(HANDLE(mHandle));
        XR_EXPECT_ALWAYS_NE_FM(retValue,0,    "CloseHandle call Failed! GetLastError:0x%lX", GetLastError());

        mHandle = uintptr_t(INVALID_HANDLE_VALUE);

        // This fails for system created thread objects.
        //XR_ASSERT_ALWAYS_EQ(mHasExited2, true);
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Thread::Start()
{
    do
    {
        unsigned tempId;
        mHandle = (uintptr_t)_beginthreadex(nullptr, 0, Thread::ThreadEntry, this, 0, (unsigned *)&tempId);
        XR_ASSERT_ALWAYS_NE_FM(mHandle, 0, "Unable to Create Thread, system is likely out of resources! GetLastError:0x%lX", GetLastError());

        // Shuffling types to deal with any size differences (i.e. if you are
        // not careful you'll periodically get garbage in upper 32 bits)
        ThreadID test = tempId;

        // If this fails that's fine. The other thread got there first.
        xr::Core::AtomicCompareAndSwap(&mID, kDefaultThreadID, test);

    }while(mHandle == 0);
}

#if XR_DEBUG_FEATURES_ENABLED && XR_COMPILER_MICROSOFT
// This is a mostly Undocumented means to set the current thread name in a debugger. I'm not aware of
// a means of doing this via a public API at present. Google "0x406D1388" for more details.
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // must be 0x1000
    LPCSTR szName; // pointer to name (in user addr space)
    DWORD dwThreadID; // thread ID (-1=caller thread)
    DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;
#endif

#if defined(XR_COMPILER_MICROSOFT)
#pragma warning( push )
// These exception related warnings are generated by the code that sets the
// thread name in the debugger. This is debug only code and the warnings are
// intended (required even, in this case).
#pragma warning ( disable : 6312 )
#pragma warning ( disable : 6322 )
#endif

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
uint32_t __stdcall Thread::ThreadEntry(void * threadObject)
{
    Thread * p = static_cast<Thread *>(threadObject);

    ThreadID tempId = GetCurrentThreadID();
    // If this fails that's fine. The other thread got there first.
    xr::Core::AtomicCompareAndSwap(&p->mID, kDefaultThreadID, tempId);

    sCurrentThread.SetValue(p);
    if(p->mName != nullptr)
    {
        // Set the name on the thread before it runs.
#if XR_DEBUG_FEATURES_ENABLED && XR_COMPILER_MICROSOFT
        // Thread name in the debugger can only be set or changed from the thread itself.
        if(IsDebuggerPresent())
        {
            // This is a mostly Undocumented means to set the current thread name in a debugger. I'm not aware of
            // a means of doing this via a public API at present. Google "0x406D1388" for more details.
            const DWORD VISUAL_STUDIO_MAGIC_EXCEPTION_VALUE = 0x406D1388;
            THREADNAME_INFO info;
            info.dwType = 0x1000;
            info.szName = p->mName;
            info.dwThreadID = DWORD(-1); // -1 == current thread
            info.dwFlags = 0;
            __try
            {
                RaiseException( VISUAL_STUDIO_MAGIC_EXCEPTION_VALUE, 0, sizeof(info)/sizeof(DWORD), (const ULONG_PTR*)&info );
            }
            __except (EXCEPTION_CONTINUE_EXECUTION)
            {
            }
        }
#endif
    }

    p->mHasStarted = true;
    uintptr_t retValue = p->Run();
    p->OnExit(retValue);
    return 0;
}

#if defined(XR_COMPILER_MICROSOFT)
#pragma warning( pop )
#endif

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
Thread::ThreadID       Thread::GetID() const
{
    return mID;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::ThreadPriority Thread::GetPriority() const
{
    XR_ASSERT_DEBUG_EQ(mHasStarted, true);
    XR_ASSERT_DEBUG_EQ(mHasExited,  false);
    return (Thread::ThreadPriority)::GetThreadPriority( HANDLE(mHandle) );
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
const char *    Thread::GetName() const
{
    return mName;
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Thread::SetName(const char * newName)
{
    if(mHasStarted)
    {
        // Cannot change after starting.
        // In theory we could, but it would leak memory.
        // the pointer passed to the debugger is modified it appears.
        // And cannot be immediately freed.
        return;
    }
    if(mName != nullptr && mName != newName)
    {
        // replace with String Safe delete
        XR_FREE( (char*)mName );
        mName = nullptr;
    }
    mName = XR_STRDUP(newName, "Thread::mName");
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool Thread::Join(uint32_t timeout_ms)
{
    DWORD ret;
    do
    {
        ret = WaitForSingleObject( HANDLE(mHandle), timeout_ms);
        XR_ASSERT_ALWAYS_NE_FM(ret, WAIT_FAILED, "Error: Unable to Join Thread of Handle:0x%p GetLastError:0x%lX", (void*)(mHandle), GetLastError());

        HANDLE h = HANDLE(mHandle);
        mHandle = uintptr_t(INVALID_HANDLE_VALUE);
        BOOL retValue;
        retValue = CloseHandle(HANDLE(h));
        XR_EXPECT_ALWAYS_NE_FM(retValue,0,    "CloseHandle call Failed! GetLastError:0x%lX", GetLastError());

    }while(ret == WAIT_FAILED);

    XR_ASSERT_ALWAYS_EQ(mHasExited, true);
    XR_ASSERT_ALWAYS_EQ(mHasExited2, true);
    return ret == WAIT_OBJECT_0;
}
// --------------------------------------------------------------------------------------  FUNCTION
void Thread::SetPriority(ThreadPriority newPriority)
{
    XR_ASSERT_DEBUG_EQ(mHasStarted, true);
    XR_ASSERT_DEBUG_EQ(mHasExited,  false);
    // TODO: map priorities appropriately.
    SetThreadPriority(HANDLE(mHandle), int(newPriority));
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread &       Thread::GetCurrentThread()
{
    Thread * temp = sCurrentThread.GetValue();
    if(temp == nullptr)
    {
        // If there is not a Thread instance for the calling thread,
        // create one, they are fairly cheap, and we have at most one per
        // created thread. Be careful not to over use them though you can
        // end up leaking these if you repeatedly create system threads
        // without the Thread class, but then use the Thread class on them.
        // I.e., this is a convenience for the main thread and perhaps
        // threads you cannot control but should be used sparingly.
        temp = XR_NEW( "ThreadManager") SystemCreatedThreadWrapper();
        sCurrentThread.SetValue(temp);
    }
    return *temp;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::ThreadID       Thread::GetCurrentThreadID()
{

    return ::GetCurrentThreadId();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
const char*    Thread::GetCurrentThreadName()
{
    return GetCurrentThread().GetName();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::ThreadPriority Thread::GetCurrentPriority()
{
    return GetCurrentThread().GetPriority();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void            Thread::YieldCurrentThread()
{
    Sleep(0);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void            Thread::YieldCurrentThread(uint32_t timeout_ms)
{
    Sleep(timeout_ms);
}

#elif defined(_POSIX_THREADS)
/*#######################################################################*/
/*#######################################################################*/
// Some basic validation of a fundamental assumption.
static_assert(sizeof(Thread::ThreadID) >= sizeof(pthread_key_t), "size check");

inline void HandleErrno(int errvalue, const char * where)
{
    XR_ASSERT_ALWAYS_EQ_FM(errvalue, 0, "Error: Error returned from %s. errno:%d", where, errvalue);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
SystemCreatedThreadWrapper::SystemCreatedThreadWrapper() : Thread() {
    mName = nullptr;
    mID = xr::Core::Thread::GetCurrentThreadID();
    mHasStarted = true;

    Link(this);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
SystemCreatedThreadWrapper::~SystemCreatedThreadWrapper()
{
	// Nothing to do.
}
/*#######################################################################*/
/*#######################################################################*/
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::Thread() : mMonitor()
{
    mName = nullptr;
    mID   = kDefaultThreadID;
    mHasExited = false;
    mHasExited2 = false;
    mHasStarted = false;
    mHasRequestedQuit = false;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::Thread(const char * name) : mMonitor()
{
    mName = XR_STRDUP(name, "Thread::mName");
    mID   = kDefaultThreadID;
    mHasExited = false;
    mHasExited2 = false;
    mHasStarted = false;
    mHasRequestedQuit = false;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::~Thread()
{
    if(mName != nullptr)
    {
        // replace with String Safe delete
        XR_FREE( const_cast<char*>(mName) );
        mName = nullptr;
    }

    // Make sure that the monitor is REALLY free
    while(mHasExited2 == false)
    {
        YieldCurrentThread();
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Thread::Start()
{
    pthread_attr_t pa;
    int errval = pthread_attr_init(&pa);
    HandleErrno(errval, "pthread_attr_init");

    errval = pthread_attr_setdetachstate(&pa, PTHREAD_CREATE_DETACHED);
    HandleErrno(errval, "pthread_attr_setdetachstate");

    // pthread_attr_setstacksize(&pa, size);
    do
    {
        pthread_t temp;
        errval = pthread_create(&temp, &pa, Thread::ThreadEntry, this);

        ThreadID tempId = (ThreadID)temp;
        // If this fails that's fine. The other thread got there first.
        xr::Core::AtomicCompareAndSwap(&mID, kDefaultThreadID, tempId);

        HandleErrno(errval, "pthread_create");
    }while(errval != 0);

    pthread_attr_destroy(&pa);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void * Thread::ThreadEntry(void * threadObject)
{
    pthread_t temp = pthread_self();
    ThreadID tempId = (ThreadID)temp;

    Thread * p = static_cast<Thread *>(threadObject);

    // If this fails that's fine. The other thread got there first.
    xr::Core::AtomicCompareAndSwap(&p->mID, kDefaultThreadID, tempId);

    p->mHasStarted = true;
    sCurrentThread.SetValue(p);
    uint32_t retValue = p->Run();
    p->OnExit(retValue);
    return 0;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
xr::Core::Thread::ThreadID       Thread::GetID() const
{
    return mID;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
xr::Core::Thread::ThreadPriority Thread::GetPriority() const
{
    sched_param sp;
    int schedType;
    int errval = pthread_getschedparam(pthread_t(mID), &schedType, &sp);
    HandleErrno(errval, "pthread_setschedparam");

    // TODO: translate priority values.

    return sp.sched_priority;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
const char *    Thread::GetName() const
{
    return mName;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Thread::SetName(const char * newName)
{
    if(mName != nullptr)
    {
        // replace with String Safe delete
        XR_FREE( const_cast<char*>(mName) );
        mName = nullptr;
    }
    mName = XR_STRDUP(newName, "Thread::mName");

    // TODO: set the name into the OS
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool Thread::Join(uint32_t timeout_ms)
{
    // Could check mHasExited for early out if this is a performance issue.
    // which is unlikely.

    struct timespec ts;

#if defined(XR_PLATFORM_DARWIN)
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
#else
    clock_gettime(CLOCK_REALTIME, &ts);
#endif

    ts.tv_sec += (timeout_ms / 1000);
    ts.tv_nsec = ((timeout_ms%1000) * 1000 * 1000);


    sMutex.Lock();

    int rc = 0;
    while (!mHasExited && rc == 0)
    {
        rc = pthread_cond_timedwait(mMonitor.UnderlyingSystemObject(), sMutex.UnderlyingSystemObject(), &ts);
    }

    sMutex.Unlock();

    return mHasExited;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void Thread::SetPriority(ThreadPriority newPriority)
{
    sched_param sp;
    // TODO: translate priority values.
    sp.sched_priority = 100; //(PTHREAD_MAX_PRIORITY / PTHREAD_MIN_PRIORITY)/2;
    int errval = pthread_setschedparam(pthread_t(mID), SCHED_RR, &sp);
    HandleErrno(errval, "pthread_setschedparam");
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread *       Thread::GetCurrentThread()
{
    Thread * temp = sCurrentThread.GetValue();
    if(temp == nullptr)
    {
        // If there is not a Thread instance for the calling thread,
        // create one, they are fairly cheap, and we have at most one per
        // created thread.
        temp = XR_NEW( "ThreadManager") SystemCreatedThreadWrapper();
    }
    return temp;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::ThreadID       Thread::GetCurrentThreadID()
{
    return (ThreadID) pthread_self();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
const char*    Thread::GetCurrentThreadName()
{
    return GetCurrentThread()->GetName();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Thread::ThreadPriority Thread::GetCurrentPriority()
{
    return GetCurrentThread()->GetPriority();
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void            Thread::YieldCurrentThread()
{
#if defined(XR_PLATFORM_DARWIN)
    pthread_yield_np();
#else
    pthread_yield ();
#endif
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void            Thread::YieldCurrentThread(uint32_t timeout_ms)
{
    usleep(timeout_ms * 1000);
}
#else // Platform
#error "Need a Thread implementation for this platform (or addit to an existing platform)"
#endif

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
