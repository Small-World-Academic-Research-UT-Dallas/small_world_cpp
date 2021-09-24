add_library(small_world_io OBJECT
  src/CsvEnrollmentDataReader.cpp
  src/JsonParameterReader.cpp
  src/Section.cpp
  src/Student.cpp)

target_include_directories(small_world_io PUBLIC include)
