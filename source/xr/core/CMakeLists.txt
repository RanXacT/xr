#------------------------------------------------------------------------------
# Create a library called "xr_core" which includes the source file "*.cpp".
#------------------------------------------------------------------------------
include_directories (${XR_SOURCE_DIR}/include) 

#------------------------------------------------------------------------------
# Add headers to the project. No need to add them explicitly, just grab the lot of them.
#------------------------------------------------------------------------------
FILE(GLOB         XR_MAIN_HDRS ${XR_SOURCE_DIR}/include/xr/*.h)
FILE(GLOB_RECURSE XR_COM_HDRS  ${XR_SOURCE_DIR}/include/xr/com/*.h)
FILE(GLOB_RECURSE XR_CORE_HDRS ${XR_SOURCE_DIR}/include/xr/core/*.h)
FILE(GLOB_RECURSE XR_PLAT_HDRS ${XR_SOURCE_DIR}/include/xr/platform/*.h)
FILE(GLOB_RECURSE XR_CORE_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


add_library(xr_core STATIC ${XR_CORE_SRCS} ${XR_MAIN_HDRS} ${XR_COM_HDRS} ${XR_PLAT_HDRS} ${XR_CORE_HDRS}) 
