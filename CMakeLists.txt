# The name of our project is "XR". CMakeLists files in this project can
# refer to the root source directory of the project as ${XR_SOURCE_DIR} and
# to the root binary directory of the project as ${XR_BINARY_DIR}.
cmake_minimum_required (VERSION 3.1)
# regeneration is highly annoying, disable it.
SET(CMAKE_SUPPRESS_REGENERATION TRUE)

PROJECT (XR)

SET(XR_PROJECT_ROOT ${XR_SOURCE_DIR})
SET(XR_TEST_ROOT ${XR_SOURCE_DIR}/test)

# Parse XR_BUILD_TARGET
# platform-compiler-architecture-extra (where extra arguments are optional)
#
# supported platforms are "pc", "osx", "ios", "linux"
STRING(REGEX REPLACE "^([^\\-]*).+$" "\\1" XR_BUILD_PLATFORM ${XR_BUILD_TARGET}) 
STRING(REGEX REPLACE "^[^\\-]*\\-([^\\-]*).+$" "\\1" XR_BUILD_COMPILER ${XR_BUILD_TARGET}) 
STRING(REGEX REPLACE "^[^\\-]*\\-[^\\-]*\\-([^\\-]*)\\-?.*$" "\\1" XR_BUILD_ARCHITECTURE ${XR_BUILD_TARGET}) 
STRING(REGEX REPLACE "^[^\\-]*\\-[^\\-]*\\-[^\\-]*\\-?(.*)$" "\\1" XR_BUILD_EXTRA ${XR_BUILD_TARGET}) 

MESSAGE(STATUS "Target       : ${XR_BUILD_TARGET}")
MESSAGE(STATUS "Platform     : ${XR_BUILD_PLATFORM}")
MESSAGE(STATUS "Compiler     : ${XR_BUILD_COMPILER}")
MESSAGE(STATUS "Architecture : ${XR_BUILD_ARCHITECTURE}")
MESSAGE(STATUS "Extra        : ${XR_BUILD_EXTRA}")

INCLUDE("setup/BuildSetup.${XR_BUILD_TARGET}.cmake")

enable_testing()

#==============================================================================
set_property(
	DIRECTORY
	PROPERTY COMPILE_DEFINITIONS  $<$<CONFIG:Debug>:XR_DEBUG_FEATURES_ENABLED=1>
	)

add_subdirectory (source)
