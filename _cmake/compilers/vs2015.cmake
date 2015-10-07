add_definitions(
		#/D_CRT_SECURE_NO_WARNINGS
		/DNOMINMAX	#disable creation of min/max macro
		/EHa
		/FC			#use full paths in error messages, __FILE__
		/WX         # Treat warnings as Errors
		/Wall		# Turn on all warnings
		/Zi         # Program Database
		
		# C++ Disabled warnings
		/wd4068		# ignore unknown pragmas (for other compilers)
		/wd4710		# function not inlined
		/wd4820		# padding added to the end of data structure.
		/wd4514		# unreferenced inline function removed.
		/wd4625		# derived class copy constructor cannot be generated because base class copy constructor inaccessible.
		/wd4626		# derived class assignment operator cannot be generated because base class operator inaccessible.
		/wd4640		# construction of local static objects is not thread-safe.
		/wd4668		# preprocessor directive not defined, this is normal.
	)