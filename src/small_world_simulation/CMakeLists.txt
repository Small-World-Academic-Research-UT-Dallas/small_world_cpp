add_library(small_world_simulation_lib OBJECT
src/Student.cpp
src/Section.cpp
src/Simulation.cpp
src/SimulationParameters.cpp)

target_include_directories(small_world_simulation_lib PUBLIC include)
target_link_libraries(small_world_simulation_lib small_world_io)

FIND_PACKAGE( Boost 1 COMPONENTS program_options REQUIRED )

add_executable(small_world_simulation src/main.cpp)
target_link_libraries(small_world_simulation
small_world_simulation_lib
small_world_io
${Boost_LIBRARIES})

add_executable(example_test test/example_test.cpp)
target_link_libraries(example_test gtest_main)
include(GoogleTest)
gtest_discover_tests(example_test)
