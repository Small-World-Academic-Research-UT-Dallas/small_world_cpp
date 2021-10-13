add_library(small_world_simulation_lib OBJECT
src/SectionRecord.cpp
src/StudentRecord.cpp
src/EnrollmentData.cpp
src/Student.cpp
src/Section.cpp
src/Simulation.cpp)

target_include_directories(small_world_simulation_lib PUBLIC include)
target_link_libraries(small_world_simulation_lib small_world_io)

FIND_PACKAGE( Boost 1 COMPONENTS program_options REQUIRED )

add_executable(small_world_simulation src/main.cpp)
target_link_libraries(small_world_simulation
small_world_simulation_lib
small_world_io
${Boost_LIBRARIES})
