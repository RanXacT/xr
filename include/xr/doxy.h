// ######################################################################################### - FILE
/*! \mainpage

XR provides a simple set of base libraries for application development. The
library is segmented into a core library, a set of services and an
application framework. The core level contains systems which are always
functional, although enable more complex functionality if extended.

\par Bad Diagram
This is a poor diagram, should create a dedicated diagram for it.
\dot
digraph ArchitectureOverview {
graph[rankdir=LR];

COM [shape=record]

subgraph cluster_0{
node [shape=record];
Platform [label="<Defines>System Abstraction Defines|<baseTypes>Base Types"];
label="xr::Platform";
}

subgraph cluster_1{
node [shape=record];
Core [label="Allocator|Assert|Console|<timeDate>Time / Date|Logging|Memory|<abs>Platform Abstraction Functions|<Test>Unit Test Framework|<comHelpers>COM Utilities|<hash>Hash Implementations|Random|Threading|IO"];
label="xr::Core";
}

subgraph cluster_Services {
node [shape=record];
Services [label="Messaging|Scheduling|Reflection|Timeline"];
label = "Services";
color=blue;
}

COM -> Platform
Core -> Platform
Core:comHelpers -> COM
Services -> Core
Services -> COM
}
\enddot

\par Getting Started:
\li \ref codingconventions

\par Core System Overview: Core systems encapsulate resources or system processes.
They are resident and require no setup to function. Setup may be beneficial or
allow more intelligent operation but it remains optional. Core systems are also
not normally instanced. There is only one unambiguous logging system or
allocation system, etc. This ensures clarity when using the systems. Core Service
names describe the resource / process they abstract.
\li \ref defines
\li \ref coreassert
\li \ref corelog
\li \ref corealloc
\li \ref corethread
\li \ref coreatomic
\li \ref coretime
\li \ref coretest
\li \ref com

\par Services Overviews: Services encapsulate high level processes. They are not
resident and require explicit setup and shutdown. The application code can use them
as singletons or pass instances around explicitly, internally there is no assumption
that they are singletons. Services are typically named for the action they abstract.
\li \ref scheduling

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// Don't cause issues with visual studio source parsing
#ifndef __INTELLISENSE__
#error "This file is not intended to be included"
#endif
