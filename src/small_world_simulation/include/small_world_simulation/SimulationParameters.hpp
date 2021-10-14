// This class exists to parse parameters from the ParameterReader up front,
// and then provide a type-safe interface to the rest of the code.

#pragma once
namespace small_world::simulation { class SimulationParameters; }

#include "small_world_io/ParameterReader.hpp"

namespace small_world::simulation {

class SimulationParameters final {
public:
  SimulationParameters(const small_world::io::ParameterReader & parameters);
  double get_r_value() const;

private:
  double r_value;
};

}
