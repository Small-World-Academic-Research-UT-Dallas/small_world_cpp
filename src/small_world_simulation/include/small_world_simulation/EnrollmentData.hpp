#pragma once

namespace small_world::simulation { class EnrollmentData; }

#include <memory>
#include <vector>

// Includes only the data we need for the simulation

#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_simulation/StudentRecord.hpp"
#include "small_world_simulation/SectionRecord.hpp"

namespace small_world::simulation {

class EnrollmentData {
public:
  EnrollmentData(const small_world::io::EnrollmentDataReader & data);
  virtual ~EnrollmentData();
  virtual std::shared_ptr<const std::vector<StudentRecord>> get_students() const;
  virtual std::shared_ptr<const std::vector<SectionRecord>> get_sections() const;

private:
  std::shared_ptr<std::vector<StudentRecord>> students;
  std::shared_ptr<std::vector<SectionRecord>> sections;
};

}
