// ######################################################################################### - FILE
/*! \file

This encapsulates a runnable object for the purposes of threading and jobs

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_CORE_RUNNABLE_H
#define XR_CORE_RUNNABLE_H

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

// ######################################################################################### - FILE
/* Forward Declarations */
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Declarations */
// ######################################################################################### - FILE
namespace xr { namespace Core {
// ***************************************************************************************** - TYPE
/*! Generic Arguments Object. Can be type cast to other things.
    */
// ***************************************************************************************** - TYPE
XR_ALIGN_PREFIX( 16 )
    /// Instance arguments for job. Opaque to the scheduler.
struct Arguments{
    //@{
    /// Arguments are opaque to the scheduler.
    uintptr_t a0;
    uintptr_t a1;
    uintptr_t a2;
    uintptr_t a3;
    //@}

    /// Initializes values
    Arguments(uintptr_t a0_, uintptr_t a1_ = 0, uintptr_t a2_ = 0, uintptr_t a3_ = 0):
    a0(a0_),a1(a1_),a2(a2_),a3(a3_)
    {}
    /// Uninitialized values.
    Arguments()
    {}
}XR_ALIGN_POSTFIX( 16 );
// ***************************************************************************************** - TYPE
/*! Common runnable based used in numerous systems for callbacks etc, the
    common base allows for system interoperability (more specifically it
    allows the scheduler to interact with all of the other systems) but
    other possibilities exist.
    */
// ***************************************************************************************** - TYPE
typedef void (*Runnable)(const Arguments * pArguments);

// ***************************************************************************************** - TYPE
/*! Common completion interface makes async operations more consistent. */
// ***************************************************************************************** - TYPE
class XR_PURE_INTERFACE Completion
{
public:
    virtual ~Completion() {}

    // ------------------------------------------------------------------------------------  MEMBER
    /// returns true when the waited on operation is complete.
    // ------------------------------------------------------------------------------------  MEMBER
    virtual bool IsDone() const = 0;

    // ------------------------------------------------------------------------------------  MEMBER
    /// Blocks on the completion of the associated operation.
    // ------------------------------------------------------------------------------------  MEMBER
    virtual void WaitOn() const = 0;
};
}} // namespace
#endif //#ifndef XR_CORE_RUNNABLE_H
