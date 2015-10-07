include_directories(${CMAKE_SOURCE_DIR})

# test and load appropriate platform configurations 
if(WIN32)
    include(_cmake/platforms/win32.cmake)
endif()

if(MINGW)
    include(_cmake/platforms/mingw.cmake)
endif()

if(MSYS)
    include(_cmake/platforms/msys.cmake)
endif()

if(CYGWIN)
    include(_cmake/platforms/cygwin.cmake)
endif()

if(UNIX)
    include(_cmake/platforms/unix.cmake)
endif()


# test and load appropriate compiler configurations 
set(cxx_compiler ${CMAKE_CXX_COMPILER_ID})
if(${cxx_compiler} MATCHES "GNU")
    message("Configuring GCC.")
    include(_cmake/compilers/gcc49.cmake)
endif()

if(${cxx_compiler} MATCHES "Clang")
    message("Configuring Clang.")
    include(_cmake/compilers/clang.cmake)
endif()

if(${cxx_compiler} MATCHES "MSVC14")
    message("Configuring Visual Studio.")
    include(_cmake/compilers/vs2015.cmake)
endif()
