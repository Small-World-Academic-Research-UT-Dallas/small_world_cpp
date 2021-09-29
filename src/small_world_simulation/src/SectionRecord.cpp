#include <cstddef>
#include <memory>
#include <vector>

#include "small_world_simulation/SectionRecord.hpp"
#include "small_world_io/Section.hpp"

using small_world::simulation::SectionRecord;

SectionRecord::SectionRecord(const small_world::io::Section & section) {
  this->students = std::make_shared<std::vector<std::size_t>>(*(section.get_students()));
}

SectionRecord::SectionRecord(std::shared_ptr<const std::vector<std::size_t>> students) {
  this->students = students;
}

SectionRecord::~SectionRecord() {}

std::shared_ptr<const std::vector<std::size_t>> SectionRecord::get_students() const {
  return this->students;
}
