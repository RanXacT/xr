/*****************************************************************************/
/*! \file

\author Copyright 2011 by Daniel Craig, All Rights reserved.
*/
/*****************************************************************************/

/*****************************************************************************/
/* Guard                                                                     */
/*****************************************************************************/
#ifndef XR_CORE_STATIC_PROFILE_H
#define XR_CORE_STATIC_PROFILE_H

#if defined( _MSC_VER ) 
#pragma once
#endif
/*****************************************************************************/
/* Public Includes                                                           */
/*****************************************************************************/
#ifndef XR_DEFINES_H
#error "Must include xr/defines.h first!"
#endif
#ifndef XR_CORE_TIME_H
#include "xr/core/time.h"
#endif
/*****************************************************************************/
/* Public Macros                                                             */
/*****************************************************************************/
#if !XR_PROFILE_FEATURES_ENABLED
// Provide nullptr implementation of macros.
#define XR_STATIC_PROFILE_SCOPE( name ) 

#else

/// If profiling is on, profile this scope.
#define XR_STATIC_PROFILE_SCOPE( name ) static ::xr::Core::StaticProfiler XR_PREPROCESSOR_JOIN(StaticProfilerInst_,__LINE__) ( name); ::xr::Core::ScopeProfiler XR_PREPROCESSOR_JOIN(ScopeProfilerInst_,__LINE__) (XR_PREPROCESSOR_JOIN(StaticProfilerInst_,__LINE__))  

/*****************************************************************************/
/* Forward Declarations                                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Declarations                                                              */
/*****************************************************************************/
namespace xr { namespace Core {
    /*#######################################################################*/
    /*! 
     */
    /*#######################################################################*/
    struct StaticProfileMetric{
    public:
        xr::Core::TimeStamp clocksThisFrame;
        uint64_t  countThisFrame;
        xr::Core::TimeStamp clocksRunningAverage;
        double    countRunningAverage;
    };



    /*#######################################################################*/
    /*! 
     */
    /*#######################################################################*/
    class StaticProfiler{
    public:
        StaticProfiler(const char * name): mName(name) {}
        /*-------------------------------------------------------------------*/
        /*!  */
        /*-------------------------------------------------------------------*/
        void AddMetric(xr::Core::TimeStamp);

        /*-------------------------------------------------------------------*/
        /*!  */
        /*-------------------------------------------------------------------*/
        static void PrintAll( bool resetMetrics );

    protected:
    private:
        static const size_t     kFrameHistory = 64;

        const char            * mName;

        xr::Core::TimeStamp     mCyclesThisFrame;
        uint64_t                mCountThisFrame;

        xr::Core::TimeStamp     mCyclesHistorical[kFrameHistory];
        uint64_t                mCountHistorical[kFrameHistory];
    };

    /*#######################################################################*/
    /*! This provides an easy way to profile based on scope. Please use this via
        XR_STATIC_PROFILE_SCOPE( name ) which will #define away should profiling
        be disabled.
     */
    /*#######################################################################*/
    class ScopeProfiler{
    public:
        /*-------------------------------------------------------------------*/
        /*-------------------------------------------------------------------*/
        XR_FORCE_INLINE ScopeProfiler(StaticProfiler &prof) 
            : mProfiler(prof) 
        { 
            mStartTime = xr::Core::GetTimeStamp(); 
        }
        /*-------------------------------------------------------------------*/
        /*-------------------------------------------------------------------*/
        XR_FORCE_INLINE ~ScopeProfiler() 
        { 
            mProfiler.AddMetric(xr::Core::GetTimeStamp() - mStartTime);  
        }
        /*-------------------------------------------------------------------*/
        /*!  */
        /*-------------------------------------------------------------------*/
        
    protected:
    private:
        xr::Core::TimeStamp    mStartTime;
        class StaticProfiler & mProfiler;

        ScopeProfiler & operator=(ScopeProfiler);
    };

}}
#endif // XR_PROFILE_FEATURES_ENABLED
#endif // Include Guard
