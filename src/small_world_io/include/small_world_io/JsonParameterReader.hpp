#pragma once

#include <memory>
#include <istream>
#include <string>

#include "small_world_io/ParameterReader.hpp"

/*

A concrete implementation of ParameterReader that relies on JSON-formatted data
for its input.

*/

namespace small_world::io {

class JsonParameterReader : ParameterReader {
public:
  JsonParameterReader(std::unique_ptr<std::istream> input);
  virtual ~JsonParameterReader();
  virtual double get_double(std::string name); // Get a paramter as a double
};

}
