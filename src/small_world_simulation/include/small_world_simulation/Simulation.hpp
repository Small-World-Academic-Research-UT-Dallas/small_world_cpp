// Represents an entire simulation

#pragma once
namespace small_world::simulation { class Simulation; }

#include <cstddef> // size_t
#include <memory> // shared_ptr
#include <vector> //vector

#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_simulation/SimulationParameters.hpp"
#include "small_world_simulation/Student.hpp"
#include "small_world_simulation/Section.hpp"
#include "small_world_simulation/StudentPopulation.hpp"

namespace small_world::simulation {

class Simulation final : public StudentPopulation {
public:
  // Constructed from enrollment data and a set of parameters
  Simulation(const small_world::io::EnrollmentDataReader & data,
             std::shared_ptr<const small_world::simulation::SimulationParameters> parameters);

  // These methods exist to implement StudentPopulation, which is the interface
  // used by Sections during simulation to access this class
  const Student & observe_student(std::size_t student) const;
  void infect_student(std::size_t student, float amount);

private:
  std::vector<Section> sections;
  std::vector<Student> students;

  // The parameters are shared between the simulation itself and all entities
  // in the simulation, to avoid lots of copying at runtime
  std::shared_ptr<const small_world::simulation::SimulationParameters> parameters;
};

}
