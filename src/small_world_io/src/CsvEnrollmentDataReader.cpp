#include <istream>
#include <fstream>
#include <memory>
#include <vector>
#include <iostream>

#include "small_world_io/CsvEnrollmentDataReader.hpp"
#include "small_world_io/Section.hpp"
#include "small_world_io/Student.hpp"
#include "fstream"
#include "algorithm"
#include "string.h"
#define len 64 // 48

using small_world::io::CsvEnrollmentDataReader;

CsvEnrollmentDataReader::CsvEnrollmentDataReader(std::ifstream &input) {
  std::cout << "Reading input file..." << std::endl;
  
  std::string line;
  int i = 0;
  if (input.is_open()) {
    while (std::getline(input, line)) {
      // process line
      char *chrstr = const_cast<char*>(line.c_str());
  
      char *arr[len];
      int n = 0;
      char *curr = strtok(chrstr, "\t");
      arr[0] = curr;

      while (curr != NULL) {
        curr = strtok(NULL, "\t");
        arr[++n] = curr;
      }
      
      // process arr
      // std::cout << arr[0] << " in " << arr[5] << "\r";

      printf("%.2f%\r", (i/620403.0)*100);
      i++;
    }
    std::cout << "\nDone" << std::endl;
    input.close();
  }
  this->students = std::make_shared<std::vector<small_world::io::Student>>();
  this->sections = std::make_shared<std::vector<small_world::io::Section>>();
}

CsvEnrollmentDataReader::~CsvEnrollmentDataReader() {}

std::shared_ptr<const std::vector<small_world::io::Student>> CsvEnrollmentDataReader::get_students() const {
  return this->students;
}

std::shared_ptr<const std::vector<small_world::io::Section>> CsvEnrollmentDataReader::get_sections() const {
  return this->sections;
}
