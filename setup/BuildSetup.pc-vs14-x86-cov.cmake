# NOTE these "set" statements must have a "FORCE" token, or they will not work.
include("setup/BuildSetup.vs.cmake")

# /Zi          <= generate debug info
# /arch:SSE2   <= Generate SSE2 code (Implied with x64, also causes a warning)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /arch:SSE2 " CACHE STRING "unused" FORCE )
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi /arch:SSE2 " CACHE STRING "unused" FORCE )

# /ZI          <= generate debug info for Edit and continue
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI " CACHE STRING "unused" FORCE )
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /ZI " CACHE STRING "unused" FORCE )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /PROFILE /OPT:NOREF /OPT:NOICF /INCREMENTAL:NO " CACHE STRING "unused" FORCE )

