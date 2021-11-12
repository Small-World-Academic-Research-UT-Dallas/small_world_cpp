#include <boost/program_options.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "small_world_io/CsvEnrollmentDataReader.hpp"
#include "small_world_io/JsonParameterReader.hpp"
#include "small_world_simulation/Simulation.hpp"
#include "small_world_simulation/SimulationParameters.hpp"

int main(int argc, char ** argv) {
  // Create and parse options map
  boost::program_options::options_description options("Options");
  options.add_options()
    ("help", "Display help and exit")
    ("parameters", boost::program_options::value<std::string>(), "The location of the parameter file")
    ("data", boost::program_options::value<std::string>(), "The location of the data file")
  ;

  boost::program_options::variables_map options_map;
  boost::program_options::store(boost::program_options::parse_command_line(
    argc, argv, options), options_map);
  boost::program_options::notify(options_map);

  // The default boost handling of missing options is ugly, so we'll handle it
  // ourselves
  if(options_map.count("help")) {
    std::cerr << options << std::endl;
    return 1;
  }

  if(options_map.count("parameters") != 1) {
    std::cerr << "Error: 'parameters' is a required option" << std::endl;
    std::cerr << std::endl << options << std::endl;
    return 1;
  }

  if(options_map.count("data") != 1) {
    std::cerr << "Error: 'data' is a required option" << std::endl;
    std::cerr << std::endl << options << std::endl;
    return 1;
  }

  // Open and parse the data file
  const std::string data_filename = options_map["data"].as<std::string>();
  std::ifstream data_file(data_filename);
  if(! data_file) {
    std::cerr << "Error: Could not open data file '" << data_filename << "'"
      << std::endl;
    return 1;
  }
  small_world::io::CsvEnrollmentDataReader enrollment_data(data_file);

  // Open and parse the parameter file
  // Todo merge.
  const std::string parameter_filename = options_map["parameters"].as<std::string>();
  std::ifstream parameter_file(parameter_filename);
  if(! parameter_file) {
    std::cerr << "Error: Could not open parameter file '" << parameter_filename
      << "'" << std::endl;
    return 1;
  }
  small_world::io::JsonParameterReader parameter_reader(parameter_file);
  auto parameters = std::make_shared<const small_world::simulation::SimulationParameters>(parameter_reader);
  small_world::simulation::Simulation simulation(enrollment_data, parameters);

  srand(time(NULL));
  simulation.run(20);
}
