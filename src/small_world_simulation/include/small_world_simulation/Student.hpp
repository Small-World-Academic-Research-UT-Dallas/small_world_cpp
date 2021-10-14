#pragma once

namespace small_world::simulation { class Student; }

/*

Represents a single student for the purpose of simulation

*/

#include "small_world_io/Student.hpp"
#include "small_world_simulation/SimulationParameters.hpp"

namespace small_world::simulation {

class Student {
public:
  enum infection_status {
    Uninfected,
    Infected,
    Immune,
  };

  Student(const small_world::io::Student & student, std::shared_ptr<const small_world::simulation::SimulationParameters> parameters);
  virtual ~Student();

  void infect(double amount);
  double get_contagiousness() const;

private:
  double viral_load;
  infection_status status;
  int infection_timer;
  std::shared_ptr<const small_world::simulation::SimulationParameters> parameters;
};

}
