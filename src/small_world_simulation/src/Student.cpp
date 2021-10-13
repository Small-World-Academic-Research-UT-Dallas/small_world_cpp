#include "small_world_simulation/Student.hpp"
#include "small_world_io/Student.hpp"

using namespace small_world::simulation;

Student::Student(const small_world::io::Student & student) {
  (void) student; // There is nothing currently in this record that we need
  this->viral_load = 0;
  this->status = infection_status::Uninfected;
  this->infection_timer = 0;
}

Student::~Student() {
  // Nothing here for now
}

void Student::infect(float amount) {
  this->viral_load += amount;
}
