#include "small_world_io/Student.hpp"

using small_world::io::Student;

Student::Student(std::shared_ptr<const std::vector<std::size_t>> sections) {
  this->sections = sections;
}

Student::~Student() {}

std::shared_ptr<const std::vector<std::size_t>> Student::get_sections() const {
  return this->sections;
}
