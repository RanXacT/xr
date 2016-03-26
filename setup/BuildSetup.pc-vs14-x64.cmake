# NOTE these "set" statements must have a "FORCE" token, or they will not work.
# this is relative to the build directory (not the current directory)
include("setup/BuildSetup.vs.cmake")

# /Zi          <= generate debug info

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi " CACHE STRING "unused" FORCE )
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  /Zi " CACHE STRING "unused" FORCE )

# /Zi          <= generate debug info (edit and continue not supported on x64)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi " CACHE STRING "unused" FORCE )
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi " CACHE STRING "unused" FORCE )

# This defaults to on, and prevents edit and continue.
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO " CACHE STRING "unused" FORCE)
