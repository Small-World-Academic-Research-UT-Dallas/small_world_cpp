#include <memory>
#include <istream>

#include "small_world_io/JsonParameterReader.hpp"

using small_world::io::JsonParameterReader;

JsonParameterReader::JsonParameterReader(std::unique_ptr<std::istream> input) {

}

JsonParameterReader::~JsonParameterReader() {

}

double JsonParameterReader::get_double(std::string name) {
  return 0; // TODO
}
