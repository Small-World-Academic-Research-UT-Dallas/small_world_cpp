#pragma once

#include <memory>
#include <vector>

#include "small_world_io/Section.hpp"
#include "small_world_io/Student.hpp"

namespace small_world::io {

class EnrollmentDataReader {
public:
  virtual ~EnrollmentDataReader() {};
  virtual std::shared_ptr<const std::vector<Student>> get_students() const = 0;
  virtual std::shared_ptr<const std::vector<Section>> get_sections() const = 0;
};

}
