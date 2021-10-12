#pragma once

namespace small_world::simulation { class Student; }

/*

Represents a single student for the purpose of simulation

*/

#include "small_world_simulation/StudentRecord.hpp"

namespace small_world::simulation {

class Student {
public:
  enum infection_status {
    Uninfected,
    Infected,
    Immune,
  };

  Student(const StudentRecord & student); // Construct from a record
  virtual ~Student();

private:
  float viral_load;
  infection_status status;
  int infection_timer;
};

}
