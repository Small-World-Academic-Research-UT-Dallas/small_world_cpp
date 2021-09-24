#include <istream>
#include <memory>
#include <vector>

#include "small_world_io/CsvEnrollmentDataReader.hpp"
#include "small_world_io/Section.hpp"
#include "small_world_io/Student.hpp"

using small_world::io::CsvEnrollmentDataReader;

CsvEnrollmentDataReader::CsvEnrollmentDataReader(std::istream input) {
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
