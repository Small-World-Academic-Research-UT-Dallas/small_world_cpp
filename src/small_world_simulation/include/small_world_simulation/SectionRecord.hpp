#pragma once

#include <cstddef>
#include <memory>
#include <vector>

// Includes all static data about a section needed for simulation
// For now, this is just a copy of the io version

#include "small_world_io/Section.hpp"

namespace small_world::simulation {

class SectionRecord {
public:
  SectionRecord(const small_world::io::Section & section);
  SectionRecord(std::shared_ptr<const std::vector<std::size_t>> students);
  virtual ~SectionRecord();

  virtual std::shared_ptr<const std::vector<std::size_t>> get_students() const final;

private:
  std::shared_ptr<const std::vector<std::size_t>> students;
};

}
