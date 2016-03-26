// ######################################################################################### - FILE
/*! \file
\brief logging system


\page corelog Log System Overview

Logging is done via named streams, each with independent control of enabled
levels and of output devices used.

\section Streams

Streams are resolved by name using a LogHandle object. The the handles are passed
into the logging macros. Note that creating a LogHandle is an expensive
operation and should be done once per subsystem where possible. Evaluation is
done via a single branch and arguments are only evaluated if the message is
accepted for logging.

\section Usage
Basic usage:
\code
// Do this once.
xr::Core::LogHandle sSystemLogHandle("SystemName");

// Now print as often as you need.
XR_LOG_DEBUG_MESSAGE(&sSystemLogHandle, "Hello World");
XR_LOG_WARNING_FORMATTED(&sSystemLogHandle, "Error from CreateFile %d", GetLastError() );
\endcode


Objects:
\li xr::Core::LogHandle : Obtained for fast "ShouldPrint" function, required by log macros.
\li xr::Core::ILogConsumer : Interface implemented by anyone interested in consuming log messages.

\section Extensibility

The logging system is quite extensible and easily so. One need only implement the
xr::Core::ILogConsumer interface and then bind it to named streams. The tricky
part really is knowing what streams to bind it to. An easy choice is binding it to ""
which effectively binds it to all printed messages. more complexity really requires
a data driven system, but as such a system is far too complex to exist at this low
level and is provided in a higher level library. Should the provided high level system
not suite your needs one could use it as a reference to build their own (while still
leveraging the low level macros and functionality).

Although high level concepts are discussed, they may not even be needed, the default
implementation in the core prints to Stdout and Stderr as appropriate (which may be
all you need)

\section Comparison Comparison to other Systems
There are a number of comparable systems. The only reason a competing solution
was created is because the other systems require memory and setup to work. The xr
logging system is similar in function, but operates fully without any setup
shutdown or dynamic memory. The major difference is decoupling the actual logging
system from the much more complex initialization system (typically via text file
parsing), all other systems I found bound the two together which prevented a truly
simple core layer.

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_LOG_H
#define XR_CORE_LOG_H

#if defined( _MSC_VER )
#pragma once
#endif
// ######################################################################################### - FILE
/* Public Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif

#if defined(XR_COMPILER_MICROSOFT)
// For printf annotation
#include <sal.h>
#endif

// ######################################################################################### - FILE
/* Public Macros */
// ######################################################################################### - FILE
#define XR_LOG_LEVEL_TRACE   0
#define XR_LOG_LEVEL_DEBUG   1
#define XR_LOG_LEVEL_INFO    2
#define XR_LOG_LEVEL_WARNING 3
#define XR_LOG_LEVEL_ERROR   4
#define XR_LOG_LEVEL_FATAL   5

// -----------------------------------------------------------------------------------------  MACRO
/// Standard log macro
// -----------------------------------------------------------------------------------------  MACRO
#define XR_LOG_MESSAGE(handle, level, str)                                    \
{                                                                             \
    const ::xr::Core::LogHandle * _th = handle; \
    if((_th)->ShouldLog(level))                                \
    {                                                                         \
        (_th)->Log(level, XR_CURRENT_FILE, XR_CURRENT_LINE, str); \
    }                                                                         \
}
// -----------------------------------------------------------------------------------------  MACRO
/// Standard log macro (with printf formatted string).
/// Note the resolved string cannot be larger than
/// ::xr::Core::LogHandle::kLogHandleMaxFormattedStringSize
// -----------------------------------------------------------------------------------------  MACRO
#define XR_LOG_FORMATTED(handle, level, ...)                                  \
{                                                                             \
    const ::xr::Core::LogHandle * _th = handle; \
    if((_th)->ShouldLog(level))                                \
    {                                                                         \
        (_th)->LogFormatted(level, XR_CURRENT_FILE, XR_CURRENT_LINE, __VA_ARGS__); \
    }                                                                         \
}
// -----------------------------------------------------------------------------------------  MACRO
/// Print a "const char *" message to all current listeners
//@{
// -----------------------------------------------------------------------------------------  MACRO
#if XR_LOG_LEVEL_TRACE >= XR_LOG_MINIMUM_LEVEL
#   define XR_LOG_TRACE_MESSAGE(handle, str)   XR_LOG_MESSAGE(  handle, ::xr::Core::kLogLevelTrace, str)
#   define XR_LOG_TRACE_FORMATTED(handle, ...) XR_LOG_FORMATTED(handle, ::xr::Core::kLogLevelTrace, __VA_ARGS__)
#else
#   define XR_LOG_TRACE_MESSAGE(handle, str)   ((void)0)
#   define XR_LOG_TRACE_FORMATTED(handle, ...)    ((void)0)
#endif

