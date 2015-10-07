add_definitions(
	    --std=c++11

            -Wall
            -Wextra 

            -Wno-unknown-pragmas
            -Wno-unused-local-typedefs
            -Wno-unused-variable
        )

if(${CMAKE_BUILD_TYPE} MATCHES "Debug") 
    add_definitions(
	-g
	-O0
    )
endif() 

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    add_definitions(
	-O4
    )
endif()
