# UnitTest++ 
find_package(UnitTest++ REQUIRED)
include_directories(${UNITTEST++_INCLUDE_DIRS})

set(platform_unit_test_lib ${UNITTEST++_LIBRARIES})