#if XR_LOG_LEVEL_DEBUG >= XR_LOG_MINIMUM_LEVEL
#   define XR_LOG_DEBUG_MESSAGE(handle, str)   XR_LOG_MESSAGE(  handle, ::xr::Core::kLogLevelDebug, str)
#   define XR_LOG_DEBUG_FORMATTED(handle, ...) XR_LOG_FORMATTED(handle, ::xr::Core::kLogLevelDebug, __VA_ARGS__)
#else
#   define XR_LOG_DEBUG_MESSAGE(handle, str)   ((void)0)
#   define XR_LOG_DEBUG_FORMATTED(handle, ...)    ((void)0)
#endif

#if XR_LOG_LEVEL_INFO >= XR_LOG_MINIMUM_LEVEL
#   define XR_LOG_INFO_MESSAGE(handle, str)   XR_LOG_MESSAGE(  handle, ::xr::Core::kLogLevelInfo, str)
#   define XR_LOG_INFO_FORMATTED(handle, ...) XR_LOG_FORMATTED(handle, ::xr::Core::kLogLevelInfo, __VA_ARGS__)
#else
#   define XR_LOG_INFO_MESSAGE(handle, str)   ((void)0)
#   define XR_LOG_INFO_FORMATTED(handle, ...)    ((void)0)
#endif

#if XR_LOG_LEVEL_WARNING >= XR_LOG_MINIMUM_LEVEL
#   define XR_LOG_WARNING_MESSAGE(handle, str)   XR_LOG_MESSAGE(  handle, ::xr::Core::kLogLevelWarning, str)
#   define XR_LOG_WARNING_FORMATTED(handle, ...) XR_LOG_FORMATTED(handle, ::xr::Core::kLogLevelWarning, __VA_ARGS__)
#else
#   define XR_LOG_WARNING_MESSAGE(handle, str)   ((void)0)
#   define XR_LOG_WARNING_FORMATTED(handle, ...)    ((void)0)
#endif

#if XR_LOG_LEVEL_ERROR >= XR_LOG_MINIMUM_LEVEL
#   define XR_LOG_ERROR_MESSAGE(handle, str)   XR_LOG_MESSAGE(  handle, ::xr::Core::kLogLevelError, str)
#   define XR_LOG_ERROR_FORMATTED(handle, ...) XR_LOG_FORMATTED(handle, ::xr::Core::kLogLevelError, __VA_ARGS__)
#else
#   define XR_LOG_ERROR_MESSAGE(handle, str)   ((void)0)
#   define XR_LOG_ERROR_FORMATTED(handle, ...)    ((void)0)
#endif

#if XR_LOG_LEVEL_FATAL >= XR_LOG_MINIMUM_LEVEL
#   define XR_LOG_FATAL_MESSAGE(handle, str)   XR_LOG_MESSAGE(  handle, ::xr::Core::kLogLevelFatal, str)
#   define XR_LOG_FATAL_FORMATTED(handle, ...) XR_LOG_FORMATTED(handle, ::xr::Core::kLogLevelFatal, __VA_ARGS__)
#else
#   define XR_LOG_FATAL_MESSAGE(handle, str)   ((void)0)
#   define XR_LOG_FATAL_FORMATTED(handle, ...)    ((void)0)
#endif
//@}
// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {
    class RecursiveMutex;
}}
// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {
// ***************************************************************************************** - TYPE
/*! \brief Priority levels are useful for throttling log data.
        higher numbers represent more important data. These should match
        XR_LOG_LEVEL_* macros
    */
// ***************************************************************************************** - TYPE
enum LogLevel
{
    kLogLevelMin          =   0, ///< This is used internally for verification purposes
    kLogLevelTrace        =   XR_LOG_LEVEL_TRACE, ///< Log only intended during unit test runs. Presence in build dependent on XR_LOG_MINIMUM_LEVEL
    kLogLevelDebug        =   XR_LOG_LEVEL_DEBUG, ///< Log only intended for builds with debug features enabled. Presence in build dependent on XR_LOG_MINIMUM_LEVEL
    kLogLevelInfo         =   XR_LOG_LEVEL_INFO, ///< Informational logs. Presence in build dependent on XR_LOG_MINIMUM_LEVEL
    kLogLevelWarning      =   XR_LOG_LEVEL_WARNING, ///< Warning messages. Presence in build dependent on XR_LOG_MINIMUM_LEVEL
    kLogLevelError        =   XR_LOG_LEVEL_ERROR, ///< Error messages. Presence in build dependent on XR_LOG_MINIMUM_LEVEL
    kLogLevelFatal        =   XR_LOG_LEVEL_FATAL, ///< A message with this level will end the process once this message is logged. Retained in all builds
    kLogLevelMax          =   kLogLevelFatal, ///< This is used internally for verification purposes
    kLogLevelNumLevels        ,  ///< This is used for verification purposes
    kLogLevelInvalid      =  -1, ///< Used to denote an invalid value.
};

