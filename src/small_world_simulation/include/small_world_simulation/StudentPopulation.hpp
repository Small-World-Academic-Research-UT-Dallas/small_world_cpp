// An interface, implemented by Simulation, that allows observing and infecting
// students. This is currently passed to Section on each simulation step

#pragma once
namespace small_world::simulation { class StudentPopulation; }

#include <cstddef> // size_t

#include "small_world_simulation/Student.hpp"

namespace small_world::simulation {

class StudentPopulation {
public:
  virtual const Student & observe_student(std::size_t student) const = 0;
  virtual void infect_student(std::size_t student, float amount) = 0;
};

}
