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
#include "small_world_simulation/SimulationParameters.hpp"

namespace small_world::simulation {

class Section {
public:
  Section(const small_world::io::Section & section, std::shared_ptr<const small_world::simulation::SimulationParameters> parameters);
  virtual ~Section();

  void simulate_section(StudentPopulation & population);

private:
  std::shared_ptr<const small_world::simulation::SimulationParameters> parameters;
  std::shared_ptr<const std::vector<std::size_t>> students;
};

}
