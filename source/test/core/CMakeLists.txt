
#------------------------------------------------------------------------------
# Don't bother maintaining a list, just compile them all.
#------------------------------------------------------------------------------
FILE(GLOB_RECURSE xr_core_test_SOURCES *.cpp)
INCLUDE_DIRECTORIES (${XR_SOURCE_DIR}/include) 


#------------------------------------------------------------------------------
# create the executable (or bundle for apple)
#------------------------------------------------------------------------------
ADD_EXECUTABLE (xr_core_test ${xr_core_test_SOURCES} ../main.cpp )

#------------------------------------------------------------------------------
# General Includes (internal includes)
#------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(xr_core_test xr_core)

# Platform specific includes
IF(WIN32)
# TARGET_LINK_LIBRARIES(xr_core_test Add_Libs_Here)
ENDIF(WIN32)
IF(APPLE)
# TARGET_LINK_LIBRARIES(xr_core_test Add_Libs_Here)
ENDIF(APPLE)
IF(UNIX)
TARGET_LINK_LIBRARIES(xr_core_test pthread)
ENDIF(UNIX)

#------------------------------------------------------------------------------
# Core does not depend on any libraries.
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Add a test for this executable.
#------------------------------------------------------------------------------
ADD_TEST(
	NAME xr_core_test 
	WORKING_DIRECTORY "${XR_TEST_ROOT}"
	COMMAND $<TARGET_FILE:xr_core_test>)

# Need to setup working directory for windows debugging
if(MSVC_IDE)
if(NOT EXISTS ${XR_BINARY_DIR}/source/test/core/xr_core_test.vcxproj.user)
FILE(COPY ${XR_SOURCE_DIR}/setup/template.vcxproj.user DESTINATION ${XR_BINARY_DIR}/source/test/core )
FILE(RENAME ${XR_BINARY_DIR}/source/test/core/template.vcxproj.user ${XR_BINARY_DIR}/source/test/core/xr_core_test.vcxproj.user)
endif(NOT EXISTS ${XR_BINARY_DIR}/source/test/core/xr_core_test.vcxproj.user)
endif(MSVC_IDE)
