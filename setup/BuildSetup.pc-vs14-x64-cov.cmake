
# this is relative to the build directory (not the current directory)
include("setup/BuildSetup.vs.cmake")

# /Zi          <= generate debug info
# /arch:SSE2   <= Generate SSE2 code (Implied with x64, also causes a warning)
# /Gr          <= __fastcall by default

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /arch:SSE2 " CACHE STRING "unused" FORCE )
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  /Zi /arch:SSE2  " CACHE STRING "unused" FORCE )

# /Zi          <= generate debug info (edit and continue not supported on x64)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi " CACHE STRING "unused" FORCE )
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi " CACHE STRING "unused" FORCE )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /PROFILE /OPT:NOREF /OPT:NOICF /INCREMENTAL:NO " CACHE STRING "unused" FORCE )
