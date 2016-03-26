#------------------------------------------------------------------------------
# Create a library called "xr_services" which includes the source file "*.cpp".
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${XR_SOURCE_DIR}/include) 

#------------------------------------------------------------------------------
# Add headers to the project. No need to add them explicitly, just grab the lot of them.
#------------------------------------------------------------------------------
FILE(GLOB_RECURSE XR_SERVICES_HDRS ${XR_SOURCE_DIR}/include/xr/SERVICES/*.h)
FILE(GLOB_RECURSE XR_SERVICES_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


add_library(xr_services STATIC ${XR_SERVICES_SRCS} ${XR_SERVICES_HDRS}) 
