#include "small_world_simulation/StudentRecord.hpp"
#include "small_world_simulation/Student.hpp"

using namespace small_world::simulation;

Student::Student(const StudentRecord & student) {
  (void) student; // There is nothing currently in this record that we need
  this->viral_load = 0;
  this->status = infection_status::Uninfected;
  this->infection_timer = 0;
}

Student::~Student() {
  // Nothing here for now
}
