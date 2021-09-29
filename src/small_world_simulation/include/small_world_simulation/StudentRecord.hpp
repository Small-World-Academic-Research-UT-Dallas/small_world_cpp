#pragma once

/*

All the static data needed for each student in the simulation. Currently, no
data is needed, but that will likely change over time.

*/

#include "small_world_io/Student.hpp"

namespace small_world::simulation {

class StudentRecord {
public:
  StudentRecord();
  StudentRecord(const small_world::io::Student & student);
  virtual ~StudentRecord();
};

}
