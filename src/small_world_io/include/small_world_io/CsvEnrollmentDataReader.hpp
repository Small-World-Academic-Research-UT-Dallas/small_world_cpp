#pragma once

#include <istream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_io/Section.hpp"
#include "small_world_io/Student.hpp"

namespace small_world::io {

class CsvEnrollmentDataReader : EnrollmentDataReader {
public:
  CsvEnrollmentDataReader(std::istream& input);
  virtual ~CsvEnrollmentDataReader();
  virtual std::shared_ptr<const std::vector<Student>> get_students() const;
  virtual std::shared_ptr<const std::vector<Section>> get_sections() const;

private:
  static std::vector<std::unordered_map<std::string, std::string>> parse_csv(std::istream& input);
  static std::vector<std::string> parse_row(std::istream& input);

  static double parse_time(const std::string& str);

  std::shared_ptr<const std::vector<Student>> students;
  std::shared_ptr<const std::vector<Section>> sections;
};

}
