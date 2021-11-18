#include <algorithm>
#include <memory>
#include <numeric>
#include <cstddef>
#include <iostream>

#include "small_world_simulation/Section.hpp"
#include "small_world_simulation/Simulation.hpp"
#include "small_world_simulation/SimulationParameters.hpp"
#include "small_world_io/Section.hpp"

using namespace small_world::simulation;

Section::Section(const small_world::io::Section & section, std::shared_ptr<const small_world::simulation::SimulationParameters> parameters) {
  this->parameters = parameters;
  this->students = section.get_students();
}

void Section::simulate_section(StudentPopulation & population) const {
  (void) parameters; // There are currently no parameters that affect Sections
  bool verbose = false;
  
  double total_viral_load = std::accumulate(this->students->begin(), this->students->end(), 0.0,
    [&population](double current, const std::size_t student_index) -> double {
      return current + population.observe_student(student_index).get_contagiousness();
    }
  );
  
  
  double average_viral_load = total_viral_load / this->students->size();
  if (verbose) {
    std::cout << "section --" << std::endl;
    std::cout << "total VL " << total_viral_load << std::endl;
    std::cout << "avg VL " << total_viral_load << "/" << this->students->size() << "=" << average_viral_load << std::endl;
  }
  
  std::for_each(this->students->begin(), this->students->end(),
    [average_viral_load, &population](const std::size_t student_index) {
      population.infect_student(student_index, average_viral_load);
    }
  );
  
}
