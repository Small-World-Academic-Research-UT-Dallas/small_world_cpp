#include <algorithm> //std::transform
#include <memory>
#include <vector>
#include <iostream> 
#include "small_world_simulation/Simulation.hpp"
#include "small_world_simulation/Student.hpp"
#include "small_world_simulation/Section.hpp"
#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_io/ParameterReader.hpp"

using namespace small_world::simulation;

Simulation::Simulation(const small_world::io::EnrollmentDataReader & data,
                       std::shared_ptr<const small_world::simulation::SimulationParameters> parameters) {
  // Store parameters for use later on in the simulation
  this->parameters = parameters;
  auto simulationStudents = this->students;
  auto simulationSections = this->sections;

  // Convert io::Student to simulation::Student
  auto students = data.get_students();
  std::transform(students->begin(), students->end(), back_inserter(simulationStudents), // simulationStudents.begin(),
    [parameters](const small_world::io::Student & student) -> Student {
      return Student(student, parameters);
    }
  );
  
  // Convert io::Section to simulation::Section
  auto sections = data.get_sections();
  
  std::transform(sections->begin(), sections->end(), back_inserter(simulationSections), // this->sections.begin(),
    [parameters](const small_world::io::Section & section) -> Section {
      return Section(section, parameters);
    }
  );
  
  // Infect student 0
  simulationStudents.at(0).force_infection();
}

const Student & Simulation::observe_student(std::size_t student) const {
  return (this->students).at(student);
}

void Simulation::infect_student(std::size_t student, float amount) {
  (this->students).at(student).infect(amount);
}

void Simulation::run(int steps) {
  while(steps > 0) {
    steps--;
    this->run_one_step();
  }
}

void Simulation::run_one_step() {
  std::for_each((this->sections).begin(), (this->sections).end(),
    [this](Section & section) {
      section.simulate_section(*this);
    }
  );

  std::for_each((this->students).begin(), (this->students).end(),
    [this](Student & student) {
      student.simulate_night();
    }
  );
}
