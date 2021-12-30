#include <memory>
#include <istream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "small_world_io/JsonParameterReader.hpp"

using small_world::io::JsonParameterReader;

JsonParameterReader::JsonParameterReader(std::istream & input) {
  try{
    this->parameters = nlohmann::json::parse(input);
  } catch (nlohmann::json::parse_error& er) {
  	std::cerr << "Json parsing error at byte " << er.byte << std::endl;
  	exit(1);
  }
}

JsonParameterReader::~JsonParameterReader() {

}

double JsonParameterReader::get_double(std::string name) const {
  try {
  	return parameters.at(name);
  } catch (nlohmann::json::out_of_range& er) {
  	std::cerr << "Could not find key: " << name << " in parameter file" << std::endl;
  	exit(1);

  } catch (nlohmann::json::type_error& er) {
  	std::cerr << "Value at key: " << name << " is of non-numeric type" << std::endl;
  	exit(1);
  }
  return 0.0;  
}
