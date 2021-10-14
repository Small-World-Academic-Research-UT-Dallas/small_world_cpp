#pragma once

namespace small_world::simulation { class Simulation; }

#include <cstddef>
#include <memory>
#include <vector>

#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_simulation/SimulationParameters.hpp"
#include "small_world_simulation/Student.hpp"
#include "small_world_simulation/Section.hpp"
#include "small_world_simulation/StudentPopulation.hpp"

namespace small_world::simulation {

class Simulation : public StudentPopulation {
public:
  Simulation(const small_world::io::EnrollmentDataReader & data,
             std::shared_ptr<const small_world::simulation::SimulationParameters> parameters);
  virtual ~Simulation();

  virtual const Student & observe_student(std::size_t student) const;
  virtual void infect_student(std::size_t student, float amount);

private:
  std::vector<Section> sections;
  std::vector<Student> students;
  std::shared_ptr<const small_world::simulation::SimulationParameters> parameters;
};

}
