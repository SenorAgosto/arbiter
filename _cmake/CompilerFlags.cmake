if(WIN32)
	add_definitions(
		/DNOMINMAX	#disable creation of min/max macro
		/EHa
		/FC			#use full paths in error messages, __FILE__
		/WX         # Treat warnings as Errors
		/Wall		# Turn on all warnings
		/Zi         # Program Database
		
		# C++ Disabled warnings
		/wd4710		# function not inlined
		/wd4820		# padding added to the end of data structure.
		/wd4514		# unreferenced inline function removed.
		/wd4625		# derived class copy constructor cannot be generated because base class copy constructor inaccessible.
		/wd4626		# derived class assignment operator cannot be generated because base class operator inaccessible.
		/wd4640		# construction of local static objects is not thread-safe.
		/wd4668		# preprocessor directive not defined, this is normal.
	)
else()
	# currently assumes we are building on MacOSX using clang++.
	add_definitions(
			-std=c++11		# use c++11 features
	)

    if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        add_definitions(
            -Wall
            -Wextra 

            -Wno-unknown-pragmas
            -Wno-unused-local-typedefs
            -Wno-unused-variable
        )
    endif() 

    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")	
        add_definitions(
			-stdlib=libc++  # use the good runtime

            # Warnings as errors
            -Weverything
            -Werror

            # Disabled Warnings
            -Wno-unknown-pragmas
            -Wno-c++98-compat
            -Wno-global-constructors
            -Wno-exit-time-destructors
            -Wno-padded
            -Wno-undef
            -Wno-c++98-compat-pedantic
            -Wno-deprecated
            -Wno-documentation
            -Wno-weak-vtables
            -Wno-disabled-macro-expansion

            # Temp...
            -Wno-unneeded-member-function 
            -Wno-unused-member-function
        )
    endif()

endif()