// ***************************************************************************************** - TYPE
/// Interface for consuming enabled log print statements.
// ***************************************************************************************** - TYPE
class XR_PURE_INTERFACE ILogConsumer
{
public:
    virtual ~ILogConsumer() {}

    /// Print an ASCII string
    virtual void Log(LogLevel level, const char * file, int32_t line, const char    * message) = 0;
    /// Print a wide string. Note that how "wide" is system dependent
    /// Windows treats wchar_t as USC-2 (2 byte), and unix treats it as USC-4
    /// (4 byte) characters
    virtual void Log(LogLevel level, const char * file, int32_t line, const wchar_t * messageWide) = 0;
};

// ***************************************************************************************** - TYPE
/// Default log consumer using basic console out.
/// will color error output (red) on the console if it supports it.
// ***************************************************************************************** - TYPE
class LogConsumerConsole : public ILogConsumer
{
public:
    void Log(LogLevel level, const char * file, int32_t line, const char    * message) XR_OVERRIDE;
    void Log(LogLevel level, const char * file, int32_t line, const wchar_t * messageWide) XR_OVERRIDE;
};

// --------------------------------------------------------------------------------------  FUNCTION
/// Optional Shutdown call. If you don't call this then handles may be
/// leaked. Leaking memory to shutdown is really not a bad thing, but for
/// testing sake this function is useful.
// --------------------------------------------------------------------------------------  FUNCTION
void LogSystemShutdown();

// ***************************************************************************************** - TYPE
/// Some LogHandles are allocated and intended to remain until the system
/// shuts down, this is useful mainly for a higher level system to
/// configure streams without worrying about them disappearing.
// ***************************************************************************************** - TYPE
enum DeletionPolicy
{
    kNaturalLifetime,           ///< The item will be deleted external to the logging system
    kFreeOnLogSystemShutdown    ///< The item will be deleted when the logging system shuts down.
                                ///<   This option is provided for the convenience of a higher
                                ///<   level initialization system. It is not intended for normal use.
};

// --------------------------------------------------------------------------------------  FUNCTION
/// Allow a few log consumers per handle. The max is defined by
/// kLogHandleMaxConsumers constant. Exceeding this constant will
/// not crash, but additional consumers will be ignored.
/// This limitation can be overcome at a higher level by creating
/// composite log consumers.
// --------------------------------------------------------------------------------------  FUNCTION
bool     AddLogConsumer(const char * name, ILogConsumer * consumer, LogLevel level);
// --------------------------------------------------------------------------------------  FUNCTION
/// Remove the specified log \a consumer from the specified stream \a name.
// --------------------------------------------------------------------------------------  FUNCTION
bool     RemoveLogConsumer(const char * name, ILogConsumer * consumer);
// --------------------------------------------------------------------------------------  FUNCTION
/// Change the Log level of this handle (and all identical and
/// downstream handles.
// --------------------------------------------------------------------------------------  FUNCTION
LogLevel SetLogConsumerLevel(const char * name, ILogConsumer * consumer, LogLevel level);
// --------------------------------------------------------------------------------------  FUNCTION
/// return the LogLevel of for the \a consumer from the specified stream \a name.
// --------------------------------------------------------------------------------------  FUNCTION
LogLevel GetLogConsumerLevel(const char * name, ILogConsumer * consumer);
// --------------------------------------------------------------------------------------  FUNCTION
/// This function is for convenience in periodic code. It is too slow
/// to be called regularly
// --------------------------------------------------------------------------------------  FUNCTION
bool LogSystemHasHandleForName(const char * name);
// ***************************************************************************************** - TYPE
/// Does a new Handle search to see if it already exists?
/// Such a search would be linear.
// ***************************************************************************************** - TYPE
class LogHandle
{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /// This is the maximum number of consumers any named stream can
    /// have
    // ------------------------------------------------------------------------------------  MEMBER
    static const size_t kLogHandleMaxConsumers = 4;
    // ------------------------------------------------------------------------------------  MEMBER
    /// This is the MAX size of formatted string that can be logged in
    /// a single call. Note that this only applies to LogFormatted calls.
    /// The Logged string will truncated to this size.
    // ------------------------------------------------------------------------------------  MEMBER
    static const size_t kLogHandleMaxFormattedStringSize = 4096;

