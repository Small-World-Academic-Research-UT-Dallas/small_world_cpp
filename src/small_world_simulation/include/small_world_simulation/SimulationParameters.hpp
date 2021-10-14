#pragma once

namespace small_world::simulation { class SimulationParameters; }

#include "small_world_io/ParameterReader.hpp"

namespace small_world::simulation {

class SimulationParameters {
public:
  SimulationParameters(const small_world::io::ParameterReader & parameters);
  double get_r_value() const;

private:
  double r_value;
};

}
