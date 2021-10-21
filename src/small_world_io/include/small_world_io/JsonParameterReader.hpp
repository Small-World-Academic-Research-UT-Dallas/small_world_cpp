#pragma once

#include <memory>
#include <istream>
#include <string>
#include <nlohmann/json.hpp>

#include "small_world_io/ParameterReader.hpp"

/*

A concrete implementation of ParameterReader that relies on JSON-formatted data
for its input.

*/

namespace small_world::io {

class JsonParameterReader : public ParameterReader {
public:
  JsonParameterReader(std::istream & input);
  virtual ~JsonParameterReader();
  virtual double get_double(std::string name) const; // Get a paramter as a double
private:
  nlohmann::json parameters;
};

}
