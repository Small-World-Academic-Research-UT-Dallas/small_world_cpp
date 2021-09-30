#include <algorithm> // std::transform
#include <memory>
#include <vector>

#include "small_world_simulation/EnrollmentData.hpp"
#include "small_world_simulation/StudentRecord.hpp"
#include "small_world_simulation/SectionRecord.hpp"
#include "small_world_io/EnrollmentDataReader.hpp"
#include "small_world_io/Student.hpp"
#include "small_world_io/Section.hpp"

using small_world::simulation::EnrollmentData;

// Construct from EnrollmentData by converting io::Student into
// simulation::StudentRecord. Also similar operation on sections, of course
EnrollmentData::EnrollmentData(const small_world::io::EnrollmentDataReader & data) {
  auto student_input = data.get_students();
  std::transform(student_input->begin(), student_input->end(), this->students->begin(),
    [](const small_world::io::Student & input) -> small_world::simulation::StudentRecord {
      return small_world::simulation::StudentRecord(input);
    }
  );

  auto section_input = data.get_sections();
  std::transform(section_input->cbegin(), section_input->cend(), this->sections->begin(),
    [](const small_world::io::Section & input) -> small_world::simulation::SectionRecord {
      return small_world::simulation::SectionRecord(input);
    }
  );
}

EnrollmentData::~EnrollmentData() {

}

// Access to const versions of the student and section records
std::shared_ptr<const std::vector<small_world::simulation::StudentRecord>> EnrollmentData::get_students() const {
  return this->students;
}

std::shared_ptr<const std::vector<small_world::simulation::SectionRecord>> EnrollmentData::get_sections() const {
  return this->sections;
}
