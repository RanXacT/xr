// ######################################################################################### - FILE
/*! \page codingconventions Coding Conventions

 \section naming Naming Conventions

\subsection varnaming Variable Names
Type is not signified in name, only scope.

Type | Rules
------------- | -------------
 MACRO_NAME            | All Caps
 TypeName              | Pascal Case: "Object", "Vector3"
 FunctionName          | Pascal Case, Verb+Noun: "SetX", "CreateInstance"
 localVariableName     | Camel Case: "x", "testSize", "row", "lastName"
 mMemberVariableName   | m + PascalCase: "mHeight"
 sStaticVariableName   | s + PascalCase: "sNumCreated"
 gGlobalVariableName   | g + PascalCase: "gDontDoThis"
 kConstantVariableName | k + PascalCase: "kPi", "kDefaultThreadPriority"
 

 \subsection funcnaming Function Names

Should generally be Verb + Noun
 - Functions should be affirmatively names
  - "HasFixedWidth()" and "GetEnabled()"
  - *not* "NoFixedWidth()" or "GetDisabled()"
 - Function names should be consistent and should mirror:
  - "Get"/"Set", "Create"/"Destroy", "Add"/"Subtract", "Send"/"Receive"
  - *not*: "Get"/"Write", "Create"/"Shutdown"

\subsection alignment Vertical alignment
Readability of the code is quite important, time and effort are traded at code writing time to 
save time in code reading. variables / members should be visually grouped and vertically 
tabulated where possible to ensure maximal readability. In extreme cases of long variable or 
type names one should not sacrifice readability for the sake of this rule. i.e. vertically align
where it improves readability and forgo where it decreases it.

\subsection ptrlocation * alignment
When vertically aligning variable declaration blocks, right align the "*" or "&". This ensures 
they are quickly visible and not lost in whitespace.

Good exrmple:
\code
xr::Core::Mutex m;
uint64_t        a;
time_t        * b;  // correct placement of *
MyTestEnum      c;
\endcode

Bad Exrmple:
\code
xr::Core::Mutex m;
uint64_t        a;
time_t*         b;  // bad placement of *
MyTestEnum      c;
\endcode

\subsection tabs Tabs vs Spaces
4 Spaces are used instead of tabs. Tabs are not used in (C/C++) source files. Tabs would be 
acceptable if people consistently differentiated between indentation (tabs) and vertical alignment 
(spaces) in code. Since that is rarely managed properly full usage of spaces provides consistent
vertical alignment. Since code is art, this ensures the art is viewed the same by all.

\subsection references References vs Pointers
Pointers are used to imply transfer of ownership, or potential nullptr. References are used
to imply non-nullptr and non transference of ownership. A returned pointer is assumed to be
needing to be deleted (except in the case of strings where expectations should be
documented). References are preferred over pointers in API's where possible and except as
mentioned in above guidelines.

\subsection argorder Argument Order
Arguments should be passed in the following order
 -# Identity arguments (handle, GUID, etc, etc)
 -# Read only arguments
 -# Read / Write arguments
 -# Write only arguments
 .
Default arguments are acceptable when they don't force a breakage of the above ordering. 
Alternate ordering is acceptable where it matches a standard function or API. As mentioned
in \ref references, passing a pointer is generally a hint that an argument is optional, 
deviation from this should be documented.

\subsection types Standard Types
Intrinsic types: c99 style types are used when possible
 - int32_t, uintptr_t,
 - char8_t, char16_t, char32_t

\subsection strings String Types
String types:
 internal strings should generally be of char8_t type, although other types are allowed where appropriate.
 path strings should be of path_char type. Path manipulation should use the core/path.h functionality.
 User Facing strings should never originate from code, and should always be of interface_char type.

\subsection includes \#include order
Required #include order
 -# xr/defines.h
 -# the associated header file to the cpp file
 -# other #includes


\subsection location Is this new code a module or a service?
Platform Module is a place for simple platform abstractions to improve the portability of
most code. These headers are included by EVERY source file and must be evaluated for length and
content in terms of compilation load.
 \li Are typedefs and #define's only covering what is intended to cover most code (but not all)
 \li Must not include non-trivial operating system headers
 \li Avoid standard library headers when possible
 \li Avoid wrappers around esoteric / very rarely used features. These are hard to port / maintain.
 Port higher level code instead, and discourage use of these features.

Core Modules provide consistent functionality across platforms for basic operations that all
programs should use.These headers are included by most source files and must be evaluated for length and
content in terms of compilation load. Core Modules:
 - Do not require setup, they work at any time.
 - Do not require configuration (although they may support configuration)
    - Some global configuration may exist but must be compile time.
 - Exist in the xr::Core Namespace. Sub-Namespaces are discouraged
 - Core modules often depend on macros for public functionality
 - Public headers attempt to minimize compile parsing time:
    - Avoid heavy use of compilation slowing primitives like templates, or meta programming.
    - Avoid including other headers where possible (use forward declarations where possible)
    - Avoid including operating system headers (Never include windows.h)
    - Include standard library headers as needed

Service Modules provide high level functionality. For example, where a core module may provide consistent
thread interaction, a service may provide a scheduling system. Services are all optional and are intended to
be runtime configured (not compile time configured). Services may as needed include operating system headers.
Interaction between services is discouraged except where such dependency is by definition. Service Modules:
 - Exist in there own xr::ServiceName namespace
 - Define a IManager in their namespace. This should have the following function: "static IManager * Initialize(struct InitializeOptions * options);"
   InitializeOptions struct is fully defined by the service. Services can optionally support nullptr
   as a parameter (and even specify a default parameter) but neither is required.
 - Should avoid using macros for public functionality
 - Should leverage interfaces for extensibility of functionality
 - Do not depend on each other unless necessary
 - Services work in direct response to public API calls. Services should generally not
   have any overhead that is not directly connected to a public API call.

Frameworks Modules provide self contained systems management. Where a service is directly and explicitly controlled 
by the application, a framework has a purpose and should be functional to perform a duty on it's own. Other than 
setup and configuration the application should only interact with the framework when something changes. Frameworks:
 - Exist in there own xr::FrameWorkName namespace
 - Define a IManager in their namespace. This should have the following function: "static IManager * Initialize(struct InitializeOptions * options);"
   InitializeOptions struct is fully defined by the Framework. Frameworks can optionally support nullptr
   as a parameter (and even specify a default parameter) but neither is required.
 - Should avoid using macros for public functionality
 - Should leverage interfaces for extensibility of functionality
 - Should generally maintain themselves

 \subsection markers File Section Markers
 hierarchy of comments is below:

\code
// ######################################################################################### - FILE
// File section separator
// ######################################################################################### - FILE

// ***************************************************************************************** - TYPE
// Type Separator
// ***************************************************************************************** - TYPE

// -----------------------------------------------------------------------------------------  MACRO
//  Macro Separator 
// -----------------------------------------------------------------------------------------  MACRO

// --------------------------------------------------------------------------------------  FUNCTION
//  Standalone Function Separator 
// --------------------------------------------------------------------------------------  FUNCTION

// ------------------------------------------------------------------------------------  MEMBER
// Function / Member Separator
// ------------------------------------------------------------------------------------  MEMBER

// ```````````````````````````````````````````````````````````````````````````  SECTION
// Function section separator (inside a function)
// ```````````````````````````````````````````````````````````````````````````  SECTION

\endcode
 */
// ######################################################################################### - FILE
// Don't cause issues with visual studio source parsing
#ifndef __INTELLISENSE__
#error "This file is not intended to be included"
#endif