    // ------------------------------------------------------------------------------------  MEMBER
    /// Create a log handle and bind it to the stream with \a name
    // ------------------------------------------------------------------------------------  MEMBER
    LogHandle(const char * name);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Create a log handle and bind it to the stream with \a name,
    /// also specify a DeletionPolicy with \a policy
    // ------------------------------------------------------------------------------------  MEMBER
    LogHandle(const char * name, DeletionPolicy policy);
    // ------------------------------------------------------------------------------------  MEMBER
    // ------------------------------------------------------------------------------------  MEMBER
    ~LogHandle();
    // ------------------------------------------------------------------------------------  MEMBER
    /// This is what all of the log defines use.
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_SIDE_EFFECTS_PREFIX
    inline bool ShouldLog(LogLevel messageLevel) const XR_NO_SIDE_EFFECTS_POSTFIX;

    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief Print a string message (if you can assemble the required arguments, probably best to use the provided macros!)*/
    // ------------------------------------------------------------------------------------  MEMBER
    void Log(LogLevel level, const char * file, int32_t line, const char * message) const;
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief Print a string message (if you can assemble the required arguments, probably best to use the provided macros!)*/
    // ------------------------------------------------------------------------------------  MEMBER
    void LogFormatted(
        LogLevel level,
        const char * file,
        int32_t line,
#if defined(XR_COMPILER_MICROSOFT)
        _Printf_format_string_
#endif
        const char * format, ...) const
#if defined(XR_COMPILER_GCC)
    __attribute__ ((format (printf, 5, 6)))
#endif
    ;
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief Print a string message (if you can assemble the required arguments, probably best to use the provided macros!)*/
    // ------------------------------------------------------------------------------------  MEMBER
    void Log(LogLevel level, const char * file, int32_t line, const wchar_t * message) const;
    // ------------------------------------------------------------------------------------  MEMBER
    /*! \brief Print a string message (if you can assemble the required arguments, probably best to use the provided macros!)*/
    // ------------------------------------------------------------------------------------  MEMBER
    void LogFormatted(
        LogLevel level,
        const char * file,
        int32_t line,
#if defined(XR_COMPILER_MICROSOFT)
        _Printf_format_string_
#endif
        const wchar_t * format,
        ...) const
#if defined(XR_COMPILER_GCC)
        // Attribute does not work with wchar'd format string.
        //__attribute__ ((format (printf, 5, 6)))
#endif
    ;

    // ------------------------------------------------------------------------------------  MEMBER
    /*! Allow a log consumers to the stream bound to this handle. The
        max is defined by kLogHandleMaxConsumers constant. Exceeding
        this constant will not crash, but additional consumers will be
        ignored. This limitation can be overcome at a higher level by
        creating composite log consumers, although the limit is not
        expected to be reached in normal use.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    bool            AddLogConsumer(ILogConsumer * consumer, LogLevel level);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Remove the passed log consumer from the stream bound to this
    /// handle
    // ------------------------------------------------------------------------------------  MEMBER
    bool            RemoveLogConsumer(ILogConsumer * consumer);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Change the Log level of this handle (and all identical and
    /// downstream handles.
    // ------------------------------------------------------------------------------------  MEMBER
    LogLevel        SetLogConsumerLevel(ILogConsumer * consumer, LogLevel level);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Return the LogLevel for the passed \a consumer as set to the
    /// named stream bound to the handle.
    // ------------------------------------------------------------------------------------  MEMBER
    LogLevel        GetLogConsumerLevel(ILogConsumer * consumer) const;
    // ------------------------------------------------------------------------------------  MEMBER
    /*! Return the consumer array for the named stream bound to the
        handle. \a outConsumers and \a outLevels should arrays of at
        least kLogHandleMaxConsumers size.
    */
    // ------------------------------------------------------------------------------------  MEMBER
    size_t          GetLogConsumers(ILogConsumer * outConsumers[kLogHandleMaxConsumers], LogLevel outLevels[kLogHandleMaxConsumers]) const;

