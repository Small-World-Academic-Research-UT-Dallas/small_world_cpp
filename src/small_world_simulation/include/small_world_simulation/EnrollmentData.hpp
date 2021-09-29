#pragma once

// Includes only the data we need for the simulation

#include "small_world_io/EnrollmentDataReader.hpp"

namespace small_world::simulation {

class EnrollmentData {
public:
  EnrollmentData(const small_world::io::EnrollmentDataReader & data);
  
  virtual ~EnrollmentData();
}

}
