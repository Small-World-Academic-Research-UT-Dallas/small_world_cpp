#pragma once

namespace small_world::simulation { class Section; }

#include <cstddef>
#include <memory>
#include <vector>

/*

Represents a section of a class for the purpose of simulation

*/

#include "small_world_io/Section.hpp"
#include "small_world_simulation/StudentPopulation.hpp"

namespace small_world::simulation {

class Section {
public:
  Section(const small_world::io::Section & section);
  virtual ~Section();

  void simulate_section(StudentPopulation & simulation);

private:
  std::shared_ptr<const std::vector<std::size_t>> students;
};

}
