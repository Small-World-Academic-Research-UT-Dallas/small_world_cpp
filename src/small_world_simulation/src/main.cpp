#include <istream>
#include <iostream>
#include <fstream>
#include "small_world_io/CsvEnrollmentDataReader.hpp"

int main() {
  std::cout << "Hello World! (simulation)" << std::endl;
  
  std::ifstream file; 
  file.open("../input/enrollment_data.tsv"); // set path here

  small_world::io::CsvEnrollmentDataReader reader = small_world::io::CsvEnrollmentDataReader(file);
}
