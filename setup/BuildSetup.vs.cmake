# NOTE these "set" statements must have a "FORCE" token, or they will not work.
# Base setup for visual studio builds.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "unused" FORCE)

# /MD          <= Multi-threaded dll for run time library.
# /GF          <= Enable String Pooling
# /GS-         <= Disable buffer security checks
# /fp:fast     <= favor fast over precise fp
# /fp:except-  <= disable fp exceptions
# /Ox          <= Full optimization
# /Oi          <= Enable Intrinsic functions
# /Oy          <= Omit frame pointers
# /Ob2         <= automatic inlining enabled
# /GL          <= Link Time code generation
set(CMAKE_CXX_FLAGS_RELEASE " /MD /GF /Gs- /fp:except- /Ox /Oi /Ob2 /Oy /D NDEBUG /GL " CACHE STRING "unused" FORCE)
set(CMAKE_C_FLAGS_RELEASE   " /MD /GF /Gs- /fp:except- /Ox /Oi /Ob2 /Oy /D NDEBUG /GL " CACHE STRING "unused" FORCE)

# /MDd         <= Multi-threaded debug dll for run time library.
# /RTC1        <= Run time checks for unused variables
# /Ob0         <= disable inlining.
set(CMAKE_CXX_FLAGS_DEBUG " /D_DEBUG /DDEBUG /MDd /Ob0 /Od /RTC1 " CACHE STRING "unused" FORCE)
set(CMAKE_C_FLAGS_DEBUG   " /D_DEBUG /DDEBUG /MDd /Ob0 /Od /RTC1 " CACHE STRING "unused" FORCE)

# /W4          <= Enable pedantic warnings
# /Gm          <= Enable minimal rebuilds
# /Gy          <= Enable function level linking
# /GR-         <= Disable RTTI
# /analyze     <= Run static analysis (removed and put in product files, don't want it in coverage builds.)
# /wd6255      <= Disable (useless) warning about using alloca
set(CMAKE_CXX_FLAGS " /DWIN32 /D_WINDOWS /EHsc /W4 /GR- /Gy /Gm /fp:fast /wd6255 " CACHE STRING "unused" FORCE)
set(CMAKE_C_FLAGS   " /DWIN32 /D_WINDOWS /EHsc /W4 /GR- /Gy /Gm /fp:fast " CACHE STRING "unused" FORCE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG " CACHE STRING "unused" FORCE)
# /LTCG        <= Link time code generation (for release builds)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG " CACHE STRING "unused" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG" CACHE STRING "unused" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG" CACHE STRING "unused" FORCE)
 