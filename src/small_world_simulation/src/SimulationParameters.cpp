#include "small_world_simulation/SimulationParameters.hpp"
#include "small_world_io/ParameterReader.hpp"
#include <iostream>

using namespace small_world::simulation;

SimulationParameters::SimulationParameters(const small_world::io::ParameterReader & parameters) {
  this->r_value = parameters.get_double("r_value");
}

void SimulationParameters::show() const {
  printf("example params\n");
}

double SimulationParameters::get_r_value() const {
  return this->r_value;
}
