#pragma once

#include <cstddef>
#include <memory>
#include <vector>

/*

Almost exactly like the Student class, but for Sections

*/

namespace small_world::io {

class Section {
public:
  Section(std::shared_ptr<const std::vector<std::size_t>> students);
  virtual ~Section();

  std::shared_ptr<const std::vector<std::size_t>> get_students() const;

private:
  std::shared_ptr<const std::vector<std::size_t>> students;
};

}
