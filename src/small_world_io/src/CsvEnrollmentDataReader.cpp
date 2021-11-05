#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "small_world_io/CsvEnrollmentDataReader.hpp"
#include "small_world_io/Section.hpp"
#include "small_world_io/Student.hpp"

using small_world::io::CsvEnrollmentDataReader;

std::vector<std::unordered_map<std::string, std::string>> parse_csv(std::istream& input);
std::vector<std::string> parse_row(std::istream& input);

std::string get_key(const std::unordered_map<std::string, std::string>& row, const std::vector<std::string>& fields);

small_world::io::Student make_student(const std::unordered_map<std::string, std::string>& row, const std::unordered_set<std::size_t>& adj);
small_world::io::Section make_section(const std::unordered_map<std::string, std::string>& row, const std::unordered_set<std::size_t>& adj);

// Sentinel types used as a type argument for parse
struct Date {};
struct Time {};

template<typename T>
struct Parse {
  static T parse(const std::string& str);
};
template<>
struct Parse<std::string> {
  static const std::string& parse(const std::string& str);
};
template<>
struct Parse<bool> {
  static bool parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Student::Career> {
  static small_world::io::Student::Career parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Student::Program> {
  static small_world::io::Student::Program parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Student::Organization> {
  static small_world::io::Student::Organization parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Student::Group> {
  static small_world::io::Student::Group parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Student::Plan> {
  static small_world::io::Student::Plan parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Student::Subplan> {
  static small_world::io::Student::Subplan parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Section::Session> {
  static small_world::io::Section::Session parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Section::Subject> {
  static small_world::io::Section::Subject parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Section::Group> {
  static small_world::io::Section::Group parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Section::Organization> {
  static small_world::io::Section::Organization parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Section::Component> {
  static small_world::io::Section::Component parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Section::Facility> {
  static small_world::io::Section::Facility parse(const std::string& str);
};
template<>
struct Parse<small_world::io::Section::Mode> {
  static small_world::io::Section::Mode parse(const std::string& str);
};
template<>
struct Parse<Date> {
  static time_t parse(const std::string& str);
};
template<>
struct Parse<Time> {
  static time_t parse(const std::string& str);
};
template<typename T>
auto parse(const std::string& str) { return Parse<T>::parse(str); }
template<typename T>
auto parse(const std::unordered_map<std::string, std::string>& row, const std::string& field) -> decltype(Parse<T>::parse(row.find(field)->second));

const std::vector<std::string> STUDENT_KEY_FIELDS {
  "STRM",
  "UNIQUE_VAL"
};

const std::vector<std::string> SECTION_KEY_FIELDS {
  "STRM",
  "CLASS_NBR",
  "CRSE_ID",
  "SESSION_CODE",
  "FACILITY_ID",
  "UNT_TAKEN",
  "MTG_PAT_START_DT",
  "MTG_PAT_END_DT",
  "MEETING_TIME_START",
  "MEETING_TIME_END",
  "MEETING_SCHEDULE"
};

CsvEnrollmentDataReader::CsvEnrollmentDataReader(std::istream& input) {
  // Useful for debugging, but substantially degrades performance
  constexpr bool VERIFY_UNIQUENESS = false;

  std::vector<std::unordered_map<std::string, std::string>> data = parse_csv(input);
  std::cout << "Data read" << std::endl;
  std::size_t n = data.size();
  
  // Identify repeated students and sections
  // key -> id, id is ordered by index of the first row with the key
  std::unordered_map<std::string, std::size_t> studentIds, sectionIds;
  // row -> id
  std::vector<std::size_t> rowStudents(n), rowSections(n);
  // id -> adjacency set
  std::unordered_map<std::size_t, std::unordered_set<std::size_t>> studentAdj, sectionAdj;
  for (std::size_t i = 0; i < n; i++) {
    std::string studentKey = get_key(data[i], STUDENT_KEY_FIELDS);
    std::string sectionKey = get_key(data[i], SECTION_KEY_FIELDS);
    std::size_t studentId = studentIds.try_emplace(studentKey, studentIds.size()).first->second;
    std::size_t sectionId = sectionIds.try_emplace(sectionKey, sectionIds.size()).first->second;
    rowStudents[i] = studentId;
    rowSections[i] = sectionId;
    
    bool stInsert = studentAdj[studentId].insert(sectionId).second;
    bool seInsert = sectionAdj[sectionId].insert(studentId).second;
    // Edges should be unique
    assert(stInsert && seInsert);
  }
  std::size_t st = studentIds.size(), se = sectionIds.size();

  std::cout << "Graph edges generated" << std::endl;

  // Construct Student and Section objects, ordered by first row 
  std::vector<small_world::io::Student> students;
  std::vector<small_world::io::Section> sections;
  students.reserve(studentIds.size());
  sections.reserve(sectionIds.size());
  // Stores the index the given Student/Section
  std::vector<bool> studentAdded(st, false), sectionAdded(se, false);
  for (std::size_t i = 0; i < n; i++) {
    std::size_t studentId = rowStudents[i], sectionId = rowSections[i];
    if (!studentAdded[studentId]) {
      small_world::io::Student student = make_student(data[i], studentAdj[studentId]);
      students.push_back(std::move(student));
      studentAdded[studentId] = true;
    }
    else if (VERIFY_UNIQUENESS) {
      small_world::io::Student student = make_student(data[i], studentAdj[studentId]);
      assert(student == students[rowStudents[i]]);
    }
    if (!sectionAdded[sectionId]) {
      small_world::io::Section section = make_section(data[i], sectionAdj[sectionId]);
      sections.push_back(make_section(data[i], sectionAdj[sectionId]));
      sectionAdded[sectionId] = true;
    }
    else if (VERIFY_UNIQUENESS) {
      small_world::io::Section section = make_section(data[i], sectionAdj[sectionId]);
      assert(section == sections[rowSections[i]]);
    }
  }

  std::cout << "Student/Section objects generated" << std::endl;

  // Set shared_pointer fields
  this->students = std::make_shared<const std::vector<small_world::io::Student>>(std::move(students));
  this->sections = std::make_shared<const std::vector<small_world::io::Section>>(std::move(sections));
}

CsvEnrollmentDataReader::~CsvEnrollmentDataReader() {}

std::shared_ptr<const std::vector<small_world::io::Student>> CsvEnrollmentDataReader::get_students() const {
  return this->students;
}

std::shared_ptr<const std::vector<small_world::io::Section>> CsvEnrollmentDataReader::get_sections() const {
  return this->sections;
}

// Parsing helper functions
std::vector<std::unordered_map<std::string, std::string>> parse_csv(std::istream& input) {
  constexpr bool PRINT_PROGRESS = true;

  std::vector<std::string> header = parse_row(input);

  std::vector<std::unordered_map<std::string, std::string>> parsed;

  std::vector<std::string> row;
  int rowNum = 0;
  while ((row = parse_row(input)), input) {
    if (PRINT_PROGRESS && ++rowNum % 1000 == 0)
      std::cout << "\rReading line " << std::setw(6) << rowNum << std::flush;
    // All rows should have the same number of commas
    assert(row.size() == header.size());
    // Add a new row
    parsed.emplace_back();
    parsed.back().reserve(row.size());

    for (size_t i = 0; i < row.size(); i++)
      parsed.back().emplace(header[i], row[i]);
  }
  // Clear output
  if (PRINT_PROGRESS)
    std::cout << '\r' << std::string(19, ' ') << '\r' << std::flush;
  return parsed;
}

std::vector<std::string> parse_row(std::istream& input) {
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

// Produces a key for the row consisting of the specified field values separated with $.
std::string get_key(const std::unordered_map<std::string, std::string>& row, const std::vector<std::string>& fields) {
  std::stringstream strm;
  for (const std::string& field : fields)
    strm << row.find(field)->second << '$';
  return strm.str();
}

small_world::io::Student make_student(const std::unordered_map<std::string, std::string>& row, const std::unordered_set<std::size_t>& adj) {
  std::vector<std::size_t> adjVec(adj.begin(), adj.end());
  std::sort(adjVec.begin(), adjVec.end());
  return small_world::io::Student(std::make_shared<const std::vector<std::size_t>>(std::move(adjVec)),
                                  parse<std::size_t>(row, "STRM"),
                                  parse<std::size_t>(row, "UNIQUE_VAL"),
                                  parse<small_world::io::Student::Career>(row, "ACAD_CAREER"),
                                  parse<small_world::io::Student::Program>(row, "ACAD_PROG"),
                                  parse<small_world::io::Student::Organization>(row, "ACAD_ORG"),
                                  parse<small_world::io::Student::Group>(row, "ACAD_GROUP"),
                                  parse<small_world::io::Student::Plan>(row, "ACAD_PLAN"),
                                  parse<small_world::io::Student::Subplan>(row, "ACAD_SUB_PLAN"));
}

small_world::io::Section make_section(const std::unordered_map<std::string, std::string>& row, const std::unordered_set<std::size_t>& adj) {
  std::vector<std::size_t> adjVec(adj.begin(), adj.end());
  std::sort(adjVec.begin(), adjVec.end());
  return small_world::io::Section(std::make_shared<const std::vector<std::size_t>>(std::move(adjVec)),
                                  parse<std::size_t>(row, "STRM"),
                                  parse<std::size_t>(row, "CLASS_NBR"),
                                  parse<std::size_t>(row, "CRSE_ID"),
                                  parse<small_world::io::Section::Session>(row, "SESSION_CODE"),
                                  parse<Date>(row, "SESS_BEGIN_DT"),
                                  parse<small_world::io::Section::Subject>(row, "CLASS_SUBJECT"),
                                  parse<std::string>(row, "CATALOG_NBR"),
                                  parse<std::string>(row, "CLASS_SECTION"),
                                  parse<small_world::io::Section::Group>(row, "CLASS_ACAD_GROUP"),
                                  parse<small_world::io::Section::Organization>(row, "CLASS_ACAD_ORG"),
                                  parse<std::size_t>(row, "UNT_TAKEN"),
                                  parse<small_world::io::Section::Component>(row, "SSR_COMPONENT"),
                                  parse<Date>(row, "MTG_PAT_START_DT"),
                                  parse<Date>(row, "MTG_PAT_END_DT"),
                                  parse<small_world::io::Section::Facility>(row, "FACILITY_ID"),
                                  parse<std::size_t>(row, "ROOM_CAPACITY"),
                                  parse<Time>(row, "MEETING_TIME_START"),
                                  parse<Time>(row, "MEETING_TIME_END"),
                                  parse<bool>(row, "MON"),
                                  parse<bool>(row, "TUES"),
                                  parse<bool>(row, "WED"),
                                  parse<bool>(row, "THURS"),
                                  parse<bool>(row, "FRI"),
                                  parse<bool>(row, "SAT"),
                                  parse<bool>(row, "SUN"),
                                  parse<small_world::io::Section::Mode>(row, "INSTRUCTION_MODE"));
}

template<typename T>
auto parse(const std::unordered_map<std::string, std::string>& row, const std::string& field) -> decltype(Parse<T>::parse(row.find(field)->second)) {
  auto it = row.find(field);
  assert(it != row.end());
  return Parse<T>::parse(it->second);
}

template<typename T>
T Parse<T>::parse(const std::string& str) {
  std::stringstream strm(str);
  T val;
  strm >> val;
  return val;
}

const std::string& Parse<std::string>::parse(const std::string& str) {
    return str;
}

bool Parse<bool>::parse(const std::string& str) {
  if (str == "Y")
    return true;
  assert(str == "N");
  return false;
}

small_world::io::Student::Career Parse<small_world::io::Student::Career>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Student::Career> MAP {
    {"GRAD", small_world::io::Student::Career::GRAD},
    {"UGRD", small_world::io::Student::Career::UGRD}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
  }

small_world::io::Student::Program Parse<small_world::io::Student::Program>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Student::Program> MAP {
    {"CERT", small_world::io::Student::Program::CERT},
    {"DOCT", small_world::io::Student::Program::DOCT},
    {"GRAD", small_world::io::Student::Program::GRAD},
    {"MASTR", small_world::io::Student::Program::MASTR},
    {"SPPRO", small_world::io::Student::Program::SPPRO},
    {"UCERT", small_world::io::Student::Program::UCERT},
    {"UGNDS", small_world::io::Student::Program::UGNDS},
    {"UGRD", small_world::io::Student::Program::UGRD},
    {"UGRD2", small_world::io::Student::Program::UGRD2}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Student::Organization Parse<small_world::io::Student::Organization>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Student::Organization> MAP {
    {"ARHM", small_world::io::Student::Organization::ARHM},
    {"ATEC", small_world::io::Student::Organization::ATEC},
    {"BBSC", small_world::io::Student::Organization::BBSC},
    {"EMGT", small_world::io::Student::Organization::EMGT},
    {"ENCS", small_world::io::Student::Organization::ENCS},
    {"ENCSBIEN", small_world::io::Student::Organization::ENCSBIEN},
    {"ENCSMECH", small_world::io::Student::Organization::ENCSMECH},
    {"ENCSSYSM", small_world::io::Student::Organization::ENCSSYSM},
    {"EPPS", small_world::io::Student::Organization::EPPS},
    {"GENS", small_world::io::Student::Organization::GENS},
    {"GRAD", small_world::io::Student::Organization::GRAD},
    {"MGMT", small_world::io::Student::Organization::MGMT},
    {"NSMT", small_world::io::Student::Organization::NSMT},
    {"NSMTBIOL", small_world::io::Student::Organization::NSMTBIOL},
    {"NSMTCHEM", small_world::io::Student::Organization::NSMTCHEM},
    {"NSMTMATH", small_world::io::Student::Organization::NSMTMATH},
    {"UGRD", small_world::io::Student::Organization::UGRD}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Student::Group Parse<small_world::io::Student::Group>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Student::Group> MAP {
    {"ARHM", small_world::io::Student::Group::ARHM},
    {"ATEC", small_world::io::Student::Group::ATEC},
    {"BBSC", small_world::io::Student::Group::BBSC},
    {"EMGT", small_world::io::Student::Group::EMGT},
    {"ENCS", small_world::io::Student::Group::ENCS},
    {"EPPS", small_world::io::Student::Group::EPPS},
    {"GENS", small_world::io::Student::Group::GENS},
    {"GRAD", small_world::io::Student::Group::GRAD},
    {"MGMT", small_world::io::Student::Group::MGMT},
    {"NSMT", small_world::io::Student::Group::NSMT},
    {"UGRD", small_world::io::Student::Group::UGRD}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Student::Plan Parse<small_world::io::Student::Plan>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Student::Plan> MAP {
    {"ACCTBS", small_world::io::Student::Plan::ACCTBS},
    {"ACCTMS", small_world::io::Student::Plan::ACCTMS},
    {"ACCTMSNF", small_world::io::Student::Plan::ACCTMSNF},
    {"ACCTND", small_world::io::Student::Plan::ACCTND},
    {"ACMBMBMS", small_world::io::Student::Plan::ACMBMBMS},
    {"ACN_MS", small_world::io::Student::Plan::ACN_MS},
    {"ACTSBS", small_world::io::Student::Plan::ACTSBS},
    {"ACTSMS", small_world::io::Student::Plan::ACTSMS},
    {"AHSTMA", small_world::io::Student::Plan::AHSTMA},
    {"AMS_BA", small_world::io::Student::Plan::AMS_BA},
    {"ARHMND", small_world::io::Student::Plan::ARHMND},
    {"ASOCMS", small_world::io::Student::Plan::ASOCMS},
    {"ATCMBA", small_world::io::Student::Plan::ATCMBA},
    {"ATCMDR", small_world::io::Student::Plan::ATCMDR},
    {"ATCMMA", small_world::io::Student::Plan::ATCMMA},
    {"ATCMMFA", small_world::io::Student::Plan::ATCMMFA},
    {"ATECBA", small_world::io::Student::Plan::ATECBA},
    {"ATECMFA", small_world::io::Student::Plan::ATECMFA},
    {"AUD_AUD", small_world::io::Student::Plan::AUD_AUD},
    {"BA__BS", small_world::io::Student::Plan::BA__BS},
    {"BCBMMS", small_world::io::Student::Plan::BCBMMS},
    {"BCHMBS", small_world::io::Student::Plan::BCHMBS},
    {"BCM_DR", small_world::io::Student::Plan::BCM_DR},
    {"BCM_MS", small_world::io::Student::Plan::BCM_MS},
    {"BIDMCERT", small_world::io::Student::Plan::BIDMCERT},
    {"BIO_BA", small_world::io::Student::Plan::BIO_BA},
    {"BIO_BS", small_world::io::Student::Plan::BIO_BS},
    {"BIO_ND", small_world::io::Student::Plan::BIO_ND},
    {"BIOTMS", small_world::io::Student::Plan::BIOTMS},
    {"BIOTND", small_world::io::Student::Plan::BIOTND},
    {"BLBABS", small_world::io::Student::Plan::BLBABS},
    {"BLCJBA", small_world::io::Student::Plan::BLCJBA},
    {"BLHCBS", small_world::io::Student::Plan::BLHCBS},
    {"BMEDCERT", small_world::io::Student::Plan::BMEDCERT},
    {"BMENBS", small_world::io::Student::Plan::BMENBS},
    {"BMENDR", small_world::io::Student::Plan::BMENDR},
    {"BMENMS", small_world::io::Student::Plan::BMENMS},
    {"BMENND", small_world::io::Student::Plan::BMENND},
    {"BSANMS", small_world::io::Student::Plan::BSANMS},
    {"BSANMSNF", small_world::io::Student::Plan::BSANMSNF},
    {"BSMBMBMS", small_world::io::Student::Plan::BSMBMBMS},
    {"CE__BS", small_world::io::Student::Plan::CE__BS},
    {"CE__DR", small_world::io::Student::Plan::CE__DR},
    {"CE__MS", small_world::io::Student::Plan::CE__MS},
    {"CGNSDR", small_world::io::Student::Plan::CGNSDR},
    {"CGS_BS", small_world::io::Student::Plan::CGS_BS},
    {"CHEMBA", small_world::io::Student::Plan::CHEMBA},
    {"CHEMBS", small_world::io::Student::Plan::CHEMBS},
    {"CHEMDR", small_world::io::Student::Plan::CHEMDR},
    {"CHEMMS", small_world::io::Student::Plan::CHEMMS},
    {"CJBLBA", small_world::io::Student::Plan::CJBLBA},
    {"CLDPBS", small_world::io::Student::Plan::CLDPBS},
    {"CMSDDR", small_world::io::Student::Plan::CMSDDR},
    {"COMDMS", small_world::io::Student::Plan::COMDMS},
    {"CRIMBA", small_world::io::Student::Plan::CRIMBA},
    {"CRIMDR", small_world::io::Student::Plan::CRIMDR},
    {"CRIMMS", small_world::io::Student::Plan::CRIMMS},
    {"CS__BS", small_world::io::Student::Plan::CS__BS},
    {"CS__DR", small_world::io::Student::Plan::CS__DR},
    {"CS__MSCS", small_world::io::Student::Plan::CS__MSCS},
    {"CS__ND", small_world::io::Student::Plan::CS__ND},
    {"CYSYCERT", small_world::io::Student::Plan::CYSYCERT},
    {"DSC_BS", small_world::io::Student::Plan::DSC_BS},
    {"DTSCCERT", small_world::io::Student::Plan::DTSCCERT},
    {"ECFNBS", small_world::io::Student::Plan::ECFNBS},
    {"ECONBA", small_world::io::Student::Plan::ECONBA},
    {"ECONBS", small_world::io::Student::Plan::ECONBS},
    {"ECONDR", small_world::io::Student::Plan::ECONDR},
    {"ECONMS", small_world::io::Student::Plan::ECONMS},
    {"ECONND", small_world::io::Student::Plan::ECONND},
    {"EE__BSEE", small_world::io::Student::Plan::EE__BSEE},
    {"EE__DR", small_world::io::Student::Plan::EE__DR},
    {"EE__MSEE", small_world::io::Student::Plan::EE__MSEE},
    {"EE__ND", small_world::io::Student::Plan::EE__ND},
    {"EMACBA", small_world::io::Student::Plan::EMACBA},
    {"EMBAMBA", small_world::io::Student::Plan::EMBAMBA},
    {"EMGTNDNF", small_world::io::Student::Plan::EMGTNDNF},
    {"ENMGMS", small_world::io::Student::Plan::ENMGMS},
    {"ENSYCERT", small_world::io::Student::Plan::ENSYCERT},
    {"FERMMSNF", small_world::io::Student::Plan::FERMMSNF},
    {"FIMBMBMS", small_world::io::Student::Plan::FIMBMBMS},
    {"FIN_BS", small_world::io::Student::Plan::FIN_BS},
    {"FIN_MS", small_world::io::Student::Plan::FIN_MS},
    {"FIN_MSNF", small_world::io::Student::Plan::FIN_MSNF},
    {"FNECBS", small_world::io::Student::Plan::FNECBS},
    {"GEINCERT", small_world::io::Student::Plan::GEINCERT},
    {"GEOSBS", small_world::io::Student::Plan::GEOSBS},
    {"GEOSDR", small_world::io::Student::Plan::GEOSDR},
    {"GEOSMS", small_world::io::Student::Plan::GEOSMS},
    {"GEOSND", small_world::io::Student::Plan::GEOSND},
    {"GIS_BS", small_world::io::Student::Plan::GIS_BS},
    {"GISCERT", small_world::io::Student::Plan::GISCERT},
    {"GLBSBS", small_world::io::Student::Plan::GLBSBS},
    {"GLHRBS", small_world::io::Student::Plan::GLHRBS},
    {"GLIPBS", small_world::io::Student::Plan::GLIPBS},
    {"GLMKBS", small_world::io::Student::Plan::GLMKBS},
    {"GLSCBS", small_world::io::Student::Plan::GLSCBS},
    {"GRADND", small_world::io::Student::Plan::GRADND},
    {"GRU_ND", small_world::io::Student::Plan::GRU_ND},
    {"GSISDR", small_world::io::Student::Plan::GSISDR},
    {"GSISMS", small_world::io::Student::Plan::GSISMS},
    {"GSISND", small_world::io::Student::Plan::GSISND},
    {"HCBLBS", small_world::io::Student::Plan::HCBLBS},
    {"HCLMMS", small_world::io::Student::Plan::HCLMMS},
    {"HCLMMSNF", small_world::io::Student::Plan::HCLMMSNF},
    {"HCLMND", small_world::io::Student::Plan::HCLMND},
    {"HCLMNDNF", small_world::io::Student::Plan::HCLMNDNF},
    {"HCMBBS", small_world::io::Student::Plan::HCMBBS},
    {"HCMGBS", small_world::io::Student::Plan::HCMGBS},
    {"HCMGNDNF", small_world::io::Student::Plan::HCMGNDNF},
    {"HCSTCERT", small_world::io::Student::Plan::HCSTCERT},
    {"HDECMS", small_world::io::Student::Plan::HDECMS},
    {"HI__DR", small_world::io::Student::Plan::HI__DR},
    {"HI__MA", small_world::io::Student::Plan::HI__MA},
    {"HISTBA", small_world::io::Student::Plan::HISTBA},
    {"HISTMA", small_world::io::Student::Plan::HISTMA},
    {"HITHCERT", small_world::io::Student::Plan::HITHCERT},
    {"HLMBMBMS", small_world::io::Student::Plan::HLMBMBMS},
    {"HLTHBS", small_world::io::Student::Plan::HLTHBS},
    {"HRGLBS", small_world::io::Student::Plan::HRGLBS},
    {"HRM_BS", small_world::io::Student::Plan::HRM_BS},
    {"HS__BA", small_world::io::Student::Plan::HS__BA},
    {"HUASDR", small_world::io::Student::Plan::HUASDR},
    {"HUASMA", small_world::io::Student::Plan::HUASMA},
    {"HUHIDR", small_world::io::Student::Plan::HUHIDR},
    {"HUMADR", small_world::io::Student::Plan::HUMADR},
    {"HUMAMA", small_world::io::Student::Plan::HUMAMA},
    {"HUMAND", small_world::io::Student::Plan::HUMAND},
    {"HUSLDR", small_world::io::Student::Plan::HUSLDR},
    {"HUSLMA", small_world::io::Student::Plan::HUSLMA},
    {"HUSLND", small_world::io::Student::Plan::HUSLND},
    {"IEMBMBMS", small_world::io::Student::Plan::IEMBMBMS},
    {"ILMBMBMS", small_world::io::Student::Plan::ILMBMBMS},
    {"IMS_DR", small_world::io::Student::Plan::IMS_DR},
    {"IMS_MS", small_world::io::Student::Plan::IMS_MS},
    {"IMS_ND", small_world::io::Student::Plan::IMS_ND},
    {"INASCERT", small_world::io::Student::Plan::INASCERT},
    {"INENMS", small_world::io::Student::Plan::INENMS},
    {"INTSBS", small_world::io::Student::Plan::INTSBS},
    {"IPECBA", small_world::io::Student::Plan::IPECBA},
    {"IPECBS", small_world::io::Student::Plan::IPECBS},
    {"IPECMS", small_world::io::Student::Plan::IPECMS},
    {"IPGLBS", small_world::io::Student::Plan::IPGLBS},
    {"IS__BA", small_world::io::Student::Plan::IS__BA},
    {"IS__BS", small_world::io::Student::Plan::IS__BS},
    {"IS__MA", small_world::io::Student::Plan::IS__MA},
    {"ITMBMBMS", small_world::io::Student::Plan::ITMBMBMS},
    {"LATSMA", small_world::io::Student::Plan::LATSMA},
    {"LGMCERT", small_world::io::Student::Plan::LGMCERT},
    {"LIT_BA", small_world::io::Student::Plan::LIT_BA},
    {"LITRBA", small_world::io::Student::Plan::LITRBA},
    {"LITRDR", small_world::io::Student::Plan::LITRDR},
    {"LITRMA", small_world::io::Student::Plan::LITRMA},
    {"LITRND", small_world::io::Student::Plan::LITRND},
    {"LOD_MSNF", small_world::io::Student::Plan::LOD_MSNF},
    {"MAMBMBMS", small_world::io::Student::Plan::MAMBMBMS},
    {"MAS_MS", small_world::io::Student::Plan::MAS_MS},
    {"MAS_MSNF", small_world::io::Student::Plan::MAS_MSNF},
    {"MAS_ND", small_world::io::Student::Plan::MAS_ND},
    {"MATHBA", small_world::io::Student::Plan::MATHBA},
    {"MATHBS", small_world::io::Student::Plan::MATHBS},
    {"MATHDR", small_world::io::Student::Plan::MATHDR},
    {"MATHMS", small_world::io::Student::Plan::MATHMS},
    {"MATHND", small_world::io::Student::Plan::MATHND},
    {"MB__BS", small_world::io::Student::Plan::MB__BS},
    {"MBA_MBA", small_world::io::Student::Plan::MBA_MBA},
    {"MBA_MBANF", small_world::io::Student::Plan::MBA_MBANF},
    {"MBA_ND", small_world::io::Student::Plan::MBA_ND},
    {"MBHCBS", small_world::io::Student::Plan::MBHCBS},
    {"MECHBS", small_world::io::Student::Plan::MECHBS},
    {"MECHDR", small_world::io::Student::Plan::MECHDR},
    {"MECHMS", small_world::io::Student::Plan::MECHMS},
    {"MED_MAT", small_world::io::Student::Plan::MED_MAT},
    {"MITMMS", small_world::io::Student::Plan::MITMMS},
    {"MKGLBS", small_world::io::Student::Plan::MKGLBS},
    {"MKMBMBMS", small_world::io::Student::Plan::MKMBMBMS},
    {"MPA_MPA", small_world::io::Student::Plan::MPA_MPA},
    {"MPA_ND", small_world::io::Student::Plan::MPA_ND},
    {"MPP_MPP", small_world::io::Student::Plan::MPP_MPP},
    {"MRKTBS", small_world::io::Student::Plan::MRKTBS},
    {"MRKTMS", small_world::io::Student::Plan::MRKTMS},
    {"MSC_DR", small_world::io::Student::Plan::MSC_DR},
    {"MSENDR", small_world::io::Student::Plan::MSENDR},
    {"MSENMS", small_world::io::Student::Plan::MSENMS},
    {"NPMCERT", small_world::io::Student::Plan::NPMCERT},
    {"NSC_BS", small_world::io::Student::Plan::NSC_BS},
    {"NWVECERT", small_world::io::Student::Plan::NWVECERT},
    {"ORCNCERT", small_world::io::Student::Plan::ORCNCERT},
    {"PAFFBS", small_world::io::Student::Plan::PAFFBS},
    {"PAFFDR", small_world::io::Student::Plan::PAFFDR},
    {"PHILBA", small_world::io::Student::Plan::PHILBA},
    {"PHYSBA", small_world::io::Student::Plan::PHYSBA},
    {"PHYSBS", small_world::io::Student::Plan::PHYSBS},
    {"PHYSDR", small_world::io::Student::Plan::PHYSDR},
    {"PHYSMS", small_world::io::Student::Plan::PHYSMS},
    {"PHYSND", small_world::io::Student::Plan::PHYSND},
    {"PPOLBS", small_world::io::Student::Plan::PPOLBS},
    {"PPPEDR", small_world::io::Student::Plan::PPPEDR},
    {"PRCOCERT", small_world::io::Student::Plan::PRCOCERT},
    {"PSCIBA", small_world::io::Student::Plan::PSCIBA},
    {"PSCIDR", small_world::io::Student::Plan::PSCIDR},
    {"PSCIMA", small_world::io::Student::Plan::PSCIMA},
    {"PSCLMA", small_world::io::Student::Plan::PSCLMA},
    {"PSLSMA", small_world::io::Student::Plan::PSLSMA},
    {"PSY_BS", small_world::io::Student::Plan::PSY_BS},
    {"PSYSDR", small_world::io::Student::Plan::PSYSDR},
    {"PSYSMS", small_world::io::Student::Plan::PSYSMS},
    {"SCE_MAT", small_world::io::Student::Plan::SCE_MAT},
    {"SCE_ND", small_world::io::Student::Plan::SCE_ND},
    {"SCGLBS", small_world::io::Student::Plan::SCGLBS},
    {"SCMBMBMS", small_world::io::Student::Plan::SCMBMBMS},
    {"SCMTBS", small_world::io::Student::Plan::SCMTBS},
    {"SCMTMS", small_world::io::Student::Plan::SCMTMS},
    {"SCMTMSNF", small_world::io::Student::Plan::SCMTMSNF},
    {"SCMTND", small_world::io::Student::Plan::SCMTND},
    {"SDARMS", small_world::io::Student::Plan::SDARMS},
    {"SE__BS", small_world::io::Student::Plan::SE__BS},
    {"SE__DR", small_world::io::Student::Plan::SE__DR},
    {"SE__MS", small_world::io::Student::Plan::SE__MS},
    {"SE__MSNF", small_world::io::Student::Plan::SE__MSNF},
    {"SE__ND", small_world::io::Student::Plan::SE__ND},
    {"SHRMCERT", small_world::io::Student::Plan::SHRMCERT},
    {"SOC_BA", small_world::io::Student::Plan::SOC_BA},
    {"SPAUBS", small_world::io::Student::Plan::SPAUBS},
    {"STATDR", small_world::io::Student::Plan::STATDR},
    {"STATMS", small_world::io::Student::Plan::STATMS},
    {"SYENCERT", small_world::io::Student::Plan::SYENCERT},
    {"SYSMMS", small_world::io::Student::Plan::SYSMMS},
    {"SYSMMSNF", small_world::io::Student::Plan::SYSMMSNF},
    {"TE__DR", small_world::io::Student::Plan::TE__DR},
    {"TE__MSTE", small_world::io::Student::Plan::TE__MSTE},
    {"TED_ND", small_world::io::Student::Plan::TED_ND},
    {"UGS_ND", small_world::io::Student::Plan::UGS_ND},
    {"UND_BA", small_world::io::Student::Plan::UND_BA},
    {"UND_BS", small_world::io::Student::Plan::UND_BS},
    {"VPA_BA", small_world::io::Student::Plan::VPA_BA},
    {"VPA_DR", small_world::io::Student::Plan::VPA_DR},
    {"VPA_MA", small_world::io::Student::Plan::VPA_MA},
    {"VPA_ND", small_world::io::Student::Plan::VPA_ND}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Student::Subplan Parse<small_world::io::Student::Subplan>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Student::Subplan> MAP {
    {"", small_world::io::Student::Subplan::NONE},
    {"ACCT", small_world::io::Student::Subplan::ACCT},
    {"ACCTO", small_world::io::Student::Subplan::ACCTO},
    {"AICMP_MDL", small_world::io::Student::Subplan::AICMP_MDL},
    {"AM_ECN_SYS", small_world::io::Student::Subplan::AM_ECN_SYS},
    {"AM_LGL_SYS", small_world::io::Student::Subplan::AM_LGL_SYS},
    {"AM_WLD_COM", small_world::io::Student::Subplan::AM_WLD_COM},
    {"AMERICA_ST", small_world::io::Student::Subplan::AMERICA_ST},
    {"AMME", small_world::io::Student::Subplan::AMME},
    {"ANIMATION", small_world::io::Student::Subplan::ANIMATION},
    {"APPLD_MATH", small_world::io::Student::Subplan::APPLD_MATH},
    {"APPLD_STAT", small_world::io::Student::Subplan::APPLD_STAT},
    {"ARHM_FND", small_world::io::Student::Subplan::ARHM_FND},
    {"ART_HIST", small_world::io::Student::Subplan::ART_HIST},
    {"ARTADMNMGT", small_world::io::Student::Subplan::ARTADMNMGT},
    {"ATEC_GEN", small_world::io::Student::Subplan::ATEC_GEN},
    {"BBSC_FND", small_world::io::Student::Subplan::BBSC_FND},
    {"BSANO", small_world::io::Student::Subplan::BSANO},
    {"BUS_AN", small_world::io::Student::Subplan::BUS_AN},
    {"BUS_COMM", small_world::io::Student::Subplan::BUS_COMM},
    {"BUS_ECON", small_world::io::Student::Subplan::BUS_ECON},
    {"BUS_ISSUES", small_world::io::Student::Subplan::BUS_ISSUES},
    {"BUSINESS", small_world::io::Student::Subplan::BUSINESS},
    {"CIRCUITS", small_world::io::Student::Subplan::CIRCUITS},
    {"COACHING", small_world::io::Student::Subplan::COACHING},
    {"COGN_NEURO", small_world::io::Student::Subplan::COGN_NEURO},
    {"COHORT", small_world::io::Student::Subplan::COHORT},
    {"COMM", small_world::io::Student::Subplan::COMM},
    {"COMP_SYS", small_world::io::Student::Subplan::COMP_SYS},
    {"COUNSELING", small_world::io::Student::Subplan::COUNSELING},
    {"CPA", small_world::io::Student::Subplan::CPA},
    {"CR_MED_ST", small_world::io::Student::Subplan::CR_MED_ST},
    {"CRTV_PRCT", small_world::io::Student::Subplan::CRTV_PRCT},
    {"DANCE", small_world::io::Student::Subplan::DANCE},
    {"DATA_SCI", small_world::io::Student::Subplan::DATA_SCI},
    {"DESIG_PROD", small_world::io::Student::Subplan::DESIG_PROD},
    {"DEVICES", small_world::io::Student::Subplan::DEVICES},
    {"DYNSYSTCON", small_world::io::Student::Subplan::DYNSYSTCON},
    {"EDUC_FND", small_world::io::Student::Subplan::EDUC_FND},
    {"ENCS_FND", small_world::io::Student::Subplan::ENCS_FND},
    {"ENERGY_MGT", small_world::io::Student::Subplan::ENERGY_MGT},
    {"ENVR_ST", small_world::io::Student::Subplan::ENVR_ST},
    {"EPPS_FND", small_world::io::Student::Subplan::EPPS_FND},
    {"FILM", small_world::io::Student::Subplan::FILM},
    {"FIN", small_world::io::Student::Subplan::FIN},
    {"FTMBA", small_world::io::Student::Subplan::FTMBA},
    {"GAME_DEV", small_world::io::Student::Subplan::GAME_DEV},
    {"GAME_ST", small_world::io::Student::Subplan::GAME_ST},
    {"GAMES", small_world::io::Student::Subplan::GAMES},
    {"GAMING", small_world::io::Student::Subplan::GAMING},
    {"GEN_BUS", small_world::io::Student::Subplan::GEN_BUS},
    {"GEN_CON", small_world::io::Student::Subplan::GEN_CON},
    {"GEN_GLB", small_world::io::Student::Subplan::GEN_GLB},
    {"GEN_MATH", small_world::io::Student::Subplan::GEN_MATH},
    {"GEN_STAT", small_world::io::Student::Subplan::GEN_STAT},
    {"GENDER_ST", small_world::io::Student::Subplan::GENDER_ST},
    {"GENERAL", small_world::io::Student::Subplan::GENERAL},
    {"GLEMBA", small_world::io::Student::Subplan::GLEMBA},
    {"HCMPROF", small_world::io::Student::Subplan::HCMPROF},
    {"HEALTHCARE", small_world::io::Student::Subplan::HEALTHCARE},
    {"HIST_GEN", small_world::io::Student::Subplan::HIST_GEN},
    {"HLTH_WELL", small_world::io::Student::Subplan::HLTH_WELL},
    {"HOL_LDRSHP", small_world::io::Student::Subplan::HOL_LDRSHP},
    {"HOL_MGMT", small_world::io::Student::Subplan::HOL_MGMT},
    {"HR", small_world::io::Student::Subplan::HR},
    {"HS_GEN", small_world::io::Student::Subplan::HS_GEN},
    {"INFOSYS", small_world::io::Student::Subplan::INFOSYS},
    {"INNOV_ENTR", small_world::io::Student::Subplan::INNOV_ENTR},
    {"INSURANCE", small_world::io::Student::Subplan::INSURANCE},
    {"INT_ARTS", small_world::io::Student::Subplan::INT_ARTS},
    {"INT_POL_EC", small_world::io::Student::Subplan::INT_POL_EC},
    {"INTL_RELAT", small_world::io::Student::Subplan::INTL_RELAT},
    {"IS_FND", small_world::io::Student::Subplan::IS_FND},
    {"LAW", small_world::io::Student::Subplan::LAW},
    {"LAW_LGL_CL", small_world::io::Student::Subplan::LAW_LGL_CL},
    {"LAWCOURTS", small_world::io::Student::Subplan::LAWCOURTS},
    {"LEGIS_ST", small_world::io::Student::Subplan::LEGIS_ST},
    {"MANAGEMENT", small_world::io::Student::Subplan::MANAGEMENT},
    {"MANUDESIN", small_world::io::Student::Subplan::MANUDESIN},
    {"MECH_MAT", small_world::io::Student::Subplan::MECH_MAT},
    {"MEDIA_COMM", small_world::io::Student::Subplan::MEDIA_COMM},
    {"MGMT_FND", small_world::io::Student::Subplan::MGMT_FND},
    {"MGTINFOSYS", small_world::io::Student::Subplan::MGTINFOSYS},
    {"MRKT", small_world::io::Student::Subplan::MRKT},
    {"MUSIC", small_world::io::Student::Subplan::MUSIC},
    {"NGT_MDT", small_world::io::Student::Subplan::NGT_MDT},
    {"NSMT_FND", small_world::io::Student::Subplan::NSMT_FND},
    {"NTWKD_CL", small_world::io::Student::Subplan::NTWKD_CL},
    {"ONLINE", small_world::io::Student::Subplan::ONLINE},
    {"OPMGT", small_world::io::Student::Subplan::OPMGT},
    {"ORG_BEHAVR", small_world::io::Student::Subplan::ORG_BEHAVR},
    {"ORG_CNSLT", small_world::io::Student::Subplan::ORG_CNSLT},
    {"PARKLAND", small_world::io::Student::Subplan::PARKLAND},
    {"PHILOSOPHY", small_world::io::Student::Subplan::PHILOSOPHY},
    {"PHT_VID_DG", small_world::io::Student::Subplan::PHT_VID_DG},
    {"PMBAE", small_world::io::Student::Subplan::PMBAE},
    {"PMBAH", small_world::io::Student::Subplan::PMBAH},
    {"PMBAO", small_world::io::Student::Subplan::PMBAO},
    {"PMBAX", small_world::io::Student::Subplan::PMBAX},
    {"PRJMGMT", small_world::io::Student::Subplan::PRJMGMT},
    {"PSYHCI", small_world::io::Student::Subplan::PSYHCI},
    {"PUBLIC_REL", small_world::io::Student::Subplan::PUBLIC_REL},
    {"PWRELECEN", small_world::io::Student::Subplan::PWRELECEN},
    {"REAL_EST", small_world::io::Student::Subplan::REAL_EST},
    {"RISK_INS", small_world::io::Student::Subplan::RISK_INS},
    {"SALES", small_world::io::Student::Subplan::SALES},
    {"SCM", small_world::io::Student::Subplan::SCM},
    {"SIGN_SYST", small_world::io::Student::Subplan::SIGN_SYST},
    {"SOC_ISSUES", small_world::io::Student::Subplan::SOC_ISSUES},
    {"STATISTICS", small_world::io::Student::Subplan::STATISTICS},
    {"STRG_HR", small_world::io::Student::Subplan::STRG_HR},
    {"TEACHCERT", small_world::io::Student::Subplan::TEACHCERT},
    {"TEACHISSUE", small_world::io::Student::Subplan::TEACHISSUE},
    {"THEATRE", small_world::io::Student::Subplan::THEATRE},
    {"THERMFLUID", small_world::io::Student::Subplan::THERMFLUID},
    {"TRNSF_LDR", small_world::io::Student::Subplan::TRNSF_LDR},
    {"URBAN_ST", small_world::io::Student::Subplan::URBAN_ST},
    {"US_WORLD", small_world::io::Student::Subplan::US_WORLD},
    {"UTSW", small_world::io::Student::Subplan::UTSW},
    {"VISUAL_ART", small_world::io::Student::Subplan::VISUAL_ART},
    {"WOMEN_ST", small_world::io::Student::Subplan::WOMEN_ST}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Section::Session Parse<small_world::io::Section::Session>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Section::Session> MAP {
    {"1", small_world::io::Section::Session::_1},
    {"8W1", small_world::io::Section::Session::_8W1},
    {"8W2", small_world::io::Section::Session::_8W2},
    {"AMN", small_world::io::Section::Session::AMN},
    {"BMN", small_world::io::Section::Session::BMN},
    {"CMN", small_world::io::Section::Session::CMN},
    {"DMN", small_world::io::Section::Session::DMN},
    {"FMN", small_world::io::Section::Session::FMN},
    {"Z01", small_world::io::Section::Session::Z01},
    {"Z02", small_world::io::Section::Session::Z02},
    {"Z03", small_world::io::Section::Session::Z03},
    {"Z04", small_world::io::Section::Session::Z04},
    {"Z05", small_world::io::Section::Session::Z05},
    {"Z06", small_world::io::Section::Session::Z06},
    {"Z07", small_world::io::Section::Session::Z07},
    {"Z08", small_world::io::Section::Session::Z08},
    {"Z09", small_world::io::Section::Session::Z09},
    {"Z10", small_world::io::Section::Session::Z10},
    {"Z11", small_world::io::Section::Session::Z11},
    {"Z12", small_world::io::Section::Session::Z12},
    {"Z13", small_world::io::Section::Session::Z13},
    {"Z14", small_world::io::Section::Session::Z14},
    {"Z15", small_world::io::Section::Session::Z15},
    {"Z16", small_world::io::Section::Session::Z16},
    {"Z17", small_world::io::Section::Session::Z17}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Section::Subject Parse<small_world::io::Section::Subject>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Section::Subject> MAP {
    {"ACCT", small_world::io::Section::Subject::ACCT},
    {"ACN", small_world::io::Section::Subject::ACN},
    {"ACTS", small_world::io::Section::Subject::ACTS},
    {"AERO", small_world::io::Section::Subject::AERO},
    {"AHST", small_world::io::Section::Subject::AHST},
    {"AMS", small_world::io::Section::Subject::AMS},
    {"ARAB", small_world::io::Section::Subject::ARAB},
    {"ARHM", small_world::io::Section::Subject::ARHM},
    {"ARTS", small_world::io::Section::Subject::ARTS},
    {"ATCM", small_world::io::Section::Subject::ATCM},
    {"AUD", small_world::io::Section::Subject::AUD},
    {"BA", small_world::io::Section::Subject::BA},
    {"BBSU", small_world::io::Section::Subject::BBSU},
    {"BCOM", small_world::io::Section::Subject::BCOM},
    {"BIOL", small_world::io::Section::Subject::BIOL},
    {"BIS", small_world::io::Section::Subject::BIS},
    {"BLAW", small_world::io::Section::Subject::BLAW},
    {"BMEN", small_world::io::Section::Subject::BMEN},
    {"BPS", small_world::io::Section::Subject::BPS},
    {"BUAN", small_world::io::Section::Subject::BUAN},
    {"CE", small_world::io::Section::Subject::CE},
    {"CGS", small_world::io::Section::Subject::CGS},
    {"CHEM", small_world::io::Section::Subject::CHEM},
    {"CHIN", small_world::io::Section::Subject::CHIN},
    {"CLDP", small_world::io::Section::Subject::CLDP},
    {"COMD", small_world::io::Section::Subject::COMD},
    {"COMM", small_world::io::Section::Subject::COMM},
    {"CRIM", small_world::io::Section::Subject::CRIM},
    {"CRWT", small_world::io::Section::Subject::CRWT},
    {"CS", small_world::io::Section::Subject::CS},
    {"DANC", small_world::io::Section::Subject::DANC},
    {"ECON", small_world::io::Section::Subject::ECON},
    {"ECS", small_world::io::Section::Subject::ECS},
    {"ECSC", small_world::io::Section::Subject::ECSC},
    {"ED", small_world::io::Section::Subject::ED},
    {"EE", small_world::io::Section::Subject::EE},
    {"EEBM", small_world::io::Section::Subject::EEBM},
    {"EECS", small_world::io::Section::Subject::EECS},
    {"EECT", small_world::io::Section::Subject::EECT},
    {"EEDG", small_world::io::Section::Subject::EEDG},
    {"EEGR", small_world::io::Section::Subject::EEGR},
    {"EEMF", small_world::io::Section::Subject::EEMF},
    {"EEOP", small_world::io::Section::Subject::EEOP},
    {"EEPE", small_world::io::Section::Subject::EEPE},
    {"EERF", small_world::io::Section::Subject::EERF},
    {"EESC", small_world::io::Section::Subject::EESC},
    {"ENGR", small_world::io::Section::Subject::ENGR},
    {"ENGY", small_world::io::Section::Subject::ENGY},
    {"ENTP", small_world::io::Section::Subject::ENTP},
    {"ENVR", small_world::io::Section::Subject::ENVR},
    {"EPCS", small_world::io::Section::Subject::EPCS},
    {"EPPS", small_world::io::Section::Subject::EPPS},
    {"FERM", small_world::io::Section::Subject::FERM},
    {"FILM", small_world::io::Section::Subject::FILM},
    {"FIN", small_world::io::Section::Subject::FIN},
    {"FREN", small_world::io::Section::Subject::FREN},
    {"GEOG", small_world::io::Section::Subject::GEOG},
    {"GEOS", small_world::io::Section::Subject::GEOS},
    {"GERM", small_world::io::Section::Subject::GERM},
    {"GISC", small_world::io::Section::Subject::GISC},
    {"GOVT", small_world::io::Section::Subject::GOVT},
    {"GST", small_world::io::Section::Subject::GST},
    {"HCS", small_world::io::Section::Subject::HCS},
    {"HDCD", small_world::io::Section::Subject::HDCD},
    {"HIST", small_world::io::Section::Subject::HIST},
    {"HLTH", small_world::io::Section::Subject::HLTH},
    {"HMGT", small_world::io::Section::Subject::HMGT},
    {"HONS", small_world::io::Section::Subject::HONS},
    {"HUAS", small_world::io::Section::Subject::HUAS},
    {"HUHI", small_world::io::Section::Subject::HUHI},
    {"HUMA", small_world::io::Section::Subject::HUMA},
    {"HUSL", small_world::io::Section::Subject::HUSL},
    {"IMS", small_world::io::Section::Subject::IMS},
    {"IPEC", small_world::io::Section::Subject::IPEC},
    {"ISAE", small_world::io::Section::Subject::ISAE},
    {"ISAH", small_world::io::Section::Subject::ISAH},
    {"ISIS", small_world::io::Section::Subject::ISIS},
    {"ISNS", small_world::io::Section::Subject::ISNS},
    {"ITSS", small_world::io::Section::Subject::ITSS},
    {"JAPN", small_world::io::Section::Subject::JAPN},
    {"LANG", small_world::io::Section::Subject::LANG},
    {"LATS", small_world::io::Section::Subject::LATS},
    {"LIT", small_world::io::Section::Subject::LIT},
    {"MAIS", small_world::io::Section::Subject::MAIS},
    {"MAS", small_world::io::Section::Subject::MAS},
    {"MATH", small_world::io::Section::Subject::MATH},
    {"MECH", small_world::io::Section::Subject::MECH},
    {"MECO", small_world::io::Section::Subject::MECO},
    {"MILS", small_world::io::Section::Subject::MILS},
    {"MIS", small_world::io::Section::Subject::MIS},
    {"MKT", small_world::io::Section::Subject::MKT},
    {"MSEN", small_world::io::Section::Subject::MSEN},
    {"MTHE", small_world::io::Section::Subject::MTHE},
    {"MUSI", small_world::io::Section::Subject::MUSI},
    {"NATS", small_world::io::Section::Subject::NATS},
    {"NSC", small_world::io::Section::Subject::NSC},
    {"OB", small_world::io::Section::Subject::OB},
    {"OBHR", small_world::io::Section::Subject::OBHR},
    {"OPRE", small_world::io::Section::Subject::OPRE},
    {"PA", small_world::io::Section::Subject::PA},
    {"PHIL", small_world::io::Section::Subject::PHIL},
    {"PHIN", small_world::io::Section::Subject::PHIN},
    {"PHYS", small_world::io::Section::Subject::PHYS},
    {"PPOL", small_world::io::Section::Subject::PPOL},
    {"PPPE", small_world::io::Section::Subject::PPPE},
    {"PSCI", small_world::io::Section::Subject::PSCI},
    {"PSY", small_world::io::Section::Subject::PSY},
    {"PSYC", small_world::io::Section::Subject::PSYC},
    {"REAL", small_world::io::Section::Subject::REAL},
    {"RHET", small_world::io::Section::Subject::RHET},
    {"RMIS", small_world::io::Section::Subject::RMIS},
    {"SCI", small_world::io::Section::Subject::SCI},
    {"SE", small_world::io::Section::Subject::SE},
    {"SMED", small_world::io::Section::Subject::SMED},
    {"SOC", small_world::io::Section::Subject::SOC},
    {"SPAN", small_world::io::Section::Subject::SPAN},
    {"SPAU", small_world::io::Section::Subject::SPAU},
    {"STAT", small_world::io::Section::Subject::STAT},
    {"SYSE", small_world::io::Section::Subject::SYSE},
    {"SYSM", small_world::io::Section::Subject::SYSM},
    {"TE", small_world::io::Section::Subject::TE},
    {"THEA", small_world::io::Section::Subject::THEA},
    {"UNIV", small_world::io::Section::Subject::UNIV},
    {"UTD", small_world::io::Section::Subject::UTD},
    {"UTSW", small_world::io::Section::Subject::UTSW},
    {"VPAS", small_world::io::Section::Subject::VPAS}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Section::Group Parse<small_world::io::Section::Group>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Section::Group> MAP {
    {"9999", small_world::io::Section::Group::_9999},
    {"ARHM", small_world::io::Section::Group::ARHM},
    {"ATEC", small_world::io::Section::Group::ATEC},
    {"BBSC", small_world::io::Section::Group::BBSC},
    {"ENCS", small_world::io::Section::Group::ENCS},
    {"EPPS", small_world::io::Section::Group::EPPS},
    {"GENS", small_world::io::Section::Group::GENS},
    {"HONS", small_world::io::Section::Group::HONS},
    {"MGMT", small_world::io::Section::Group::MGMT},
    {"NSMT", small_world::io::Section::Group::NSMT},
    {"UGRD", small_world::io::Section::Group::UGRD}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Section::Organization Parse<small_world::io::Section::Organization>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Section::Organization> MAP {
    {"ARHM", small_world::io::Section::Organization::ARHM},
    {"ATEC", small_world::io::Section::Organization::ATEC},
    {"BBSC", small_world::io::Section::Organization::BBSC},
    {"ENCS", small_world::io::Section::Organization::ENCS},
    {"ENCSBIEN", small_world::io::Section::Organization::ENCSBIEN},
    {"ENCSCSCI", small_world::io::Section::Organization::ENCSCSCI},
    {"ENCSELEC", small_world::io::Section::Organization::ENCSELEC},
    {"ENCSMECH", small_world::io::Section::Organization::ENCSMECH},
    {"ENCSMSEN", small_world::io::Section::Organization::ENCSMSEN},
    {"ENCSSYSM", small_world::io::Section::Organization::ENCSSYSM},
    {"EPPS", small_world::io::Section::Organization::EPPS},
    {"GENS", small_world::io::Section::Organization::GENS},
    {"GENSOTED", small_world::io::Section::Organization::GENSOTED},
    {"HONS", small_world::io::Section::Organization::HONS},
    {"MGMT", small_world::io::Section::Organization::MGMT},
    {"NSMT", small_world::io::Section::Organization::NSMT},
    {"NSMTBIOL", small_world::io::Section::Organization::NSMTBIOL},
    {"NSMTCHEM", small_world::io::Section::Organization::NSMTCHEM},
    {"NSMTGEOS", small_world::io::Section::Organization::NSMTGEOS},
    {"NSMTMATH", small_world::io::Section::Organization::NSMTMATH},
    {"NSMTPHYS", small_world::io::Section::Organization::NSMTPHYS},
    {"NSMTSCED", small_world::io::Section::Organization::NSMTSCED},
    {"REGS", small_world::io::Section::Organization::REGS},
    {"UGRD", small_world::io::Section::Organization::UGRD},
    {"UTDAL", small_world::io::Section::Organization::UTDAL}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Section::Component Parse<small_world::io::Section::Component>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Section::Component> MAP {
    {"DSR", small_world::io::Section::Component::DSR},
    {"ENS", small_world::io::Section::Component::ENS},
    {"EXM", small_world::io::Section::Component::EXM},
    {"IND", small_world::io::Section::Component::IND},
    {"INT", small_world::io::Section::Component::INT},
    {"LAB", small_world::io::Section::Component::LAB},
    {"LBR", small_world::io::Section::Component::LBR},
    {"LE2", small_world::io::Section::Component::LE2},
    {"LEC", small_world::io::Section::Component::LEC},
    {"LLB", small_world::io::Section::Component::LLB},
    {"LLN", small_world::io::Section::Component::LLN},
    {"MST", small_world::io::Section::Component::MST},
    {"PRA", small_world::io::Section::Component::PRA},
    {"RSC", small_world::io::Section::Component::RSC},
    {"RXM", small_world::io::Section::Component::RXM},
    {"SEM", small_world::io::Section::Component::SEM},
    {"STU", small_world::io::Section::Component::STU}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Section::Facility Parse<small_world::io::Section::Facility>::parse(const std::string& str) {
  static const std::unordered_map<std::string, small_world::io::Section::Facility> MAP {
    {" ", small_world::io::Section::Facility::NONE},
    {"AB 1.308", small_world::io::Section::Facility::AB_1_308},
    {"AB 1.318", small_world::io::Section::Facility::AB_1_318},
    {"AB 1.506", small_world::io::Section::Facility::AB_1_506},
    {"AD 2.216", small_world::io::Section::Facility::AD_2_216},
    {"AD 2.232", small_world::io::Section::Facility::AD_2_232},
    {"AD 2.238", small_world::io::Section::Facility::AD_2_238},
    {"AH1 1.104", small_world::io::Section::Facility::AH1_1_104},
    {"AH1 1.106", small_world::io::Section::Facility::AH1_1_106},
    {"AH1 1.110", small_world::io::Section::Facility::AH1_1_110},
    {"AH2 1.204", small_world::io::Section::Facility::AH2_1_204},
    {"ATC 1.102", small_world::io::Section::Facility::ATC_1_102},
    {"ATC 1.305", small_world::io::Section::Facility::ATC_1_305},
    {"ATC 1.406", small_world::io::Section::Facility::ATC_1_406},
    {"ATC 1.910", small_world::io::Section::Facility::ATC_1_910},
    {"ATC 2.101", small_world::io::Section::Facility::ATC_2_101},
    {"ATC 2.602", small_world::io::Section::Facility::ATC_2_602},
    {"ATC 2.605", small_world::io::Section::Facility::ATC_2_605},
    {"ATC 2.705E", small_world::io::Section::Facility::ATC_2_705E},
    {"ATC 2.908", small_world::io::Section::Facility::ATC_2_908},
    {"ATC 2.914", small_world::io::Section::Facility::ATC_2_914},
    {"ATC 2.918", small_world::io::Section::Facility::ATC_2_918},
    {"ATC 3.205", small_world::io::Section::Facility::ATC_3_205},
    {"ATC 3.502", small_world::io::Section::Facility::ATC_3_502},
    {"ATC 3.601", small_world::io::Section::Facility::ATC_3_601},
    {"ATC 3.904", small_world::io::Section::Facility::ATC_3_904},
    {"ATC 3.910", small_world::io::Section::Facility::ATC_3_910},
    {"ATC 3.914", small_world::io::Section::Facility::ATC_3_914},
    {"ATC 4.902", small_world::io::Section::Facility::ATC_4_902},
    {"ATC 4.906", small_world::io::Section::Facility::ATC_4_906},
    {"ATC 4.910", small_world::io::Section::Facility::ATC_4_910},
    {"BE 2.330", small_world::io::Section::Facility::BE_2_330},
    {"BE 2.506", small_world::io::Section::Facility::BE_2_506},
    {"BE 2.520", small_world::io::Section::Facility::BE_2_520},
    {"BSB14.102J", small_world::io::Section::Facility::BSB14_102J},
    {"CB 1.102", small_world::io::Section::Facility::CB_1_102},
    {"CB 1.106", small_world::io::Section::Facility::CB_1_106},
    {"CB 1.202", small_world::io::Section::Facility::CB_1_202},
    {"CB 1.206", small_world::io::Section::Facility::CB_1_206},
    {"CB 1.210", small_world::io::Section::Facility::CB_1_210},
    {"CB 1.214", small_world::io::Section::Facility::CB_1_214},
    {"CB 1.218", small_world::io::Section::Facility::CB_1_218},
    {"CB 1.219", small_world::io::Section::Facility::CB_1_219},
    {"CB 1.222", small_world::io::Section::Facility::CB_1_222},
    {"CB 1.223", small_world::io::Section::Facility::CB_1_223},
    {"CB1 1.102", small_world::io::Section::Facility::CB1_1_102},
    {"CB1 1.104", small_world::io::Section::Facility::CB1_1_104},
    {"CB1 1.106", small_world::io::Section::Facility::CB1_1_106},
    {"CB3 1.302", small_world::io::Section::Facility::CB3_1_302},
    {"CB3 1.304", small_world::io::Section::Facility::CB3_1_304},
    {"CB3 1.306", small_world::io::Section::Facility::CB3_1_306},
    {"CB3 1.308", small_world::io::Section::Facility::CB3_1_308},
    {"CB3 1.310", small_world::io::Section::Facility::CB3_1_310},
    {"CB3 1.312", small_world::io::Section::Facility::CB3_1_312},
    {"CB3 1.314", small_world::io::Section::Facility::CB3_1_314},
    {"CBH 1.602", small_world::io::Section::Facility::CBH_1_602},
    {"CD1 A.101", small_world::io::Section::Facility::CD1_A_101},
    {"CD1 A.229", small_world::io::Section::Facility::CD1_A_229},
    {"CD1 B.108", small_world::io::Section::Facility::CD1_B_108},
    {"CD1 C.140", small_world::io::Section::Facility::CD1_C_140},
    {"CD2 J.108", small_world::io::Section::Facility::CD2_J_108},
    {"CD2 J.204", small_world::io::Section::Facility::CD2_J_204},
    {"CD2 J.213", small_world::io::Section::Facility::CD2_J_213},
    {"CHEC 106", small_world::io::Section::Facility::CHEC_106},
    {"CHEC 200", small_world::io::Section::Facility::CHEC_200},
    {"CHEC 222", small_world::io::Section::Facility::CHEC_222},
    {"CHEC 223", small_world::io::Section::Facility::CHEC_223},
    {"CR 1.202", small_world::io::Section::Facility::CR_1_202},
    {"CR 1.212", small_world::io::Section::Facility::CR_1_212},
    {"CR 1.508", small_world::io::Section::Facility::CR_1_508},
    {"CRA 11.205", small_world::io::Section::Facility::CRA_11_205},
    {"CRA 12.110", small_world::io::Section::Facility::CRA_12_110},
    {"CRA 12.120", small_world::io::Section::Facility::CRA_12_120},
    {"CRA 12.125", small_world::io::Section::Facility::CRA_12_125},
    {"ECSN 2.110", small_world::io::Section::Facility::ECSN_2_110},
    {"ECSN 2.112", small_world::io::Section::Facility::ECSN_2_112},
    {"ECSN 2.120", small_world::io::Section::Facility::ECSN_2_120},
    {"ECSN 2.126", small_world::io::Section::Facility::ECSN_2_126},
    {"ECSN 2.804", small_world::io::Section::Facility::ECSN_2_804},
    {"ECSN 3.108", small_world::io::Section::Facility::ECSN_3_108},
    {"ECSN 3.110", small_world::io::Section::Facility::ECSN_3_110},
    {"ECSN 3.112", small_world::io::Section::Facility::ECSN_3_112},
    {"ECSN 3.114", small_world::io::Section::Facility::ECSN_3_114},
    {"ECSN 3.118", small_world::io::Section::Facility::ECSN_3_118},
    {"ECSN 3.120", small_world::io::Section::Facility::ECSN_3_120},
    {"ECSN 4.324", small_world::io::Section::Facility::ECSN_4_324},
    {"ECSS 2.103", small_world::io::Section::Facility::ECSS_2_103},
    {"ECSS 2.201", small_world::io::Section::Facility::ECSS_2_201},
    {"ECSS 2.203", small_world::io::Section::Facility::ECSS_2_203},
    {"ECSS 2.305", small_world::io::Section::Facility::ECSS_2_305},
    {"ECSS 2.306", small_world::io::Section::Facility::ECSS_2_306},
    {"ECSS 2.311", small_world::io::Section::Facility::ECSS_2_311},
    {"ECSS 2.312", small_world::io::Section::Facility::ECSS_2_312},
    {"ECSS 2.410", small_world::io::Section::Facility::ECSS_2_410},
    {"ECSS 2.412", small_world::io::Section::Facility::ECSS_2_412},
    {"ECSS 2.415", small_world::io::Section::Facility::ECSS_2_415},
    {"ECSS 3.619", small_world::io::Section::Facility::ECSS_3_619},
    {"ECSS 3.910", small_world::io::Section::Facility::ECSS_3_910},
    {"ECSS 4.910", small_world::io::Section::Facility::ECSS_4_910},
    {"ECSW 1.130", small_world::io::Section::Facility::ECSW_1_130},
    {"ECSW 1.315", small_world::io::Section::Facility::ECSW_1_315},
    {"ECSW 1.355", small_world::io::Section::Facility::ECSW_1_355},
    {"ECSW 1.365", small_world::io::Section::Facility::ECSW_1_365},
    {"ECSW 2.315", small_world::io::Section::Facility::ECSW_2_315},
    {"ECSW 2.325", small_world::io::Section::Facility::ECSW_2_325},
    {"ECSW 2.335", small_world::io::Section::Facility::ECSW_2_335},
    {"ECSW 3.210", small_world::io::Section::Facility::ECSW_3_210},
    {"ECSW 3.250", small_world::io::Section::Facility::ECSW_3_250},
    {"ECSW 3.315", small_world::io::Section::Facility::ECSW_3_315},
    {"ECSW 3.325", small_world::io::Section::Facility::ECSW_3_325},
    {"ECSW 3.335", small_world::io::Section::Facility::ECSW_3_335},
    {"ECSW 4.315", small_world::io::Section::Facility::ECSW_4_315},
    {"ECSW 4.325", small_world::io::Section::Facility::ECSW_4_325},
    {"ECSW 4.335", small_world::io::Section::Facility::ECSW_4_335},
    {"FN 2.102", small_world::io::Section::Facility::FN_2_102},
    {"FN 2.104", small_world::io::Section::Facility::FN_2_104},
    {"FN 2.106", small_world::io::Section::Facility::FN_2_106},
    {"FN 2.202", small_world::io::Section::Facility::FN_2_202},
    {"FN 2.204", small_world::io::Section::Facility::FN_2_204},
    {"FN 2.210", small_world::io::Section::Facility::FN_2_210},
    {"FN 2.212", small_world::io::Section::Facility::FN_2_212},
    {"FN 2.214", small_world::io::Section::Facility::FN_2_214},
    {"FN 2.302", small_world::io::Section::Facility::FN_2_302},
    {"FN 2.304", small_world::io::Section::Facility::FN_2_304},
    {"FN 2.306", small_world::io::Section::Facility::FN_2_306},
    {"FN 3.410C", small_world::io::Section::Facility::FN_3_410C},
    {"FO 1.202", small_world::io::Section::Facility::FO_1_202},
    {"FO 1.206C", small_world::io::Section::Facility::FO_1_206C},
    {"FO 1.502", small_world::io::Section::Facility::FO_1_502},
    {"FO 2.208", small_world::io::Section::Facility::FO_2_208},
    {"FO 2.404", small_world::io::Section::Facility::FO_2_404},
    {"FO 2.608", small_world::io::Section::Facility::FO_2_608},
    {"FO 2.702", small_world::io::Section::Facility::FO_2_702},
    {"FO 3.206", small_world::io::Section::Facility::FO_3_206},
    {"FO 3.222", small_world::io::Section::Facility::FO_3_222},
    {"FO 3.616", small_world::io::Section::Facility::FO_3_616},
    {"GR 2.302", small_world::io::Section::Facility::GR_2_302},
    {"GR 2.530", small_world::io::Section::Facility::GR_2_530},
    {"GR 3.206", small_world::io::Section::Facility::GR_3_206},
    {"GR 3.302", small_world::io::Section::Facility::GR_3_302},
    {"GR 3.402A", small_world::io::Section::Facility::GR_3_402A},
    {"GR 3.402B", small_world::io::Section::Facility::GR_3_402B},
    {"GR 3.420", small_world::io::Section::Facility::GR_3_420},
    {"GR 3.602", small_world::io::Section::Facility::GR_3_602},
    {"GR 3.606", small_world::io::Section::Facility::GR_3_606},
    {"GR 4.204", small_world::io::Section::Facility::GR_4_204},
    {"GR 4.208", small_world::io::Section::Facility::GR_4_208},
    {"GR 4.301", small_world::io::Section::Facility::GR_4_301},
    {"GR 4.428", small_world::io::Section::Facility::GR_4_428},
    {"GR 4.708", small_world::io::Section::Facility::GR_4_708},
    {"GR 4.804", small_world::io::Section::Facility::GR_4_804},
    {"HH 2.402", small_world::io::Section::Facility::HH_2_402},
    {"HH 2.502", small_world::io::Section::Facility::HH_2_502},
    {"JO 1.216", small_world::io::Section::Facility::JO_1_216},
    {"JO 2.114", small_world::io::Section::Facility::JO_2_114},
    {"JO 2.504", small_world::io::Section::Facility::JO_2_504},
    {"JO 2.604", small_world::io::Section::Facility::JO_2_604},
    {"JO 3.209", small_world::io::Section::Facility::JO_3_209},
    {"JO 3.516", small_world::io::Section::Facility::JO_3_516},
    {"JO 3.532", small_world::io::Section::Facility::JO_3_532},
    {"JO 3.536", small_world::io::Section::Facility::JO_3_536},
    {"JO 3.906", small_world::io::Section::Facility::JO_3_906},
    {"JO 3.908", small_world::io::Section::Facility::JO_3_908},
    {"JO 4.102", small_world::io::Section::Facility::JO_4_102},
    {"JO 4.112", small_world::io::Section::Facility::JO_4_112},
    {"JO 4.502", small_world::io::Section::Facility::JO_4_502},
    {"JO 4.504", small_world::io::Section::Facility::JO_4_504},
    {"JO 4.614", small_world::io::Section::Facility::JO_4_614},
    {"JO 4.708", small_world::io::Section::Facility::JO_4_708},
    {"MC 2.410", small_world::io::Section::Facility::MC_2_410},
    {"MC 2.524", small_world::io::Section::Facility::MC_2_524},
    {"ML1 1.106", small_world::io::Section::Facility::ML1_1_106},
    {"ML1 1.110", small_world::io::Section::Facility::ML1_1_110},
    {"ML1 1.114", small_world::io::Section::Facility::ML1_1_114},
    {"ML1 1.118", small_world::io::Section::Facility::ML1_1_118},
    {"ML1 1.122", small_world::io::Section::Facility::ML1_1_122},
    {"ML2 1.202", small_world::io::Section::Facility::ML2_1_202},
    {"ML2 1.214", small_world::io::Section::Facility::ML2_1_214},
    {"ML2 1.216", small_world::io::Section::Facility::ML2_1_216},
    {"ML2 1.218", small_world::io::Section::Facility::ML2_1_218},
    {"PHY 1.103", small_world::io::Section::Facility::PHY_1_103},
    {"PHY 1.202", small_world::io::Section::Facility::PHY_1_202},
    {"PHY 1.304", small_world::io::Section::Facility::PHY_1_304},
    {"PS3 1.540", small_world::io::Section::Facility::PS3_1_540},
    {"RHNW 1.001", small_world::io::Section::Facility::RHNW_1_001},
    {"RHNW 2.002", small_world::io::Section::Facility::RHNW_2_002},
    {"RHW 2.400", small_world::io::Section::Facility::RHW_2_400},
    {"RHW 2.401", small_world::io::Section::Facility::RHW_2_401},
    {"RHW 2.402", small_world::io::Section::Facility::RHW_2_402},
    {"RHW 4.100", small_world::io::Section::Facility::RHW_4_100},
    {"ROC 1.401", small_world::io::Section::Facility::ROC_1_401},
    {"ROC 2.103", small_world::io::Section::Facility::ROC_2_103},
    {"ROC 2.301T", small_world::io::Section::Facility::ROC_2_301T},
    {"ROC 2.701", small_world::io::Section::Facility::ROC_2_701},
    {"ROW 1.141", small_world::io::Section::Facility::ROW_1_141},
    {"SLC 1.102", small_world::io::Section::Facility::SLC_1_102},
    {"SLC 1.202", small_world::io::Section::Facility::SLC_1_202},
    {"SLC 1.204", small_world::io::Section::Facility::SLC_1_204},
    {"SLC 1.206", small_world::io::Section::Facility::SLC_1_206},
    {"SLC 1.211", small_world::io::Section::Facility::SLC_1_211},
    {"SLC 1.214", small_world::io::Section::Facility::SLC_1_214},
    {"SLC 2.202", small_world::io::Section::Facility::SLC_2_202},
    {"SLC 2.203", small_world::io::Section::Facility::SLC_2_203},
    {"SLC 2.206", small_world::io::Section::Facility::SLC_2_206},
    {"SLC 2.207", small_world::io::Section::Facility::SLC_2_207},
    {"SLC 2.215", small_world::io::Section::Facility::SLC_2_215},
    {"SLC 2.216", small_world::io::Section::Facility::SLC_2_216},
    {"SLC 2.302", small_world::io::Section::Facility::SLC_2_302},
    {"SLC 2.303", small_world::io::Section::Facility::SLC_2_303},
    {"SLC 2.304", small_world::io::Section::Facility::SLC_2_304},
    {"SLC 3.102", small_world::io::Section::Facility::SLC_3_102},
    {"SLC 3.202", small_world::io::Section::Facility::SLC_3_202},
    {"SLC 3.203", small_world::io::Section::Facility::SLC_3_203},
    {"SLC 3.210", small_world::io::Section::Facility::SLC_3_210},
    {"SLC 3.215", small_world::io::Section::Facility::SLC_3_215},
    {"SLC 3.220", small_world::io::Section::Facility::SLC_3_220},
    {"SOM 1.102", small_world::io::Section::Facility::SOM_1_102},
    {"SOM 1.107", small_world::io::Section::Facility::SOM_1_107},
    {"SOM 1.110", small_world::io::Section::Facility::SOM_1_110},
    {"SOM 1.117", small_world::io::Section::Facility::SOM_1_117},
    {"SOM 1.118", small_world::io::Section::Facility::SOM_1_118},
    {"SOM 1.211", small_world::io::Section::Facility::SOM_1_211},
    {"SOM 1.212", small_world::io::Section::Facility::SOM_1_212},
    {"SOM 1.217", small_world::io::Section::Facility::SOM_1_217},
    {"SOM 1.302", small_world::io::Section::Facility::SOM_1_302},
    {"SOM 1.502", small_world::io::Section::Facility::SOM_1_502},
    {"SOM 1.508", small_world::io::Section::Facility::SOM_1_508},
    {"SOM 1.516", small_world::io::Section::Facility::SOM_1_516},
    {"SOM 11.202", small_world::io::Section::Facility::SOM_11_202},
    {"SOM 11.206", small_world::io::Section::Facility::SOM_11_206},
    {"SOM 11.210", small_world::io::Section::Facility::SOM_11_210},
    {"SOM 12.202", small_world::io::Section::Facility::SOM_12_202},
    {"SOM 12.206", small_world::io::Section::Facility::SOM_12_206},
    {"SOM 12.210", small_world::io::Section::Facility::SOM_12_210},
    {"SOM 12.214", small_world::io::Section::Facility::SOM_12_214},
    {"SOM 12.218", small_world::io::Section::Facility::SOM_12_218},
    {"SOM 12.222", small_world::io::Section::Facility::SOM_12_222},
    {"SOM 13.501", small_world::io::Section::Facility::SOM_13_501},
    {"SOM 13.510", small_world::io::Section::Facility::SOM_13_510},
    {"SOM 14.501", small_world::io::Section::Facility::SOM_14_501},
    {"SOM 2.102", small_world::io::Section::Facility::SOM_2_102},
    {"SOM 2.103", small_world::io::Section::Facility::SOM_2_103},
    {"SOM 2.106", small_world::io::Section::Facility::SOM_2_106},
    {"SOM 2.107", small_world::io::Section::Facility::SOM_2_107},
    {"SOM 2.112", small_world::io::Section::Facility::SOM_2_112},
    {"SOM 2.115", small_world::io::Section::Facility::SOM_2_115},
    {"SOM 2.116", small_world::io::Section::Facility::SOM_2_116},
    {"SOM 2.117", small_world::io::Section::Facility::SOM_2_117},
    {"SOM 2.402", small_world::io::Section::Facility::SOM_2_402},
    {"SOM 2.714", small_world::io::Section::Facility::SOM_2_714},
    {"SOM 2.717", small_world::io::Section::Facility::SOM_2_717},
    {"SOM 2.722", small_world::io::Section::Facility::SOM_2_722},
    {"SOM 2.801", small_world::io::Section::Facility::SOM_2_801},
    {"SOM 2.802", small_world::io::Section::Facility::SOM_2_802},
    {"SOM 2.803", small_world::io::Section::Facility::SOM_2_803},
    {"SOM 2.804", small_world::io::Section::Facility::SOM_2_804},
    {"SOM 2.901", small_world::io::Section::Facility::SOM_2_901},
    {"SOM 2.902", small_world::io::Section::Facility::SOM_2_902},
    {"SOM 2.903", small_world::io::Section::Facility::SOM_2_903},
    {"SOM 2.904", small_world::io::Section::Facility::SOM_2_904},
    {"SPN 1.115", small_world::io::Section::Facility::SPN_1_115},
    {"SPN 1.121", small_world::io::Section::Facility::SPN_1_121},
    {"SPN 1.221", small_world::io::Section::Facility::SPN_1_221},
    {"SSA 13.330", small_world::io::Section::Facility::SSA_13_330},
    {"SSB 3.107B", small_world::io::Section::Facility::SSB_3_107B},
    {"TH 2.702", small_world::io::Section::Facility::TH_2_702}
  };
  auto it = MAP.find(str);
  assert(it != MAP.end());
  return it->second;
}

small_world::io::Section::Mode Parse<small_world::io::Section::Mode>::parse(const std::string& str) {
  uint16_t ans = ::parse<uint16_t>(str);
  assert(ans == 1 || ans == 2 || ans == 6);
  return static_cast<small_world::io::Section::Mode>(ans);
}

// Parses a date in M/D/YYYY format
time_t Parse<Date>::parse(const std::string& str) {
  std::size_t pos1 = str.find('/');
  assert(pos1 != std::string::npos);
  std::size_t pos2 = str.find('/', pos1 + 1);
  assert(pos2 != std::string::npos);

  tm time;
  time.tm_sec = 0;
  time.tm_min = 0;
  time.tm_hour = 0;
  time.tm_mday = ::parse<int>(str.substr(pos1 + 1, pos2 - (pos1 + 1)));
  time.tm_mon = ::parse<int>(str.substr(0, pos1));
  time.tm_year = ::parse<int>(str.substr(pos2 + 1));
  // tm_wday and tm_yday are ignored by mktime
  time.tm_isdst = -1;
  return mktime(&time);
}

// Parses a time in 24-hour hh:mm format
time_t Parse<Time>::parse(const std::string& str) {
  if (str.empty())
    return 0;
  std::size_t pos = str.find(':');
  assert(pos != std::string::npos);

  tm time;
  time.tm_sec = 0;
  time.tm_min = ::parse<int>(str.substr(pos + 1));
  time.tm_hour = ::parse<int>(str.substr(0, pos));
  time.tm_mday = 1;
  time.tm_mon = 0;
  time.tm_year = 70;
  // tm_wday and tm_yday are ignored by mktime
  time.tm_isdst = -1;
  return mktime(&time);
}
