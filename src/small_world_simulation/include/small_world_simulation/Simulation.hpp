#pragma once

namespace small_world::simulation { class Simulation; }

#include <vector>

#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_io/ParameterReader.hpp"
#include "small_world_simulation/Student.hpp"
#include "small_world_simulation/Section.hpp"

namespace small_world::simulation {

class Simulation {
public:
  Simulation(const small_world::io::EnrollmentDataReader & data,
             const small_world::io::ParameterReader & parameters);
  virtual ~Simulation();

private:
  std::vector<Section> sections;
  std::vector<Student> students;
};

}
