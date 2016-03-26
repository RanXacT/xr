// ######################################################################################### - FILE
/*!

Each unique handle is entered into a list (so there are duplicates). Hierarchy
is maintained when items are added and removed. A item first finds it's parents
(which are always before it's position in the list) and then looks for any
potential children (currently orphans).

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_LOG_H
#include "xr/core/log.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_STRING_UTILS_H
#include "xr/core/string_utils.h"
#endif
#ifndef XR_CORE_MEM_UTILS_H
#include "xr/core/mem_utils.h"
#endif
#ifndef XR_CORE_THREADING_MUTEX_H
#include "xr/core/threading/mutex.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_CONSOLE_H
#include "xr/core/console.h"
#endif

// ######################################################################################### - FILE
/* Private Macros */
// ######################################################################################### - FILE


// ######################################################################################### - FILE
/* Implementation */
// ######################################################################################### - FILE
namespace xr { namespace Core {

/// \internal
/// Need to make sure this mutex is initialized quite early to prevent
/// init order issues.
#if defined(XR_COMPILER_MICROSOFT)
__pragma(warning(push))
__pragma(warning(disable: 4075)) //  initializers put in unrecognized initialization area
#pragma init_seg( ".CRT$XCB" )
static Core::RecursiveMutex gLogMutex;
__pragma(warning(pop))
#elif defined(XR_COMPILER_GCC)
static Core::RecursiveMutex __attribute__((init_priority (101))) gLogMutex;
#else
#error "This needs to initialize first on most systems"
static Core::RecursiveMutex gLogMutex;
#endif

// ######################################################################################### - FILE
// Global Members (instance members)
// ######################################################################################### - FILE

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogSystemShutdown()
{
    Core::AutoProtectScope<RecursiveMutex> aps (LogHandle::GetListMutex());
    LogHandle * h = LogHandle::GetFirstHandle();

    while( h != nullptr)
    {
        LogHandle * next = h->GetNext();
        if(h->mLifetime == kFreeOnLogSystemShutdown)
        {
            // The delete should unlink the entry.
            XR_DELETE(h);
        }

        h = next;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool     AddLogConsumer(const char * name, ILogConsumer * consumer, LogLevel level)
{
    LogHandle::GetListMutex()->Lock();

    LogHandle * h = LogHandle::GetFirstHandle();

    while(h != nullptr)
    {
        if(StringCompare(name, h->GetName()) == 0 && h->GetDeletionPolicy() == kFreeOnLogSystemShutdown)
        {
            break;
        }

        h = h->GetNext();
    }

    if(h == nullptr)
    {
        // There is not a resident entry. Create one. (don't rely on any
        // natural entries you may find in the list, they could disappear
        // at any time.
        h = XR_NEW("Core::Log") LogHandle(name, kFreeOnLogSystemShutdown);
    }

    return h->AddLogConsumer(consumer, level);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool     RemoveLogConsumer(const char * name, ILogConsumer * consumer)
{
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);

    LogHandle * h = LogHandle::GetFirstHandle();

    while(h != nullptr)
    {
        if(StringCompare(name, h->GetName()) == 0)
        {
            break;
        }

        h = h->GetNext();
    }

    if(h == nullptr)
    {
        return false;
    }

    return h->RemoveLogConsumer(consumer);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
LogLevel SetLogConsumerLevel(const char * name, ILogConsumer * consumer, LogLevel level)
{
    LogHandle lh(name);
    return lh.SetLogConsumerLevel(consumer, level);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
LogLevel GetLogConsumerLevel(const char * name, ILogConsumer * consumer)
{
    LogHandle lh(name);
    return lh.GetLogConsumerLevel(consumer);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool LogSystemHasHandleForName(const char * name)
{
    Core::AutoProtectScope<RecursiveMutex> aps (LogHandle::GetListMutex());

    bool bFound = false;

    LogHandle * temp = LogHandle::GetFirstHandle();

    while(temp != nullptr)
    {
        if(StringCompare(name, temp->GetName()) == 0)
        {
            bFound = true;
            break;
        }
        temp = temp->GetNext();
    }

    return bFound;
}
// ######################################################################################### - FILE
// LogHandle Members (instance members)
// ######################################################################################### - FILE
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::Setup(const char * name, DeletionPolicy lifetime)
{
    for(size_t i = 0; i < kLogHandleMaxConsumers; i++)
    {
        mLogConsumers[i] = 0;
        mLogConsumerLevels[i] = kLogLevelMax;
    }
    mLogConsumerCount = 0;
    mName    = name;
    mNext    = nullptr;
    mPrev    = nullptr;
    mMinLevel= kLogLevelMax;
    mParent  = nullptr;
    mLifetime = lifetime;
    InsertHandleSafe(this, name);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
LogHandle::LogHandle(const char * name)
{
    Setup(name, kNaturalLifetime);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
LogHandle::LogHandle(const char * name, DeletionPolicy policy)
{
    Setup(name, policy);
}
LogHandle::~LogHandle()
{
    RemoveHandleSafe(this);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::Log(LogLevel level, const char * file, int32_t line, const char * message) const
{
    const LogHandle * temp = this;

    // this mutex is a safety measure, and is probably worthwhile given the
    // trade off and the fact that logging is about to happen.
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);

    while(temp != nullptr)
    {
        for(size_t i = 0; i < temp->mLogConsumerCount; i++)
        {
            if(temp->mLogConsumers[i] != nullptr && level >= temp->mLogConsumerLevels[i])
            {
                temp->mLogConsumers[i]->Log(level, file,line,message);
            }
        }

        temp = temp->mParent;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::LogFormatted(
    LogLevel level,
    const char * file,
    int32_t line,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const char * format, ...) const
{
    // Only format the string once.
    char buffer[kLogHandleMaxFormattedStringSize];

    va_list ap;
    va_start(ap,format);
    ::xr::Core::VStringPrintf(buffer, kLogHandleMaxFormattedStringSize, format, ap);
    va_end(ap);

    // Log it.
    Log(level, file,line,buffer);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::Log(LogLevel level, const char * file, int32_t line, const wchar_t * message) const
{
    const LogHandle * temp = this;

    // this mutex is a safety measure, and is probably worthwhile given the
    // trade off and the fact that logging is about to happen.
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);

    while(temp != nullptr)
    {
        for(size_t i = 0; i < temp->mLogConsumerCount; i++)
        {
            if(temp->mLogConsumers[i] != nullptr && level >= temp->mLogConsumerLevels[i])
            {
                temp->mLogConsumers[i]->Log(level, file,line,message);
            }
        }

        temp = temp->mParent;
    }

}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::LogFormatted(
    LogLevel level,
    const char * file,
    int32_t line,
#if defined(XR_COMPILER_MICROSOFT)
    _Printf_format_string_
#endif
    const wchar_t * format, ...) const
{
    // Only format the string once.
    wchar_t buffer[kLogHandleMaxFormattedStringSize];

    va_list ap;
    va_start(ap,format);
    ::xr::Core::VStringPrintf(buffer, kLogHandleMaxFormattedStringSize, format, ap);
    va_end(ap);

    // Log it.
    Log(level, file,line,buffer);
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
LogHandle  * LogHandle::GetNext()
{
    return mNext;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
LogHandle  * LogHandle::GetPrev()
{
    return mPrev;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
const char * LogHandle::GetName()
{
    return mName;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
XR_NO_SIDE_EFFECTS_PREFIX
DeletionPolicy LogHandle::GetDeletionPolicy()
{
    return mLifetime;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
Core::RecursiveMutex* LogHandle::GetListMutex()
{
    return &gLogMutex;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::InsertHandleSafe(LogHandle * handle, const char * name)
{
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);

    LogHandle * prevHandle = nullptr;
    LogHandle * closestRelative = nullptr;
    bool exrctMatch = false;

    FindByNameSafe(name, &closestRelative, &exrctMatch, &prevHandle);

    LinkSafe(handle, prevHandle);

    // If this is inheriting values, let us know later.
    if(!exrctMatch)
    {
        handle->mParent = closestRelative;

        // This is a new entry into the list (not a duplicate)
        // so there may be some children looking for a parent
        handle->CollectOrphans();
    }
    else
    {
        // If this is the same item, copy it's parent over
        handle->mParent = closestRelative->mParent;
        handle->CopyConsumersFrom(closestRelative);
    }

    if(handle->mParent != nullptr)
    {
        handle->FixupMinLevelSafe();
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::RemoveHandleSafe(LogHandle * handle)
{
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);
    UnLinkSafe(handle);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
bool LogHandle::AddLogConsumer(ILogConsumer * consumer, LogLevel level)
{
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);

    // Validate.
    if(mLogConsumerCount >= kLogHandleMaxConsumers)
    {
        aps.Unlock();
        // Error, just return nullptr
        XR_DEBUG_UNEXPECTED_FM("Too many Log consumers passed to entry:%s. Max %" XR_SIZE_PRINT "", mName, kLogHandleMaxConsumers);
        return false;
    }

    // Put in place at the end.
    mLogConsumers[mLogConsumerCount]      = consumer;
    mLogConsumerLevels[mLogConsumerCount] = level;

    // Walk chain and fix up the min level
    FixupMinLevelSafe();

    // Inc Counter.
    mLogConsumerCount++;

    // Propagate settings
    PropConsumerList();

    return true;
}
/*```````````````````````````````````````````````````````````````*/
/*```````````````````````````````````````````````````````````````*/
bool LogHandle::RemoveLogConsumer(ILogConsumer * consumer)
{
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);

    bool bFound = false;
    size_t i = 0;

    // Search for it.
    for(; i < mLogConsumerCount; i++)
    {
        if(mLogConsumers[i] == consumer)
        {
            bFound = true;
            break;
        }
    }

    // shift remaining up one
    for(; i < (mLogConsumerCount-1); i++)
    {
        mLogConsumers[i] = mLogConsumers[i+1];
        mLogConsumerLevels[i] = mLogConsumerLevels[i+1];
    }

    // Clear out the last one
    if(bFound)
    {
        mLogConsumers[mLogConsumerCount-1] = nullptr;
        mLogConsumerLevels[mLogConsumerCount-1] = kLogLevelInvalid;
    }

    PropConsumerList();

    return bFound;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
size_t LogHandle::GetLogConsumers(ILogConsumer * outConsumers[kLogHandleMaxConsumers], LogLevel outLevels[kLogHandleMaxConsumers]) const
{
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);

    // Fill them in.
    for(size_t i = 0; i < mLogConsumerCount; i++)
    {
        outConsumers[i] = mLogConsumers[i];
        outLevels[i] = mLogConsumerLevels[i];
    }
    return mLogConsumerCount;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
xr::Core::LogLevel LogHandle::SetLogConsumerLevel( ILogConsumer *consumer, LogLevel level)
{
    Core::AutoProtectScope<RecursiveMutex> aps (&gLogMutex);

    // Find it.
    size_t index = mLogConsumerCount + 1;
    for(size_t i = 0; i < mLogConsumerCount; i++)
    {
        if(mLogConsumers[i] == consumer)
        {
            index = i;
            break;
        }
    }
    if(index >= mLogConsumerCount + 1)
    {
        // Not found.
        return kLogLevelInvalid;
    }

    // Place it.
    LogLevel retValue = mLogConsumerLevels[index];
    mLogConsumerLevels[index] = level;

    // Walk chain and fix up the min level
    FixupMinLevelSafe();

    PropConsumerList();
    return retValue;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::PropConsumerList( )
{
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    // The consumer list on this item changed. Find duplicate items and
    // update their lists.
    //  1. First look for identical items backwards.
    //  2. First look for identical items forwards.
    //  3. First look for child items forwards.
    // ```````````````````````````````````````````````````````````````````````````  SECTION

    // ```````````````````````````````````````````````````````````````````````````  SECTION
    // 1. Walk the List backwards. All equivalent handles will be right next to each other.
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    LogHandle * temp = this->mPrev;
    const char * name = mName;
    while(temp != nullptr)
    {
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        // handle must exrctly match when in order to apply.
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        int result = StringCompare(name, temp->mName);
        if( result == 0)
        {
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            // Copy the state to duplicate handle.
            temp->CopyConsumersFrom(this);
        }
        else
        {
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            // This is a sorted list, and we've passed where the item would be located
            break;
        }

        temp = temp->mPrev;
    }
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    // 2. Now look forwards for exrct matches.
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    temp = this->mNext;
    while(temp != nullptr)
    {
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        // handle must exrctly match when in order to apply.
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        int result = StringCompare(name, temp->mName);
        if( result == 0)
        {
            temp->CopyConsumersFrom(this);
        }
        else
        {
            // This is a sorted list, and we've passed where the item would be located
            break;
        }

        temp = temp->mNext;
    }
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    // 3. Now look for items that would derive from this item.
    //    And get them to update their cached min level
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    size_t lookupChars = StringLengthWithNull(name) - 1;
    while(temp != nullptr)
    {
        size_t handleChars = StringLengthWithNull(temp->mName) - 1;

        if(handleChars <= lookupChars)
        {
            // cannot possibly derive from.
            temp = temp->mNext;
            continue;
        }
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        // handle should be a superset to the lookup name
        // in order to apply.
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        int result = MemCompare8(name, temp->mName, lookupChars);
        if( result == 0)
        {
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            // 2 options for this path:
            //      1: They are related, in this case there will be '.' here in the handle name
            //      2: They are unrelated (although start with the same letters)
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            if( lookupChars == 0 || temp->mName[lookupChars] == '.')
            {
                // ```````````````````````````````````````````````````````````````````````````  SECTION
                // 1: The handle is a superset of the lookup with a '.' in the right location
                // we could be it's parent. Let's check. Note that ptr check would not be correct
                // since multiple handles exist for the same name.
                // ```````````````````````````````````````````````````````````````````````````  SECTION
                XR_ASSERT_ALWAYS_NE(temp->mParent, nullptr);
                if(lookupChars == 0 || StringCompare(mName, temp->mParent->mName) == 0)
                {
                    temp->FixupMinLevelSafe();
                }
            }
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            // 2: They are not actually related.
            // ```````````````````````````````````````````````````````````````````````````  SECTION
        }
        else if(result > 0)
        {
            // This is a sorted list, and we've passed where the item would be located
            break;
        }

        temp = temp->mNext;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
xr::Core::LogLevel LogHandle::GetLogConsumerLevel( ILogConsumer * consumer ) const
{
    // Find it.
    size_t index = mLogConsumerCount + 1;
    for(size_t i = 0; i < mLogConsumerCount; i++)
    {
        if(mLogConsumers[i] == consumer)
        {
            index = i;
            break;
        }
    }
    if(index >= mLogConsumerCount + 1)
    {
        // Not found.
        return kLogLevelInvalid;
    }

    return mLogConsumerLevels[index];
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::CopyConsumersFrom( const LogHandle *from )
{
    // Copy appropriate values over.
    memcpy(this->mLogConsumers, from->mLogConsumers, sizeof(from->mLogConsumers)) ;
    memcpy(this->mLogConsumerLevels, from->mLogConsumerLevels, sizeof(from->mLogConsumerLevels)) ;
    this->mMinLevel = from->mMinLevel;
    this->mLogConsumerCount = from->mLogConsumerCount;
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::CollectOrphans(  )
{
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    // Look for any un-parented items that are derivative from us.
    //
    // Imagine creating
    //   "1.2"
    //   "1"   <- Is parent
    //
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    size_t lookupChars = StringLengthWithNull(mName) - 1;

    for(LogHandle * temp = mNext; temp != nullptr; temp = temp->mNext)
    {
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        // "" is parent to all
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        if(lookupChars == 0)
        {
            if(temp->mParent == nullptr)
            {
                temp->mParent = this;
            }
            continue;
        }

        size_t handleChars = StringLengthWithNull(temp->mName) - 1;

        // ```````````````````````````````````````````````````````````````````````````  SECTION
        // Not possible to match with fewer characters.
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        if(handleChars <= lookupChars)
        {
            continue;
        }

        int result = MemCompare8(mName, temp->mName, lookupChars);
        if( result == 0)
        {
            if( temp->mName[lookupChars] == '.')
            {
                if(temp->mParent != nullptr)
                {
                    // There is already a parent. Are we a closer relative?
                    if(lookupChars < (StringLengthWithNull(temp->mParent->mName)-1))
                    {
                        // Nope, we are the other one's parent
                        continue;
                    }

                    // If we are inserting ourself, the item's present parent should
                    // be our parent. (or the list hierarchy is messed up).
                    XR_ASSERT_DEBUG_EQ( StringCompare(temp->mParent->mName, mParent->mName), 0);
                }
                temp->mParent = this;
            }
        }
        else if(result > 0)
        {
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            // This is a sorted list, and we've passed where the item would be located
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            break;
        }

    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::FixupMinLevelSafe()
{
    LogLevel lowest = kLogLevelMax;

    LogHandle * temp = this;

    while(temp != nullptr)
    {
        for(size_t i = 0; i < kLogHandleMaxConsumers; i++)
        {
            lowest = lowest < temp->mLogConsumerLevels[i] ? lowest : temp->mLogConsumerLevels[i];
        }

        temp = temp->mParent;
    }

    mMinLevel = lowest;
}
// ######################################################################################### - FILE
// Static members (for list management)
// ######################################################################################### - FILE
LogHandle * LogHandle::sFirstHandle = nullptr;

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::FindByNameSafe( const char * name, LogHandle ** outClosestHandle, bool * outFoundExrct, LogHandle ** outPreviousHandle )
{
    *outFoundExrct = false;

    // ```````````````````````````````````````````````````````````````````````````  SECTION
    // Walk the List. find any relative closer than the global object.
    // If this is a performance issue, we could (greatly) improve the search algorithm.
    // ```````````````````````````````````````````````````````````````````````````  SECTION
    LogHandle * temp = sFirstHandle;
    *outPreviousHandle = nullptr;

    size_t lookupChars = StringLengthWithNull(name) - 1;

    while(temp != nullptr)
    {
        size_t handleChars = StringLengthWithNull(temp->mName) - 1;

        // ```````````````````````````````````````````````````````````````````````````  SECTION
        // deal with special case: "" is parent to all.
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        if(handleChars == 0 && lookupChars == 0)
        {
            *outFoundExrct = true;
            *outClosestHandle = temp;
            *outPreviousHandle = temp;
            break;
        }

        if(handleChars == 0)
        {
            *outClosestHandle = temp;
        }

        // ```````````````````````````````````````````````````````````````````````````  SECTION
        // handle should be a subset (or exrct match to) the lookup name
        // in order to apply.
        // ```````````````````````````````````````````````````````````````````````````  SECTION
        if(handleChars > lookupChars)
        {
            // cannot possibly be a subset.
            temp = temp->mNext;
            continue;
        }

        int result = MemCompare8(name, temp->mName, handleChars);
        if( result == 0)
        {
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            // 3 options for this path:
            //      1: They are identical. In this case lookupChars == handleChars
            //      2: They are related, in this case there will be '.' here in the lookup name
            //      3: They are unrelated (although start with the same letters)
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            if(lookupChars == handleChars)
            {
                // ```````````````````````````````````````````````````````````````````````````  SECTION
                // 1: exrct match found, no further searching required.
                // Just link us into the list.
                // Setup new link pointers first so that it never
                // ```````````````````````````````````````````````````````````````````````````  SECTION
                *outFoundExrct = true;
                *outClosestHandle = temp;
                *outPreviousHandle = temp;
                break;
            }
            else if( name[handleChars] == '.')
            {
                // ```````````````````````````````````````````````````````````````````````````  SECTION
                // 2: The full handle is a parent hierarchically to the lookup name
                // meaning this is the new closest relative so far!
                // If there is a closer relative we will encounter it later.
                // no need for more complicated search.
                // ```````````````````````````````````````````````````````````````````````````  SECTION
                *outClosestHandle = temp;
            }
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            // 3: They are not actually related.
            // ```````````````````````````````````````````````````````````````````````````  SECTION
        }
        else if(result > 0)
        {
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            // This is a sorted list, and we've passed where the item would be located
            // ```````````````````````````````````````````````````````````````````````````  SECTION
            break;
        }

        *outPreviousHandle = temp;

        temp = temp->mNext;
    }


}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::LinkSafe( LogHandle *handle, LogHandle * afterThis )
{
    // Trivial Insert if first link.
    if(afterThis == nullptr)
    {
        if(sFirstHandle != nullptr)
        {
            handle->mPrev = nullptr;
            handle->mNext = sFirstHandle;
            sFirstHandle->mPrev = handle;
        }
        sFirstHandle = handle;
        return;
    }

    // LinkSafe it in here.
    handle->mNext = afterThis->mNext;
    handle->mPrev = afterThis;
    // LinkSafe it in.
    afterThis->mNext   = handle;
    if(handle->mNext != nullptr)
    {
        handle->mNext->mPrev = handle;
    }
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogHandle::UnLinkSafe( LogHandle *handle )
{
    // Handle removing the first (and perhaps only) item.
    if(sFirstHandle == handle)
    {
        sFirstHandle = handle->mNext;
    }

    // Steps:
    // Look for a duplicate
    // Find dependent entries
    // If duplicate found: replace references with duplicate
    // If duplicate not found: replace references with our parent.

    LogHandle *replacementParent = handle->mParent;

    // Unlink something other than the first item.
    if(handle->mPrev != nullptr)
    {

        // Look backwards for identical handles
        int result = StringCompare(handle->mPrev->mName, handle->mName);
        if(result == 0)
        {
            replacementParent = handle->mPrev;
        }

        handle->mPrev->mNext = handle->mNext;
    }
    if(handle->mNext != nullptr)
    {
        // Look backwards for identical handles
        int result = StringCompare(handle->mNext->mName, handle->mName);
        if(result == 0)
        {
            replacementParent = handle->mNext;
        }

        handle->mNext->mPrev = handle->mPrev;
    }

    // Now we've found a replacement parent. Replace any children's references.
    // Since this is sorted we could technically early out.
    LogHandle * temp = handle->mNext;
    while(temp != nullptr)
    {
        if(temp->mParent == handle)
        {
            temp->mParent = replacementParent;
        }
        temp = temp->mNext;
    }
}

// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogConsumerConsole::Log( LogLevel level, const char * , int32_t , const char * message )
{
    // Default log does not spam the file / line
    xr::Core::ConsolePrintf(
        level < kLogLevelError ? kConsoleStdOut : kConsoleStdOut,
        "%s", message);
}
// --------------------------------------------------------------------------------------  FUNCTION
// --------------------------------------------------------------------------------------  FUNCTION
void LogConsumerConsole::Log( LogLevel level, const char * , int32_t , const wchar_t * messageWide )
{
    // Default log does not spam the file / line
    xr::Core::ConsolePrintf(
        level < kLogLevelError ? kConsoleStdOut : kConsoleStdOut,
        messageWide);
}

}}//namespace xr

// ######################################################################################### - FILE
/* Compile Time Asserts */
// ######################################################################################### - FILE
//static_assert(expr, "")
