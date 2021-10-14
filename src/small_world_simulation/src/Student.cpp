#include "small_world_simulation/Student.hpp"
#include "small_world_simulation/SimulationParameters.hpp"
#include "small_world_io/Student.hpp"

using namespace small_world::simulation;

Student::Student(const small_world::io::Student & student, std::shared_ptr<const small_world::simulation::SimulationParameters> parameters) {
  (void) student; // There is nothing currently in this record that we need
  this->parameters = parameters;
  this->viral_load = 0;
  this->status = infection_status::Uninfected;
  this->infection_timer = 0;
}

Student::~Student() {
  // Nothing here for now
}

void Student::infect(double amount) {
  this->viral_load += amount;
}

double Student::get_contagiousness() const {
  return (this->status == infection_status::Infected) ? this->parameters->get_r_value() : 0.0;
}
