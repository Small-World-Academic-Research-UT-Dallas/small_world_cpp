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

double Student::get_viral_load() {
  return this->viral_load;
}

void Student::simulate_night() {
  // Currently 1/(1+x^(-r)) where x is viral load and r is 1.

  double x = this->viral_load;
  double r = 1; // this->parameters->get_r_value();
  double threshold =  0.5; // ((double) rand()) / ((double) RAND_MAX);

  double infection_chance = 1.0 / (1.0 + pow(x, -1*r));
  // this->viral_load = 0; // for now, there is no timer. infected forever.
  
  if(infection_chance >= threshold) this->status = infection_status::Infected;
}

void Student::force_infection() {
  this->status = infection_status::Infected;
}