    // ------------------------------------------------------------------------------------  MEMBER
    /// Traversal of Handle list. Note that the List should be locked
    /// before you call these.
    // ------------------------------------------------------------------------------------  MEMBER
    LogHandle  * GetNext();
    // ------------------------------------------------------------------------------------  MEMBER
    /// Traversal of Handle list. Note that the List should be locked
    /// before you call these.
    // ------------------------------------------------------------------------------------  MEMBER
    LogHandle  * GetPrev();
    // ------------------------------------------------------------------------------------  MEMBER
    /// return the name of the stream this handle is bound to
    /// (which is the name that was passed into the LogHandle
    /// constructor).
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_SIDE_EFFECTS_PREFIX
    const char * GetName() XR_NO_SIDE_EFFECTS_POSTFIX;
    // ------------------------------------------------------------------------------------  MEMBER
    /// Return the deletion policy of the handle (which can be set via
    /// the LogHandle constructor.
    // ------------------------------------------------------------------------------------  MEMBER
    XR_NO_SIDE_EFFECTS_PREFIX
    DeletionPolicy GetDeletionPolicy() XR_NO_SIDE_EFFECTS_POSTFIX;
    // ------------------------------------------------------------------------------------  MEMBER
    /// handle list mutex. This will not prevent print statements from
    /// happening, it only prevents the handle list from being modified.
    // ------------------------------------------------------------------------------------  MEMBER
    static Core::RecursiveMutex* GetListMutex();
    // ------------------------------------------------------------------------------------  MEMBER
    /// returns the first handle.
    // ------------------------------------------------------------------------------------  MEMBER
    static LogHandle * GetFirstHandle(){ return sFirstHandle; }
protected:
    // ------------------------------------------------------------------------------------  MEMBER
    /// Internally called by all constructors.
    // ------------------------------------------------------------------------------------  MEMBER
    void Setup(const char * name, DeletionPolicy lifetime);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Internal function. Caller must have already locked the mutex
    /// and validated inputs.
    // ------------------------------------------------------------------------------------  MEMBER
    static void FindByNameSafe(const char * name, LogHandle ** outClosestHandle, bool * outFoundExrct, LogHandle ** outPreviousHandle);

    // ------------------------------------------------------------------------------------  MEMBER
    /// Internal function. Caller must have already locked the mutex
    /// and validated inputs.
    // ------------------------------------------------------------------------------------  MEMBER
    static void LinkSafe(LogHandle *handle, LogHandle * afterThis);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Internal function. Caller must have already locked the mutex
    /// and validated inputs.
    // ------------------------------------------------------------------------------------  MEMBER
    static void UnLinkSafe(LogHandle *handle);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Internal function. Caller must have already locked the mutex
    /// and validated inputs.
    // ------------------------------------------------------------------------------------  MEMBER
    void FixupMinLevelSafe();
    // ------------------------------------------------------------------------------------  MEMBER
    /// Internal function. Caller must have already locked the mutex
    /// and validated inputs.
    // ------------------------------------------------------------------------------------  MEMBER
    void CopyConsumersFrom(const LogHandle *handle);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Internal function. Caller must have already locked the mutex
    /// and validated inputs.
    // ------------------------------------------------------------------------------------  MEMBER
    void CollectOrphans();
    // ------------------------------------------------------------------------------------  MEMBER
    /// Internal function. Caller must have already locked the mutex
    /// and validated inputs.
    // ------------------------------------------------------------------------------------  MEMBER
    void    PropConsumerList();


    // ------------------------------------------------------------------------------------  MEMBER
    /// This function will cause duplicates in the list if the name
    /// already exists. Although this may waste some space, it may be
    /// more efficient if an allocation is avoided.
    // ------------------------------------------------------------------------------------  MEMBER
    static void InsertHandleSafe(LogHandle * handle, const char * name);
    // ------------------------------------------------------------------------------------  MEMBER
    /// Remove handle form the list.
    // ------------------------------------------------------------------------------------  MEMBER
    static void RemoveHandleSafe(LogHandle * handle);

private:
    LogLevel       mMinLevel;   //!< Minimum level
    size_t         mLogConsumerCount;
    LogLevel       mLogConsumerLevels[kLogHandleMaxConsumers]; //!< Array of level per consumer
    ILogConsumer * mLogConsumers[kLogHandleMaxConsumers];      //!< Array of actual consumers
    const char   * mName;       //!< Name of handle
    LogHandle    * mNext;       //!< List entry.
    LogHandle    * mPrev;       //!< List entry.
    LogHandle    * mParent;     //!< If values are inherited, then this is non-nullptr.
    DeletionPolicy mLifetime;
    static LogHandle * sFirstHandle;


    friend void LogSystemShutdown();
}; // end of LogHandle Class


// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
bool LogHandle::ShouldLog(LogLevel messageLevel) const
{
    return (messageLevel >= (LogLevel)mMinLevel);
}

}}

#endif //#ifndef XR_CORE_LOG_H
