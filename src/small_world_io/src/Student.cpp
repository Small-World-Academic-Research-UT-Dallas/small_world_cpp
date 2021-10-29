#include "small_world_io/Student.hpp"

using small_world::io::Student;

Student::Student(std::shared_ptr<const std::vector<std::size_t>> sections,
                 std::size_t strm,
                 std::size_t unique_val,
                 Career career,
                 Program program,
                 Organization organization,
                 Group group,
                 Plan plan,
                 Subplan subplan) :
  sections(sections),
  strm(strm),
  unique_val(unique_val),
  career(career),
  program(program),
  organization(organization),
  group(group),
  plan(plan),
  subplan(subplan)
{}

Student::~Student() {}

std::shared_ptr<const std::vector<std::size_t>> Student::get_sections() const {
  return this->sections;
}

std::size_t Student::get_strm() const {
  return this->strm;
}

std::size_t Student::get_unique_val() const {
  return this->unique_val;
}

Student::Career Student::get_career() const {
  return this->career;
}

Student::Program Student::get_program() const {
  return this->program;
}

Student::Organization Student::get_organization() const {
  return this->organization;
}

Student::Group Student::get_group() const {
  return this->group;
}

Student::Plan Student::get_plan() const {
  return this->plan;
}

Student::Subplan Student::get_subplan() const {
  return this->subplan;
}

bool operator==(const small_world::io::Student& l, const small_world::io::Student& r) {
  return *l.get_sections() == *r.get_sections() &&
         l.get_strm() == r.get_strm() &&
         l.get_unique_val() == r.get_unique_val() &&
         l.get_career() == r.get_career() &&
         l.get_program() == r.get_program() &&
         l.get_organization() == r.get_organization() &&
         l.get_group() == r.get_group() &&
         l.get_plan() == r.get_plan() &&
         l.get_subplan() == r.get_subplan();
}

bool operator!=(const small_world::io::Student& l, const small_world::io::Student& r) {
  return !(l == r);
}
