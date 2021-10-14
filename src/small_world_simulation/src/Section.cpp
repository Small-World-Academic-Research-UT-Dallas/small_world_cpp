#include <algorithm>
#include <memory>
#include <numeric>
#include <cstddef>

#include "small_world_simulation/Section.hpp"
#include "small_world_simulation/Simulation.hpp"
#include "small_world_simulation/SimulationParameters.hpp"
#include "small_world_io/Section.hpp"

using namespace small_world::simulation;

Section::Section(const small_world::io::Section & section, std::shared_ptr<const small_world::simulation::SimulationParameters> parameters) {
  this->parameters = parameters;
  this->students = section.get_students();
}

Section::~Section() {}

void Section::simulate_section(StudentPopulation & population) {
  (void) parameters; // There are currently no parameters that affect Sections
  double total_viral_load = std::accumulate(this->students->begin(), this->students->end(), 0.0,
    [&population](const std::size_t student_index, double current) -> double {
      return population.observe_student(student_index).get_contagiousness() + current;
    }
  );

  double average_viral_load = total_viral_load / this->students->size();
  std::for_each(this->students->begin(), this->students->end(),
    [&population, average_viral_load](const std::size_t student_index) {
      population.infect_student(student_index, average_viral_load);
    }
  );
}
