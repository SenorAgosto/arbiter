file( GLOB interface_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h *.hpp)
file( GLOB implementation_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h src/*.hpp src/*.c src/*.cpp)
file( GLOB details_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} details/*.h details/*.hpp)
file( GLOB states_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} details/states/*.h details/states/*.hpp)
file( GLOB platform_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} platform/*)

source_group("Source" FILES ${implementation_files})
source_group("Interface" FILES ${interface_files})
source_group("Interface\\Details" FILES ${details_headers})
source_group("Interface\\Details\\States" FILES ${states_headers})
source_group("Interface\\Platform" FILES ${platform_headers})

add_library( arbiter ${implementation_files} ${interface_files} ${platform_headers} ${details_headers} ${states_headers})

add_subdirectory(tests/unit_test)
add_subdirectory(tests/acceptance_test)
