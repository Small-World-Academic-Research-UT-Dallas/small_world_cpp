// Represents a student for the purpose of simulation

#pragma once
namespace small_world::simulation { class Student; }

#include "small_world_io/Student.hpp"
#include "small_world_simulation/SimulationParameters.hpp"

namespace small_world::simulation {

class Student final {
public:
  enum infection_status {
    Uninfected,
    Infected,
    Immune,
  };

  Student(const small_world::io::Student & student, std::shared_ptr<
    const small_world::simulation::SimulationParameters> parameters);

  // Signals exposure to an infected individual
  void infect(double amount);

  // How contagious is this student (if at all)?
  double get_contagiousness() const;

private:
  double viral_load; // Viral load accumulated throughout the day
  infection_status status; // Current status
  int infection_timer; // How long until we get better, if we are infected
  std::shared_ptr<const small_world::simulation::SimulationParameters> parameters;
};

}
