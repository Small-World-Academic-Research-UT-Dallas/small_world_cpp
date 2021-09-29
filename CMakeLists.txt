cmake_minimum_required(VERSION 3.18)
project(small_world_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)

add_subdirectory(src/small_world_io)
add_subdirectory(src/small_world_simulation)
add_subdirectory(src/small_world_static_analysis)
