# NOTE these "set" statements must have a "FORCE" token, or they will not work.
INCLUDE("setup/BuildSetup.gcc.cmake")

# -m64         <= Enable 64 bit pointers.
# -mmmx        <= Enable mmmx. (implied by m64)
# -msse        <= Enable sse.  (implied by m64)
# -msse2       <= Enable sse2. (implied by m64)
# -std=c++14   <= Enable C++ standard 0x features.

# --coverage   <= Enable test coverage. Note, now need a script to iterate over all generated .gcno files

set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -m64 -mcrc32 --coverage " CACHE STRING "unused" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -mcrc32 -std=c++14 --coverage " CACHE STRING "unused" FORCE)

# -O3          <= Aggressive optimization
set(CMAKE_C_FLAGS_RELEASE " ${CMAKE_C_FLAGS_RELEASE} -O3 " CACHE STRING "unused" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 " CACHE STRING "unused" FORCE)

# Nothing to add here.
#set(CMAKE_C_FLAGS_DEBUG " ${CMAKE_C_FLAGS_DEBUG} " CACHE STRING "unused" FORCE)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} " CACHE STRING "unused" FORCE)
