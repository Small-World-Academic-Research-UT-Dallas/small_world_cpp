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

  // Convert io::Student to simulation::Student
  auto students = data.get_students();
  std::transform(students->begin(), students->end(), back_inserter(this->students), // simulationStudents.begin(),
    [parameters](const small_world::io::Student & student) -> Student {
      return Student(student, parameters);
    }
  );
  
  // Convert io::Section to simulation::Section
  auto sections = data.get_sections();
  
  std::transform(sections->begin(), sections->end(), back_inserter(this->sections), // this->sections.begin(),
    [parameters](const small_world::io::Section & section) -> Section {
      return Section(section, parameters);
    }
  );

  // Infect student 0
  this->students.at(0).force_infection();
  std::cout << this->students.size() << std::endl;
}

const Student & Simulation::observe_student(std::size_t student) const {
  return (this->students).at(student);
}

void Simulation::infect_student(std::size_t student, float amount) {
  (this->students).at(student).infect(amount);
}

void Simulation::run(int steps) {
  int remain = steps;
  while(remain > 0) {
    remain--;
    int inf = this->run_one_step();
    std::cout << "Day " << (steps-remain) << "\tafter today " << inf << " infected" << std::endl;
  }
}

int Simulation::run_one_step() {
  std::for_each(this->sections.begin(), this->sections.end(),
    [this](Section & section) {
      section.simulate_section(*this);
    }
  );
  
  int infected = 0;
  for (Student & student : students) {
    student.simulate_night();
    // std::cout << "VL = " << student.get_viral_load() << std::endl;
    infected += student.get_contagiousness();
  }

  return infected;
}
