file( GLOB interface_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h *.hpp)
file( GLOB implementation_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h src/*.hpp src/*.c src/*.cpp)
file( GLOB details_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} details/*.h details/*.hpp)
file( GLOB platform_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} platform/*)

source_group("Source" FILES ${implementation_files})
source_group("Interface" FILES ${interface_files})
source_group("Interface\\Details" FILES ${details_headers})
source_group("Interface\\Platform" FILES ${platform_headers})

if(WIN32)
	include_directories("\\lib\\x64\\vc110\\Boost\\1.52.0\\include")
else()
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)

	find_package(Boost COMPONENTS filesystem system REQUIRED) 
	
	if(Boost_FOUND)
		include_directories(${Boost_INCLUDE_DIR})
		#link_directories(${Boost_LIBRARY_DIR})
		add_definitions("-DHAS_BOOST")
	endif() 	
endif()

add_library( arbiter ${implementation_files} ${interface_files} ${platform_headers} ${details_headers})

add_subdirectory(tests/unit_test)
add_subdirectory(tests/acceptance_test)
