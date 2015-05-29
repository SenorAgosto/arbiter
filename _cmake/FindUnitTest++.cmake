# Try to find UnitTest++
# Once done this will define
# UNITTEST++_FOUND - system has the lib 
# UNITTEST++_INCLUDE_DIRS - the path to include directory 
# UNITTEST++_LIBRARIES - the libraries needed to use
# UNITTEST++_DEFINITIONS - compiler switches required for using UnitTest++

find_package(PkgConfig)
pkg_check_modules(PC_UNITTEST++ QUIET libunittest++)
set(UNITTEST++_DEFINITIONS ${PC_UNITTEST++_CFLAGS_OTHER})

# see if there is a brew path (useful when it is not /usr/local/)
set(PC_HOMEBREW_PATH $ENV{HOMEBREW})
if(PC_HOMEBREW_PATH)
    set(PC_HOMEBREW_INCLUDE_DIRS "$ENV{HOMEBREW}/include")
    set(PC_HOMEBREW_LIBRARY_DIRS "$ENV{HOMEBREW}/lib")
endif()

find_path(UNITTEST++_INCLUDE_DIR UnitTest++/UnitTest++.h
    HINTS ${PC_UNITTEST++_INCLUDEDIR} ${PC_UNITTEST++_INCLUDE_DIRS} ${PC_HOMEBREW_INCLUDE_DIRS}
    PATH_SUFFIXES UnitTest++ )

find_library(UNITTEST++_LIBRARY NAMES UnitTest++ libUnitTest++
    HINTS ${PC_UNITTEST++_LIBDIR} ${PC_UNITTEST++_LIBRARY_DIRS} ${PC_HOMEBREW_LIBRARY_DIRS} )

set(UNITTEST++_LIBRARIES ${UNITTEST++_LIBRARY} )
set(UNITTEST++_INCLUDE_DIRS ${UNITTEST++_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs) 
find_package_handle_standard_args(UnitTest++ DEFAULT_MSG
    UNITTEST++_LIBRARY UNITTEST++_INCLUDE_DIR)

mark_as_advanced(UNITTEST++_INCLUDE_DIR UNITTEST++_LIBRARY)
 
