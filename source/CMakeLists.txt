

#------------------------------------------------------------------------------
# Set Coverage defines if needed.
#------------------------------------------------------------------------------
IF("${XR_BUILD_EXTRA}" STREQUAL "cov")
MESSAGE(STATUS "Adding Coverage defines")
ADD_DEFINITIONS( -DXR_CODE_COVERAGE_BUILD=1 )
ENDIF("${XR_BUILD_EXTRA}" STREQUAL "cov")
# Set minimum windows version to Windows 10
#
# -D_WIN32_WINNT=0x0501       <= Minimum Windowx XP
# -DWINVER=0x0501             <= Minimum Windowx XP
# -DNTDDI_VERSION=0x0501      <= Minimum Windowx XP
# -DNTDDI_VERSION=0x05010000  <= Minimum Windowx XP
# -D__MSVCRT_VERSION__=0x0900 <= Minimum visual studio std lib, vs 9 (VS 2008) (used by MINGW)
#
#   Some reference:
#   
#   Minimum system required Value for NTDDI_VERSION
#   
#   Windows 10                      NTDDI_WIN10     (0x0A000000)
#   Windows 8.1                     NTDDI_WINBLUE   (0x06030000)
#   Windows 8                       NTDDI_WIN8      (0x06020000)
#   Windows 7                       NTDDI_WIN7      (0x06010000)
#   Windows Server 2008             NTDDI_WS08      (0x06000100)
#   Windows Vista with SP1          NTDDI_VISTASP1  (0x06000100)
#   Windows Vista                   NTDDI_VISTA     (0x06000000)
#   Windows Server 2003 with SP2    NTDDI_WS03SP2   (0x05020200)
#   Windows Server 2003 with SP1    NTDDI_WS03SP1   (0x05020100)
#   Windows Server 2003             NTDDI_WS03      (0x05020000)
#   Windows XP with SP3             NTDDI_WINXPSP3  (0x05010300)
#   Windows XP with SP2             NTDDI_WINXPSP2  (0x05010200)
#   Windows XP with SP1             NTDDI_WINXPSP1  (0x05010100)
#   Windows XP                      NTDDI_WINXP     (0x05010000)
#   Windows 2000 with SP4           NTDDI_WIN2KSP4  (0x05000400)
#   Windows 2000 with SP3           NTDDI_WIN2KSP3  (0x05000300)
#   Windows 2000 with SP2           NTDDI_WIN2KSP2  (0x05000200)
#   Windows 2000 with SP1           NTDDI_WIN2KSP1  (0x05000100)
#   Windows 2000                    NTDDI_WIN2K     (0x05000000)
#   
#   Minimum system required Minimum value for _WIN32_WINNT and WINVER
#   
#   Windows 7                                           _WIN32_WINNT_WIN7  (0x0601)
#   Windows Server 2008                                 _WIN32_WINNT_WS08  (0x0600)
#   Windows Vista                                       _WIN32_WINNT_VISTA (0x0600)
#   Windows Server 2003 with SP1, Windows XP with SP2   _WIN32_WINNT_WS03  (0x0502)
#   Windows Server 2003, Windows XP                     _WIN32_WINNT_WINXP (0x0501)
#   Windows 2000                                        _WIN32_WINNT_WIN2K (0x0500)
#   
#------------------------------------------------------------------------------
IF(WIN32)
# Currently only support Windows 10+
# this is mainly to use condition variables. Should XP be desired, would need to
# updated some code.
ADD_DEFINITIONS( -DNTDDI_VERSION=0x0A000000 -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -D__MSVCRT_VERSION__=0x1200 )
ENDIF(WIN32)

#------------------------------------------------------------------------------
# TODO: Make these conditional, for now they are always on.
#------------------------------------------------------------------------------
ADD_DEFINITIONS( -DXR_TEST_FEATURES_ENABLED=1 -DXR_DEBUG_FEATURES_ENABLED=1 )


#------------------------------------------------------------------------------
# Other Libraries
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Project Subdirectories
#------------------------------------------------------------------------------
add_subdirectory (xr)
add_subdirectory (test)

#------------------------------------------------------------------------------
# MSVC stuff to build other configs from Visual studio IDE (convenience)
#------------------------------------------------------------------------------
if(MSVC_IDE)

add_custom_target(GCC4 call "${XR_SOURCE_DIR}/build/pc-mingwgcc5-x86/all.bat"
	WORKING_DIRECTORY "${XR_SOURCE_DIR}/build/pc-mingwgcc4-x86"
	COMMENT build and test using GCC 4 (x86)
	VERBATIM)

add_custom_target(GCC4x64 call "${XR_SOURCE_DIR}/build/pc-mingwgcc5-x64/all.bat"
	WORKING_DIRECTORY "${XR_SOURCE_DIR}/build/pc-mingwgcc4-x64"
	COMMENT build and test using GCC 4 (x64)
	VERBATIM)

add_custom_target(vs14 call "${XR_SOURCE_DIR}/build/pc-vs14-x86/all.bat"
	WORKING_DIRECTORY "${XR_SOURCE_DIR}/build/pc-vs14-x86"
	COMMENT build and test using Visual Studio 2015 (x86)
	VERBATIM)
	
add_custom_target(vs14x64 call "${XR_SOURCE_DIR}/build/pc-vs14-x64/all.bat"
	WORKING_DIRECTORY "${XR_SOURCE_DIR}/build/pc-vs14-x64"
	COMMENT build and test using Visual Studio 2015 (x64)
	VERBATIM)
	
add_custom_target(vs14-cov call "${XR_SOURCE_DIR}/build/pc-vs14-x86-cov/all.bat"
	WORKING_DIRECTORY "${XR_SOURCE_DIR}/build/pc-vs14-x86-cov"
	COMMENT build and test using Visual Studio 2015 (x86)
	VERBATIM)

add_custom_target(DOXYGEN call "${XR_SOURCE_DIR}/windows.bat" docs
	COMMAND exit %ERRORLEVEL%
	WORKING_DIRECTORY "${XR_SOURCE_DIR}/"
	COMMENT Build library .chm file using doxygen
	VERBATIM)
	
endif(MSVC_IDE)
