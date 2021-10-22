#include <algorithm>
#include <exception>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "small_world_io/CsvEnrollmentDataReader.hpp"
#include "small_world_io/Section.hpp"
#include "small_world_io/Student.hpp"

using small_world::io::CsvEnrollmentDataReader;

CsvEnrollmentDataReader::CsvEnrollmentDataReader(std::istream& input) {
  // TODO fix IO (currently combining separate sections with the same id in the input data)
  std::ios::sync_with_stdio(false);
  input.tie(nullptr);

  std::vector<std::unordered_map<std::string, std::string>> csvData = parse_csv(input);

  // These map student and section identifiers in the CSV file to a contiguous range starting with 0
  std::unordered_map<std::string, size_t> studentMap;
  std::unordered_map<std::string, size_t> sectionMap;

  // These map the contiguous ids from studentMap and sectionMap back to their ids from the file format
  std::vector<size_t> invStudentMap;
  std::vector<size_t> invSectionMap;

  // The start and end times of each section
  std::vector<double> startTimes;
  std::vector<double> endTimes;

  std::vector<std::pair<size_t, size_t>> edges;
  edges.reserve(csvData.size());

  for (size_t i = 0; i < csvData.size(); i++) {
    std::string studentID = csvData[i]["UNIQUE_VAL"];
    std::string sectionID = csvData[i]["CLASS_NBR"] + '$'
      + csvData[i]["FACILITY_ID"] + '$'
      + csvData[i]["MEETING_TIME_START"] + '$'
      + csvData[i]["MEETING_TIME_END"] + '$'
      + csvData[i]["MON"] + '$'
      + csvData[i]["TUES"] + '$'
      + csvData[i]["WED"] + '$'
      + csvData[i]["THURS"] + '$'
      + csvData[i]["FRI"] + '$'
      + csvData[i]["SAT"] + '$'
      + csvData[i]["SUN"] + '$'
      + csvData[i]["INSTRUCTION_MODE"];
    if (i == 0)
      std::cout << sectionID << std::endl;
    // Add to map if not present
    auto studentEntry = studentMap.try_emplace(studentID, studentMap.size());
    auto sectionEntry = sectionMap.try_emplace(sectionID, sectionMap.size());

    edges.emplace_back(studentEntry.first->second, sectionEntry.first->second);

    if (sectionEntry.second) {
      startTimes.push_back(parse_time(csvData[i]["MEETING_TIME_START"]));
      endTimes.push_back(parse_time(csvData[i]["MEETING_TIME_END"]));
    }
  }

  std::cout << studentMap.size() << " students, " << sectionMap.size() << " sections" << std::endl;

  std::vector<std::vector<size_t>> studentAdj(studentMap.size());
  std::vector<std::vector<size_t>> sectionAdj(sectionMap.size());

  for (const std::pair<size_t, size_t>& edge : edges) {
    studentAdj[edge.first].push_back(edge.second);
    sectionAdj[edge.second].push_back(edge.first);
  }

  //for (const std::pair<size_t, size_t>& edge : edges) std::cout << edge.first << ' ' << edge.second << std::endl;

  std::vector<Student> students;
  students.reserve(studentMap.size());
  std::vector<Section> sections;
  sections.reserve(sectionMap.size());

  // TODO add other attributes
  for (int i = 0; i < studentMap.size(); i++)
    students.emplace_back(
      std::make_shared<std::vector<size_t>>(std::move(studentAdj[i]))
    );
  for (int i = 0; i < sectionMap.size(); i++)
    sections.emplace_back(
      std::make_shared<std::vector<size_t>>(std::move(sectionAdj[i])),
      endTimes[i] - startTimes[i]
    );

  this->students = std::make_shared<std::vector<Student>>(std::move(students));
  this->sections = std::make_shared<std::vector<Section>>(std::move(sections));
}

CsvEnrollmentDataReader::~CsvEnrollmentDataReader() {}

std::shared_ptr<const std::vector<small_world::io::Student>> CsvEnrollmentDataReader::get_students() const {
  return this->students;
}

std::shared_ptr<const std::vector<small_world::io::Section>> CsvEnrollmentDataReader::get_sections() const {
  return this->sections;
}

std::vector<std::unordered_map<std::string, std::string>> CsvEnrollmentDataReader::parse_csv(std::istream& input) {
  std::vector<std::string> header = parse_row(input);

  std::vector<std::unordered_map<std::string, std::string>> parsed;

  std::vector<std::string> row;
  int rowNum = 0;
  while ((row = parse_row(input)), input) {
    if (++rowNum % 1000 == 0)
      std::cout << "\rrow" << std::setw(7) << rowNum << std::flush;
    // All rows should have the same number of commas
    if (row.size() != header.size())
      throw std::invalid_argument("Row has the wrong number of entries.");
    // Add a new row
    parsed.emplace_back();
    parsed.back().reserve(row.size());

    for (size_t i = 0; i < row.size(); i++)
      parsed.back().emplace(header[i], row[i]);
  }
  std::cout << '\r' << std::string(10, ' ') << '\r' << std::flush;
  return parsed;
}

std::vector<std::string> CsvEnrollmentDataReader::parse_row(std::istream& input) {
  std::string line;
  std::getline(input, line);

  std::vector<std::string> row;
  size_t i = 0, j = line.find(',', 0);
  while (j != std::string::npos) {
    row.emplace_back(line.begin() + i, line.begin() + j);
    // In Enrollment Data.csv, commas were replaced with @, as they would otherwise interfere with the csv format.
    std::replace(row.back().begin(), row.back().end(), '@', ',');
    i = j + 1;
    j = line.find(',', i);
  }
  return row;
}

double CsvEnrollmentDataReader::parse_time(const std::string& str) {
  if (str.empty())
    return 0;
  size_t pos = str.find(':');
  if (pos == std::string::npos)
    throw std::invalid_argument("Invalid time \"" + str + "\"");
  std::string hStr(str.begin(), str.begin() + pos), mStr(str.begin() + pos + 1, str.end());
  std::stringstream strm(hStr + ' ' + mStr);
  int h, m;
  strm >> h >> m;
  return h + m / 60.;
}
