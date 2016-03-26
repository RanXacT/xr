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
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_CONSOLE_H
#include "xr/core/console.h"
#endif

#if defined(XR_PLATFORM_WINDOWS)
#include "xr/platform/windows_lite.h"
#elif defined(XR_PLATFORM_DARWIN)
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/thread_status.h>
#include <mach/exception.h>
#include <mach/task.h>
#include <pthread.h>

static mach_port_t exception_port;

#elif defined(XR_PLATFORM_LINUX)
#include <signal.h>
#endif
// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
#if defined(XR_PLATFORM_WINDOWS)
static long WINAPI UnhandledExceptionFilter(struct EXCEPTION_POINTERS *)
{
    XR_ALWAYS_FAIL_M("Unhandled Exception occurred");
    return EXCEPTION_CONTINUE_SEARCH;
}
#elif defined(XR_PLATFORM_DARWIN)
static void *ExceptionThread(void *)
{
    mach_msg_return_t r;


    struct {
        mach_msg_header_t head;
        mach_msg_body_t msgh_body;
        char data[1024];
    } msg;

    for(;;)
    {
        r = mach_msg(
            &msg.head,
            MACH_RCV_MSG|MACH_RCV_LARGE,
            0,
            sizeof(msg),
            exception_port,
            MACH_MSG_TIMEOUT_NONE,
            MACH_PORT_NULL);
        if(r != MACH_MSG_SUCCESS)
        {
            XR_ALWAYS_FAIL_M("Exception Handler: mach_msg failed");
        }

        if(::xr::Core::IsDebuggerConnected())
        {
            // Send the message on to the debugger.
            r = mach_msg(
                            &msg.head,
                            MACH_SEND_MSG,
                            msg.head.msgh_size,
                            0,
                            MACH_PORT_NULL,
                            MACH_MSG_TIMEOUT_NONE,
                            MACH_PORT_NULL);
            if(r != MACH_MSG_SUCCESS)
            {
                XR_ALWAYS_FAIL_M("mach_msg failed");
            }

            // end the thread (debugger is attached, what more do we want?)
            // If there is a reason to not end this thread (i.e. to handle
            // case where debugger is detached), then there will need to
            // be more testing, so far such testing got weird errors,
            // although admittedly I've only done minimal work with exception
            // ports so a fix may be simple.
            return 0;
        }

        XR_ALWAYS_FAIL_M("Exception Raised.")
    }
}

#elif defined(XR_PLATFORM_LINUX)
/* The signal handler just clears the flag and re-enables itself. */
void ExceptionHandler(int sig)
{
    XR_ALWAYS_FAIL_M("Exception Raised.");
}
#endif

// ***************************************************************************************** - TYPE
// Default assert handler. Prints to stderr and then halts the program.
// ***************************************************************************************** - TYPE
class DefaultAssertHandler: public xr::Core::IUnknownHelper1<DefaultAssertHandler, ::xr::Core::IAssertFailedHandler, Core::NotRefCounted>
{
public:
    XR_COM_CLASS_ID(0x0d4ffe1);

#if defined(XR_PLATFORM_WINDOWS)
    DefaultAssertHandler()
    {
        ::SetUnhandledExceptionFilter((void*)UnhandledExceptionFilter);
    }
#elif defined(XR_PLATFORM_DARWIN)
    DefaultAssertHandler()
    {
        if(::xr::Core::IsDebuggerConnected())
        {
            // Don't do anything if the debugger is connected, will just cause problems.
            return;
        }
        kern_return_t r;
        mach_port_t me;
        pthread_t thread;
        pthread_attr_t attr;
        exception_mask_t mask;

        me = mach_task_self();
        r = mach_port_allocate(me,MACH_PORT_RIGHT_RECEIVE,&exception_port);
        if(r != MACH_MSG_SUCCESS)
        {
            XR_ALWAYS_FAIL_M("mach_port_allocate failed");
        }

        r = mach_port_insert_right(me,exception_port,exception_port,
            MACH_MSG_TYPE_MAKE_SEND);
        if(r != MACH_MSG_SUCCESS)
        {
            XR_ALWAYS_FAIL_M("mach_port_insert_right");
        }

        // These are the events we handle
        mask = EXC_MASK_BAD_ACCESS | EXC_MASK_CRASH | EXC_MASK_BAD_INSTRUCTION;

        /* set the new exception ports */
        r = task_set_exception_ports(
            me,
            mask,
            exception_port,
            EXCEPTION_DEFAULT,
            MACHINE_THREAD_STATE
            );
        if(r != MACH_MSG_SUCCESS)
        {
            XR_ALWAYS_FAIL_M("task_set_exception_ports");
        }

        if(pthread_attr_init(&attr) != 0)
        {
            XR_ALWAYS_FAIL_M("pthread_attr_init");
        }
        if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) != 0)
        {
            XR_ALWAYS_FAIL_M("pthread_attr_setdetachedstate");
        }

        if(pthread_create(&thread,&attr,ExceptionThread,nullptr) != 0)
        {
            XR_ALWAYS_FAIL_M("pthread_create");
        }
        pthread_attr_destroy(&attr);
    }
