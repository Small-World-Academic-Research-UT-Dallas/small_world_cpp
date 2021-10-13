#pragma once

namespace small_world::simulation { class Simulation; }

#include <cstddef>
#include <memory>
#include <vector>

#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_simulation/Student.hpp"
#include "small_world_simulation/Section.hpp"

namespace small_world::simulation {

class Simulation {
public:
  Simulation(const small_world::io::EnrollmentDataReader & data);
  virtual ~Simulation();

private:

};

}
