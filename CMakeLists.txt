cmake_minimum_required(VERSION 3.18)
project(small_world_cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)

add_subdirectory(modules/simulation)
add_subdirectory(modules/io)
