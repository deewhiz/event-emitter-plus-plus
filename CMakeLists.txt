project (event-emitter++)
cmake_minimum_required(VERSION 2.8)

# project version
set(EE_VERSION_MAJOR 1)
set(EE_VERSION_MINOR 0)
set(EE_VERSION_PATCH 0)

# just in case
add_compile_options(-std=c++11)

set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${INCLUDE_ROOT})

# add example project
add_subdirectory("example")
