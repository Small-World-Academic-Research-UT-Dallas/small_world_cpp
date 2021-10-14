#include <cmath>

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

void Student::infect(double amount) {
  this->viral_load += amount;
}

double Student::get_contagiousness() const {
  return (this->status == infection_status::Infected) ? 1.0 : 0.0;
}

void Student::simulate_night() {
  double infection_chance = 1.0 / (1.0 + pow(this->viral_load, this->parameters->get_r_value()));
  this->viral_load = 0;
  double rng = ((double) rand()) / ((double) RAND_MAX);
  if(rng < infection_chance) this->status = infection_status::Infected;
}

void Student::force_infection() {
  this->status = infection_status::Infected;
}
