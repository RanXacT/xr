# NOTE these "set" statements must have a "FORCE" token, or they will not work.
INCLUDE("setup/BuildSetup.gcc.cmake")

# -m32         <= Enable 32 bit pointers.
# -mmmx        <= Enable mmmx.
# -msse        <= Enable sse.
# -msse2       <= Enable sse2.
# -std=c++14 <= Enable C++ standard 0x features.
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -mmmx -msse -msse2 -mcrc32 -m32 " CACHE STRING "unused" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -msse -msse2 -mcrc32 -m32 -std=c++14 " CACHE STRING "unused" FORCE)

# -O3          <= Aggressive optimization
set(CMAKE_C_FLAGS_RELEASE " ${CMAKE_C_FLAGS_RELEASE} -O3 " CACHE STRING "unused" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 " CACHE STRING "unused" FORCE)

# Nothing to add here.
#set(CMAKE_C_FLAGS_DEBUG " ${CMAKE_C_FLAGS_DEBUG} " CACHE STRING "unused" FORCE)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "unused" FORCE)
