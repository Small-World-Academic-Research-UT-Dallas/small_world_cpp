#include <iostream>
#include <boost/program_options.hpp>

#include "small_world_io/CsvEnrollmentDataReader.hpp"
#include "small_world_simulation/EnrollmentData.hpp"
#include "small_world_simulation/Simulation.hpp"

int main(int argc, char ** argv) {
  boost::program_options::options_description options("Options");
  options.add_options()
    ("help", "Display help and exit")
    ("parameters", "The location of the parameter file")
  ;

  boost::program_options::variables_map options_map;
  boost::program_options::store(boost::program_options::parse_command_line(
    argc, argv, options), options_map);
  boost::program_options::notify(options_map);

  if(options_map.count("help")) {
    std::cout << options << std::endl;
    return 1;
  }

  std::cout << "Hello World! (simulation)" << std::endl;
}
