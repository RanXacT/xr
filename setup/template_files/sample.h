// ######################################################################################### - FILE
/*! \file

\author Copyright 2011 by Daniel Craig, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
// Guard 
// ######################################################################################### - FILE
#ifndef XR_SAMPLE_H
#define XR_SAMPLE_H

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
/*!
    */
// ***************************************************************************************** - TYPE
class Sample{
public:
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    void Function1();
    // ------------------------------------------------------------------------------------  MEMBER
    /*!  */
    // ------------------------------------------------------------------------------------  MEMBER
    void Function2();
protected:
private:
};

}}

#endif // Include Guard
