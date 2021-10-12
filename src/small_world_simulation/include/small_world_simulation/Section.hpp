#pragma once

namespace small_world::simulation { class Section; }

#include <cstddef>
#include <memory>
#include <vector>

/*

Represents a section of a class for the purpose of simulation

*/

#include "small_world_simulation/SectionRecord.hpp"
#include "small_world_simulation/Simulation.hpp"

namespace small_world::simulation {

class Section {
public:
  Section(const SectionRecord & section);
  virtual ~Section();

  void simulate_section(Simulation & simulation);

private:
  std::shared_ptr<const std::vector<std::size_t>> students;
};

}
