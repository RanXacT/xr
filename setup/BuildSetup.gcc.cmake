# NOTE these "set" statements must have a "FORCE" token, or they will not work.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "unused" FORCE)

# -Wno-pragmas <= no warnings about pragma misuse (typically pragma region FORCE)
# -g           <= Enable debug symbols       
# -w           <= disable all warnings (for c files)
# -Wall        <= enable all standard warnings
# -fno-rtti    <= no RTTI
# -fno-exceptions   <= no exceptions
# -mno-fp-exceptions  <= no floating point exceptions
# -Wno-unknown-pragmas  <= Don't bother about pragma's gcc does not know about.
# -Wno-invalid-offsetof <= Allows offset_of for noon POD types.
set(CMAKE_C_FLAGS " -g -w -fstrict-aliasing " CACHE STRING "unused" FORCE)
set(CMAKE_CXX_FLAGS " -g -Wall -fstrict-aliasing -Wno-unknown-pragmas -Wno-invalid-offsetof -fno-rtti " CACHE STRING "unused" FORCE)

# Optimization settings actually set by downstream files.
set(CMAKE_C_FLAGS_RELEASE " -DNDEBUG " CACHE STRING "unused" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "unused" FORCE)

set(CMAKE_C_FLAGS_DEBUG " -DDEBUG -D_DEBUG " CACHE STRING "unused" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "unused" FORCE)
