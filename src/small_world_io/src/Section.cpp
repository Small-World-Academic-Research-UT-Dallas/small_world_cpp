#include "small_world_io/Section.hpp"

using small_world::io::Section;

Section::Section(std::shared_ptr<const std::vector<std::size_t>> students) {
  this->students = students;
}

Section::~Section() {}

std::shared_ptr<const std::vector<std::size_t>> Section::get_students() const {
  return this->students;
}
