cmake_minimum_required(VERSION 2.8)
Project("arbiter")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/_cmake")

include(_cmake/config.cmake)
include(_cmake/dependencies.cmake)

add_subdirectory(arbiter)
