#pragma once

#include <cstddef>
#include <memory>
#include <vector>

/*

This class represents all of the data we have about a student. I've started with
simply what classes the student is in, but this will obviously grow quickly. I
just don't know what else we need to track for sure yet.

The sections that a student is in are indexed by integers of size_t

PS - if we track something like year, it's probably best to use enums rather
than "magic number" integers

*/

namespace small_world::io {

class Student {
public:
  Student(std::shared_ptr<const std::vector<std::size_t>> sections);
  virtual ~Student();

  virtual std::shared_ptr<const std::vector<std::size_t>> get_sections() const final;

private:
  std::shared_ptr<const std::vector<std::size_t>> sections;
};

}
