#pragma once

#include <cassert>
#include <tuple>
#include <type_traits>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Section.hpp>
#include <small_world_io/Student.hpp>

namespace small_world::static_analysis {

template<bool Const>
struct ConstFilter {
  inline constexpr bool operator()(const small_world::io::Student& student) const noexcept {
    return Const;
  }

  inline constexpr bool operator()(const small_world::io::Section& section) const noexcept {
    return Const;
  }
};

struct StrmFilter {
  size_t strm;

  explicit inline StrmFilter(size_t strm) :
    strm(strm)
  {}

  inline bool operator()(const small_world::io::Student& student) const noexcept {
    return student.get_strm() == strm;
  }

  inline bool operator()(const small_world::io::Section& section) const noexcept {
    return section.get_strm() == strm;
  }
};

template<typename Filter>
inline constexpr void filterStudents(std::vector<bool>& ans, const io::EnrollmentDataReader& data, const Filter& filter = Filter{}) {
  const std::vector<io::Student>& students = *data.get_students();
  assert(ans.size() == students.size());
  for (size_t i = 0; i < students.size(); i++)
    ans[i] = ans[i] & filter(students[i]);
}

template<typename Filter>
inline constexpr void filterSections(std::vector<bool>& ans, const io::EnrollmentDataReader& data, const Filter& filter = Filter{}) {
  const std::vector<io::Section>& sections = *data.get_sections();
  assert(ans.size() == sections.size());
  for (size_t i = 0; i < sections.size(); i++)
    ans[i] = ans[i] & filter(sections[i]);
}

// TODO other fields, logical operators

}