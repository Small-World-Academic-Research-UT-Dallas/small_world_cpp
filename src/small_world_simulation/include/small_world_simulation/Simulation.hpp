#pragma once

namespace small_world::simulation { class Simulation; }

#include <cstddef>
#include <memory>
#include <vector>

#include "small_world_simulation/EnrollmentData.hpp"
#include "small_world_simulation/Student.hpp"
#include "small_world_simulation/Section.hpp"

namespace small_world::simulation {

class Simulation {
public:
  Simulation(const EnrollmentData & data);
  virtual ~Simulation();

private:

};

}
