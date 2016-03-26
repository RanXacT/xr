
#------------------------------------------------------------------------------
# Don't bother maintaining a list, just compile them all.
#------------------------------------------------------------------------------
FILE(GLOB_RECURSE xr_services_test_SOURCES *.cpp)
INCLUDE_DIRECTORIES (${XR_SOURCE_DIR}/include) 

#------------------------------------------------------------------------------
ADD_EXECUTABLE (xr_services_test ${xr_services_test_SOURCES} ../main.cpp )

#------------------------------------------------------------------------------
# General Includes (internal includes)
#------------------------------------------------------------------------------
# IMPORTANT: include services BEFORE core if you plan to build with mingw
# or you will get painfully misterious undefined references
TARGET_LINK_LIBRARIES(xr_services_test xr_services xr_core)

#------------------------------------------------------------------------------
# Platform specific includes
#------------------------------------------------------------------------------
IF(WIN32)
# wsock32.lib ws2_32.lib
# TARGET_LINK_LIBRARIES(xr_services_test Add_Libs_Here)
ENDIF(WIN32)
IF(APPLE)
# crypto ssl ldap z
# TARGET_LINK_LIBRARIES(xr_services_test Add_Libs_Here)
ENDIF(APPLE)
IF(UNIX)
TARGET_LINK_LIBRARIES(xr_services_test pthread)
ENDIF(UNIX)

#------------------------------------------------------------------------------
# External library includes
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Add a test for this executable.
#------------------------------------------------------------------------------

ADD_TEST(
	NAME xr_services_test 
	WORKING_DIRECTORY ${XR_TEST_ROOT} 
	COMMAND $<TARGET_FILE:xr_services_test>)

# Need to setup working directory for windows debugging
if(MSVC_IDE)
if(NOT EXISTS ${XR_BINARY_DIR}/source/test/services/xr_services_test.vcxproj.user)
FILE(COPY ${XR_SOURCE_DIR}/setup/template.vcxproj.user DESTINATION ${XR_BINARY_DIR}/source/test/services )
FILE(RENAME ${XR_BINARY_DIR}/source/test/services/template.vcxproj.user ${XR_BINARY_DIR}/source/test/services/xr_services_test.vcxproj.user)
endif(NOT EXISTS ${XR_BINARY_DIR}/source/test/services/xr_services_test.vcxproj.user)
endif(MSVC_IDE)
