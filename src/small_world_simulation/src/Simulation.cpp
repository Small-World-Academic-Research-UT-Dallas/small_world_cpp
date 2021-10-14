#include <algorithm> //std::transform
#include <memory>
#include <vector>

#include "small_world_simulation/Simulation.hpp"
#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_io/ParameterReader.hpp"

using namespace small_world::simulation;

Simulation::Simulation(const small_world::io::EnrollmentDataReader & data,
                       std::shared_ptr<const small_world::simulation::SimulationParameters> parameters) {
  this->parameters = parameters;

  auto students = data.get_students();
  std::transform(students->begin(), students->end(), this->students.begin(),
    [parameters](const small_world::io::Student & student) -> Student {
      return Student(student, parameters);
    }
  );

  auto sections = data.get_sections();
  std::transform(sections->begin(), sections->end(), this->sections.begin(),
    [parameters](const small_world::io::Section & section) -> Section {
      return Section(section, parameters);
    }
  );
}

Simulation::~Simulation() {}

const Student & Simulation::observe_student(std::size_t student) const {
  return this->students.at(student);
}

void Simulation::infect_student(std::size_t student, float amount) {
  this->students.at(student).infect(amount);
}