#elif defined(XR_PLATFORM_LINUX)
    DefaultAssertHandler()
    {
        signal (SIGILL,  ExceptionHandler);
        signal (SIGSEGV, ExceptionHandler);
        signal (SIGBUS,  ExceptionHandler);
    }
#endif
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    AssertFailAction AssertFailed(
                                            AssertCallType type,
                                            const char * filename,
                                            const int32_t lineNumber,
                                            const char * message) XR_OVERRIDE;
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    AssertFailAction AssertFailedFormatted(
                                        AssertCallType type,
                                        const char * filename,
                                        const int32_t lineNumber,
#if defined(XR_COMPILER_MICROSOFT)
                                        _Printf_format_string_
#endif
                                        const char * format,
                                        ...) XR_OVERRIDE;
private:
};

// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
::xr::Core::AssertFailAction DefaultAssertHandler::AssertFailed(
                                        ::xr::Core::AssertCallType type,
                                        const char * filename,
                                        const int32_t lineNumber,
                                        const char * message)
{
    // Print: "file(line): Assert|Expect - Message"
    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
        XR_EOL "%s(%" XR_INT32_PRINT "): %s - %s" XR_EOL,
        filename,
        lineNumber,
        type == ::xr::Core::kCallAssert ? "Assertion failed" : "Expectation failed",
        message);
    return type == ::xr::Core::kCallAssert ? ::xr::Core::kActionBreak : ::xr::Core::kActionContinue;
}
// --------------------------------------------------------------------------------------  FUNCTION
/*!  */
// --------------------------------------------------------------------------------------  FUNCTION
::xr::Core::AssertFailAction DefaultAssertHandler::AssertFailedFormatted(
                                        ::xr::Core::AssertCallType type,
                                        const char * filename,
                                        const int32_t lineNumber,
#if defined(XR_COMPILER_MICROSOFT)
                                        _Printf_format_string_
#endif
                                        const char * format,
                                        ...)
{
    // Print: "file(line): Assert|Expect - Message"

    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr,
        XR_EOL "%s(%" XR_INT32_PRINT "): %s - " ,
        filename,
        lineNumber,
        type == ::xr::Core::kCallAssert ? "Assertion failed" : "Expectation failed");

    va_list args;
    va_start(args,format);

    xr::Core::vConsolePrintf(xr::Core::kConsoleStdErr, format, args);

    va_end(args);

    xr::Core::ConsolePrintf(xr::Core::kConsoleStdErr, XR_EOL);

    return type == ::xr::Core::kCallAssert ? ::xr::Core::kActionBreak : ::xr::Core::kActionContinue;
}

static DefaultAssertHandler     gDefaultAssertHandler;
IAssertFailedHandler * Assert::sAssertFailHandler = &gDefaultAssertHandler;

// --------------------------------------------------------------------------------------  FUNCTION
/*! Sets the currently registered assert handler (returns the old value).*/
// --------------------------------------------------------------------------------------  FUNCTION
IAssertFailedHandler & Assert::SetFailHandler(IAssertFailedHandler & newHandler)
{
    IAssertFailedHandler & temp = *sAssertFailHandler;
    sAssertFailHandler = &newHandler;
    return temp;
}
// --------------------------------------------------------------------------------------  FUNCTION
/// return the currently registered assert failed handler.
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
IAssertFailedHandler & Assert::GetFailHandler()
{
    return *sAssertFailHandler;
}

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
