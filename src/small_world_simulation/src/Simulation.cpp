#include <algorithm> //std::transform
#include <vector>

#include "small_world_simulation/Simulation.hpp"
#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_io/ParameterReader.hpp"

using namespace small_world::simulation;

Simulation::Simulation(const small_world::io::EnrollmentDataReader & data,
                       const small_world::io::ParameterReader & parameters) {
  auto students = data.get_students();
  std::transform(students->begin(), students->end(), this->students.begin(),
    [](const small_world::io::Student & student) -> Student {
      return Student(student);
    }
  );

  auto sections = data.get_sections();
  std::transform(sections->begin(), sections->end(), this->sections.begin(),
    [](const small_world::io::Section & section) -> Section {
      return Section(section);
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
